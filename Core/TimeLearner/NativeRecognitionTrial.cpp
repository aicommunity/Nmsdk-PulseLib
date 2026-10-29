#include "NativeRecognitionTrial.h"
#include "NativeModelBuilder.h"
#include "NativeModelEditor.h"
#include "NativeTrainingBindings.h"
#include "ModelIdentity.h"
#include "NumericChecks.h"
#include <QJsonArray>
#include <algorithm>

namespace NMSDK::TimeLearner {
namespace {
using namespace RDK;
void Require(bool ok,const char* reason) { if(!ok)throw std::runtime_error(reason); }
std::map<std::string,std::string> Parameters(NPulseGeneratorTransit& source)
{
    std::map<std::string,std::string> values{{"$class",source.GetCompClassName()}};
    for(const auto& name:source.GetPropertiesOrder())
    {
        const auto p=source.FindProperty(name);
        if(p && (p->GetType() & ptParameter) && name!="Frequency" && name!="Delay")
            source.GetPropertyValue(name,values[name]);
    }
    return values;
}
void Events(const MDMatrix<double>& history,double time,double begin,
    std::optional<double>& watermark,std::vector<double>& out)
{
    Require(history.GetRows()==1,"invalid_recognition_event_dimensions");
    std::optional<double> previous;
    for(int i=0;i<history.GetCols();++i)
    {
        const double event=history(0,i);
        Require(Detail::Finite(event) && !Detail::After(event,time) &&
            (!previous || event>*previous),"invalid_recognition_event_history");
        previous=event;
        if((!watermark || event>*watermark) && !Detail::After(begin,event))out.push_back(event);
    }
    if(previous && (!watermark || *previous>*watermark))watermark=previous;
}
QJsonArray Numbers(const std::vector<double>& values)
{ QJsonArray a;for(double v:values)a.append(v);return a; }
}

void NativeRecognitionTrialExecutor::Queue(NativeRecognitionTrialRequest request)
{
    if(Busy() || result)throw std::logic_error("recognition_trial_busy_or_unconsumed");
    Require(!request.runId.empty() && !request.trialId.empty() && !request.sampleId.empty() &&
        (request.label=="target" || request.label=="foil"),"invalid_recognition_trial_identity");
    Require(request.resetPolicy==RecognitionResetPolicy::PerTrial ||
        request.resetPolicy==RecognitionResetPolicy::PreserveNeuronDynamics,"invalid_recognition_reset_policy");
    Require(Detail::Finite(request.duration) && request.duration>0. && request.maximumSamples>=2,
        "invalid_recognition_trial_budget");
    Require(request.pattern.GetCols()==1 && request.pattern.GetRows()>0,"invalid_recognition_pattern_shape");
    QJsonArray values;
    for(int i=0;i<request.pattern.GetRows();++i)
    {
        const double v=request.pattern(i,0);Require(Detail::Finite(v) && v>=0.,"invalid_recognition_pattern_value");
        values.append(v);
    }
    Require(NmsdkTools::CanonicalModelDataSha256(values).toStdString()==request.patternHash,"recognition_pattern_hash_mismatch");
    Require(seen.insert({request.runId,request.trialId}).second,"reused_recognition_trial_id");
    current.emplace();current->request=std::move(request);queued=true;
    physical.reset();watched.clear();sourceParameters.clear();source={};zone={};
    lastStimulus.reset();lastSpike.reset();lastSample.reset();saved=false;
}

void NativeRecognitionTrialExecutor::Check(const NativeProbeBindings& b) const
{
    Require(physical && ownerLife.IsCurrent() && b.owner==physical->owner &&
        b.neuron.Get()==physical->neuron.Get() && b.dataset.Get()==physical->dataset.Get() && b.somas==physical->somas,
        "recognition_trial_graph_or_lifecycle_changed");
    for(const auto& node:watched)
    {
        Require(node.lifecycle.IsAlive() && b.owner->GetComponentL(node.path,true).Get()==node.node.Get(),
            "recognition_trial_graph_replaced");
        Require(node.node->IsReady() && node.node->IsInit() &&
            double(node.node->GetTimeStep())==current->frequencyHz,"unready_or_multirate_recognition_graph");
        // Dataset's one-shot playback intentionally resets its generator.
        if(node.node.Get()!=source.Get())Require(node.lifecycle.IsCurrent(),"recognition_trial_lifecycle_changed");
    }
    for(auto parent=b.owner->GetOwner();parent;parent=parent->GetOwner())
        Require(double(parent->GetTimeStep())==current->frequencyHz,"unsupported_recognition_multirate");
    Require(double(b.owner->GetTimeStep())==current->frequencyHz &&
        b.dataset->NumSamples.GetData()==1 && b.dataset->NumFeatures.GetData()==1 &&
        b.dataset->MaxSpikesPerFeature.GetData()==current->request.pattern.GetRows() && b.dataset->Iteration.GetData()==0 &&
        NativeTrainingPatternEqual(b.dataset->MatrixData.GetData(),current->request.pattern),"recognition_presentation_changed");
    Require(Parameters(*source)==sourceParameters && source->Delay.GetData()==0. &&
        b.dataset->StateGeneration.GetData()==2 && !b.dataset->AdvanceSampleAfterBurst.GetData() &&
        b.dataset->Delay.GetData()==std::max(oldDelay,current->request.duration+2./current->frequencyHz),
        "recognition_stimulus_control_changed");
    const double expected=!b.dataset->ResetDelay.GetData() && b.dataset->GetOneShotEndTime(0)>0.
        ?1./source->PulseLength.GetData():0.;
    Require(source->Frequency.GetData()==expected,"recognition_generator_control_changed");
    Require(revision(false)==current->modelBefore,"recognition_model_changed");
}

void NativeRecognitionTrialExecutor::BeforeChildren(const NativeProbeBindings& b,double time,
    const std::function<void(const MDMatrix<double>&)>& applyPresentation,
    const std::function<std::string(bool)>& readRevision)
{
    if(!Busy())return;
    try
    {
        if(running){Check(b);return;}
        Require(b.owner && b.neuron && b.dataset && b.inputCount>0 && bool(readRevision),"missing_recognition_trial_graph");
        revision=readRevision;current->modelBefore=revision(true);
        Require(!current->modelBefore.empty(),"missing_recognition_model_revision");
        Require(current->request.pattern.GetRows()==b.inputCount,"recognition_input_dimension_mismatch");
        const double frequency=b.owner->GetTimeStep();
        Require(Detail::Finite(frequency) && frequency>0. && Detail::Finite(time),"invalid_recognition_clock");
        current->frequencyHz=frequency;current->begin=time;current->end=time+current->request.duration;
        Require(Detail::ResolvableTimeInterval(time,current->end,1./frequency),"unresolvable_recognition_clock");
        const double steps=current->request.duration*frequency;
        Require(Detail::Finite(steps) && !Detail::After(steps,std::round(steps)) && !Detail::After(std::round(steps),steps) &&
            steps+1.<=double(current->request.maximumSamples),"recognition_window_grid_or_budget");
        auto generator=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        Require(bool(generator),"missing_recognition_generator");
        Require(!generator->UseTransitSignal.GetData() && !generator->UsePatternOutput.GetData() &&
            generator->FrequencyDeviation.GetData()==0. && generator->Delay.GetData()==0. &&
            Detail::Finite(generator->PulseLength.GetData()) && generator->PulseLength.GetData()>=1./frequency &&
            Detail::Finite(generator->Amplitude.GetData()) && generator->Amplitude.GetData()>0.,"unsupported_recognition_generator");
        const auto integration=AssessNativeIntegration(b.neuron,generator->Amplitude.GetData());
        Require(integration.valid,"unsafe_recognition_integration");
        double span=0.;
        for(int i=0;i<b.inputCount;++i)
        {
            const double isi=current->request.pattern(i,0);span+=isi;
            Require(i==0 || !Detail::After(generator->PulseLength.GetData(),isi),"overlapping_recognition_stimuli");
        }
        Require(!Detail::After(span+generator->PulseLength.GetData()+1./frequency,current->request.duration),
            "recognition_window_before_stimulus_end");
        Require(!b.dataset->MatrixData.IsConnected() && !b.dataset->MatrixData.UsesExternalDataSource() &&
            !b.dataset->Delay.IsConnected() && !b.dataset->Delay.UsesExternalDataSource() &&
            !b.dataset->AdvanceSampleAfterBurst.IsConnected() && !b.dataset->AdvanceSampleAfterBurst.UsesExternalDataSource() &&
            !b.dataset->StateGeneration.IsConnected() && !b.dataset->StateGeneration.UsesExternalDataSource() &&
            !generator->Frequency.IsConnected() && !generator->Frequency.UsesExternalDataSource() &&
            !generator->Delay.IsConnected() && !generator->Delay.UsesExternalDataSource(),
            "recognition_control_is_external");
        oldDelay=b.dataset->Delay.GetData();oldAdvance=b.dataset->AdvanceSampleAfterBurst.GetData();
        physical.emplace(b);ownerLife=b.owner->ObserveLifecycle();saved=true;
        applyPresentation(current->request.pattern);
        source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        zone=b.neuron->GetComponentL<NPulseLTZoneCommon>("LTZone",true);
        Require(source && zone,"unsupported_recognition_ltzone");
        b.dataset->Reset();
        if(current->request.resetPolicy==RecognitionResetPolicy::PerTrial)b.neuron->Reset();
        else
        {
            std::vector<double> prior;
            Events(zone->OutputPulseTimes.GetData(),time,time,lastSpike,prior);
        }
        Require(current->request.resetPolicy!=RecognitionResetPolicy::PerTrial || zone->OutputPulseTimes.GetCols()==0,
            "recognition_reset_retained_spikes");
        b.dataset->Delay=std::max(oldDelay,current->request.duration+2./frequency);
        b.dataset->AdvanceSampleAfterBurst=false;b.dataset->StateGeneration=2;
        sourceParameters=Parameters(*source);
        std::set<UContainer*> seenNodes;
        std::function<void(UEPtr<UContainer>)> collect=[&](UEPtr<UContainer> node) {
            Require(node && seenNodes.insert(node.Get()).second,"invalid_recognition_tree");
            watched.push_back({node,node->GetLongName(b.owner),node->ObserveLifecycle()});
            std::vector<NameT> names;node->GetComponentsList(names);
            for(const auto& name:names)collect(node->GetComponent(name,true));
        };
        collect(b.neuron);collect(b.dataset);
        Require(b.somas.size()==(b.branch?1u:std::size_t(b.inputCount)),"missing_recognition_somas");
        Require(revision(true)==current->modelBefore,"recognition_prepare_changed_model");
        current->armed=true;queued=false;running=true;Check(b);
    }
    catch(const std::exception& e){Finish(e.what(),false);}
}

void NativeRecognitionTrialExecutor::AfterChildren(const NativeProbeBindings& b,double time)
{
    if(!running)return;
    try
    {
        Check(b);
        Require(Detail::Finite(time) && (!lastSample || (time>*lastSample &&
            !Detail::IntervalAfter(time,*lastSample,1./current->frequencyHz))),"recognition_sample_gap");
        Require(lastSample || (!Detail::After(time,current->begin) && !Detail::After(current->begin,time)),
            "recognition_missing_first_sample");
        Require(current->samples.size()<current->request.maximumSamples,"recognition_sample_budget_exhausted");
        NativeRecognitionTrialSample sample;sample.time=time;
        Require(source->Output.GetRows()==1 && source->Output.GetCols()==1 &&
            Detail::Finite(source->Output(0,0)),"invalid_recognition_stimulus_sample");
        sample.stimulus=source->Output(0,0);
        for(const auto& soma:b.somas)
        {
            Require(soma && soma->SumPotential.GetRows()==1 && soma->SumPotential.GetCols()==1 &&
                Detail::Finite(soma->SumPotential(0,0)),"invalid_recognition_soma_sample");
            sample.somaPotentials.push_back(soma->SumPotential(0,0));
        }
        Events(source->OutputPulseTimes.GetData(),time,current->begin,lastStimulus,current->stimulusTimes);
        Events(zone->OutputPulseTimes.GetData(),time,current->begin,lastSpike,current->spikeTimes);
        current->samples.push_back(std::move(sample));lastSample=time;current->closedAt=time;
        if(!Detail::After(current->end,time))
        {
            Require(!Detail::After(time,current->end),"recognition_window_overshoot");
            current->complete=true;
            Require(current->stimulusTimes.size()==std::size_t(b.inputCount),"recognition_stimulus_count_mismatch");
            double planned=current->begin;
            for(int i=0;i<b.inputCount;++i)
            {
                planned+=current->request.pattern(i,0);
                Require(!Detail::After(planned,current->stimulusTimes[i]) &&
                    !Detail::After(current->stimulusTimes[i],planned+1./current->frequencyHz),
                    "recognition_stimulus_timestamp_mismatch");
            }
            Finish("window_closed",true,true);
        }
    }
    catch(const std::exception& e){Finish(e.what(),current && current->complete);}
}

void NativeRecognitionTrialExecutor::Stop()
{
    if(!physical || !saved || !ownerLife.IsAlive())return;
    // No stale weak handle is dereferenced without proving membership/lifetime.
    for(const auto& node:watched)Require(node.lifecycle.IsAlive() &&
        physical->owner->GetComponentL(node.path,true).Get()==node.node.Get(),"recognition_stop_graph_replaced");
    Require(physical->owner->GetComponentL("DatasetMatrix",true).Get()==physical->dataset.Get(),"recognition_stop_dataset_replaced");
    physical->dataset->StateGeneration=0;
    if(source){source->Frequency=0.;source->Reset();}
    physical->dataset->Delay=oldDelay;physical->dataset->AdvanceSampleAfterBurst=oldAdvance;saved=false;
}
void NativeRecognitionTrialExecutor::Finish(const std::string& reason,bool complete,bool valid)
{
    if(!current)return;
    current->reason=reason;current->complete=complete;current->valid=valid && complete && current->armed;
    try { if(revision)current->modelAfter=revision(true); }
    catch(const std::exception& e){current->valid=false;current->reason+=";"+std::string(e.what());}
    if(current->modelAfter!=current->modelBefore)current->valid=false;
    try { Stop(); }
    catch(const std::exception& e){current->valid=false;current->reason+=";"+std::string(e.what());}
    result=std::move(current);current.reset();queued=false;running=false;
    revision={};physical.reset();watched.clear();source={};zone={};
}
void NativeRecognitionTrialExecutor::Cancel(const std::string& reason)
{ if(Busy())Finish(reason,false); }
std::optional<NativeRecognitionTrialObservation> NativeRecognitionTrialExecutor::TakeResult()
{ auto value=std::move(result);result.reset();return value; }

QJsonObject NativeRecognitionTrialJson(const NativeRecognitionTrialObservation& o)
{
    QJsonArray pattern;for(int i=0;i<o.request.pattern.GetRows();++i)pattern.append(o.request.pattern(i,0));
    QJsonArray samples;for(const auto& s:o.samples)samples.append(QJsonObject{
        {"time",s.time},{"stimulus",s.stimulus},{"soma",Numbers(s.somaPotentials)}});
    QJsonArray relativeStimuli,relativeSpikes;
    if(!o.stimulusTimes.empty())
    {
        for(double t:o.stimulusTimes)relativeStimuli.append(t-o.stimulusTimes.front());
        for(double t:o.spikeTimes)relativeSpikes.append(t-o.stimulusTimes.front());
    }
    return {{"kind","native_recognition_trial"},{"schema_version",1},
        {"run_id",QString::fromStdString(o.request.runId)},{"trial_id",QString::fromStdString(o.request.trialId)},
        {"sample_id",QString::fromStdString(o.request.sampleId)},{"label",QString::fromStdString(o.request.label)},
        {"pattern_hash",QString::fromStdString(o.request.patternHash)},{"pattern_isi",pattern},
        {"reset_policy",o.request.resetPolicy==RecognitionResetPolicy::PerTrial?"reset_per_trial":"preserve_neuron_dynamics"},
        {"frequency_hz",o.frequencyHz},{"begin",o.begin},{"end",o.end},{"closed_at",o.closedAt},
        {"maximum_samples",double(o.request.maximumSamples)},
        {"model_before",QString::fromStdString(o.modelBefore)},{"model_after",QString::fromStdString(o.modelAfter)},
        {"armed",o.armed},{"complete",o.complete},{"censored",!o.complete},{"valid",o.valid},
        {"reason",QString::fromStdString(o.reason)},
        {"stimulus_onset",o.stimulusTimes.empty()?QJsonValue(QJsonValue::Null):QJsonValue(o.stimulusTimes.front())},
        {"stimulus_times",Numbers(o.stimulusTimes)},{"spike_times",Numbers(o.spikeTimes)},
        {"relative_stimulus_times",relativeStimuli},{"relative_spike_times",relativeSpikes},
        {"spike_count",double(o.spikeTimes.size())},{"samples",samples},
        {"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
}
}
