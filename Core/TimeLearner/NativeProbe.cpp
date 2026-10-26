#include "NativeProbe.h"
#include "NumericChecks.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
using namespace RDK;
void Require(bool condition,const char* reason)
{ if(!condition)throw std::runtime_error(reason); }
void Collect(UEPtr<UContainer> node,std::vector<UEPtr<UContainer>>& nodes,std::set<UContainer*>& seen)
{
    Require(node && seen.insert(node.Get()).second,"invalid_native_tree");
    nodes.push_back(node);std::vector<NameT> names;node->GetComponentsList(names);
    for(const auto& name:names)Collect(node->GetComponent(name,true),nodes,seen);
}
NPulseGeneratorTransit* Generator(const NativeProbeBindings& b)
{
    Require(bool(b.dataset),"missing_native_dataset");
    auto generator=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"missing_native_generator");return generator.Get();
}
std::map<std::string,std::string> SourceParameters(NPulseGeneratorTransit* source)
{
    std::map<std::string,std::string> values{{"$class",source->GetCompClassName()}};
    for(const auto& name:source->GetPropertiesOrder())
    {
        const auto property=source->FindProperty(name);
        if(property && (property->GetType() & ptParameter) && name!="Frequency" && name!="Delay")
            source->GetPropertyValue(name,values[name]);
    }
    return values;
}
}

void NativeProbeExecutor::Queue(NativeProbeRequest value)
{
    if(state==NativeProbeStatus::Running || state==NativeProbeStatus::Queued || result)
        throw std::logic_error("native_probe_busy_or_unconsumed_result");
    const auto key=std::make_pair(value.specification.runId,value.specification.trialId);
    if(!seenTrials.insert(key).second)throw std::logic_error("reused_native_trial_id");
    request=std::move(value);state=NativeProbeStatus::Queued;prepared.reset();
    lastEmission.reset();emitted=0;watched.clear();generatorParameters.clear();capturedGenerator=nullptr;
    ownerLife={};neuronLife={};datasetLife={};generatorLife={};
}

bool NativeProbeExecutor::SameLiveRoots() const
{
    if(!physical || !ownerLife.IsAlive() || !neuronLife.IsAlive() || !datasetLife.IsAlive())return false;
    auto neuron=physical->owner->GetComponentL<NPulseNeuron>("Neuron",true);
    auto dataset=physical->owner->GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    return neuron.Get()==physical->neuron.Get() && dataset.Get()==physical->dataset.Get() &&
        (!capturedGenerator || dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true).Get()==capturedGenerator);
}
bool NativeProbeExecutor::CanReadModel() const
{
    return SameLiveRoots() && generatorLife.IsAlive() &&
        std::all_of(watched.begin(),watched.end(),[this](const auto& node){
            return node.lifecycle.IsAlive() && physical->owner->GetComponentL(node.path,true).Get()==node.node.Get();
        });
}
void NativeProbeExecutor::CheckLiveGraph(const NativeProbeBindings& b) const
{
    Require(SameLiveRoots() && b.neuron.Get()==physical->neuron.Get() &&
        b.dataset.Get()==physical->dataset.Get() && b.somas==physical->somas,"native_graph_replaced");
    Require(ownerLife.IsCurrent(),"native_lifecycle_changed");
    for(const auto& node:watched)
    {
        Require(node.lifecycle.IsAlive(),"native_graph_destroyed");
        Require(b.owner->GetComponentL(node.path,true).Get()==node.node.Get(),"native_graph_replaced");
        Require(node.node->IsReady() && node.node->IsInit(),"native_graph_not_ready");
        Require(!node.stable || node.lifecycle.IsCurrent(),"native_lifecycle_changed");
    }
}
void NativeProbeExecutor::CheckSource(const NativeProbeBindings& b) const
{
    for(auto ancestor=b.owner;ancestor;ancestor=dynamic_cast<UNet*>(ancestor->GetOwner().Get()))
        Require(double(ancestor->GetTimeStep())==frequency,"unsupported_native_multirate");
    Require(double(b.dataset->GetTimeStep())==frequency,"unsupported_native_multirate");
    auto source=Generator(b);
    Require(SourceParameters(source)==generatorParameters && source->Delay.GetData()==0.,
        "native_generator_configuration_changed");
    const double expected=!b.dataset->ResetDelay.GetData() && b.dataset->GetOneShotEndTime(0)>0.
        ?1./source->PulseLength.GetData():0.;
    Require(source->Frequency.GetData()==expected,"native_generator_control_changed");
    Require(b.dataset->StateGeneration.GetData()==2 && !b.dataset->AdvanceSampleAfterBurst.GetData() &&
        b.dataset->Delay.GetData()==std::max(oldDelay,request.specification.end+2./frequency),
        "native_dataset_control_changed");
}

void NativeProbeExecutor::CheckPattern(const NativeProbeBindings& b) const
{
    Require(b.dataset && b.dataset->NumSamples.GetData()==1 && b.dataset->NumFeatures.GetData()==1 &&
        b.dataset->Iteration.GetData()==0 && b.dataset->MaxSpikesPerFeature.GetData()==b.inputCount &&
        b.dataset->MatrixData.GetRows()==b.inputCount && b.dataset->MatrixData.GetCols()==1 &&
        request.expectedInputIsi.size()==std::size_t(b.inputCount),"unsupported_native_dataset_shape");
    for(int i=0;i<b.inputCount;++i)
        Require(Detail::Finite(request.expectedInputIsi[i]) && request.expectedInputIsi[i]>=0. &&
            b.dataset->MatrixData(i,0)==request.expectedInputIsi[i],"native_pattern_changed");
}

void NativeProbeExecutor::BeforeChildren(const NativeProbeBindings& b,double time)
{
    if(state==NativeProbeStatus::Running)
    {
        try
        {
            Require(Detail::Finite(time),"invalid_native_time");
            CheckLiveGraph(b);CheckPattern(b);CheckSource(b);
            Require(prepared && request.readModelRevision()==prepared->effectiveModelRevision,"model_revision_changed");
        }
        catch(const std::exception& error){Failure(time,error.what());}
        catch(...){Failure(time,"native_boundary_exception");}
        return;
    }
    if(state!=NativeProbeStatus::Queued)return;
    lastTime=time;
    try
    {
        Require(b.owner && b.neuron && b.dataset && b.inputCount>0,"missing_native_probe_graph");
        physical.emplace(b); // UEPtr is a native weak handle, not const-assignable.
        ownerLife=b.owner->ObserveLifecycle();neuronLife=b.neuron->ObserveLifecycle();datasetLife=b.dataset->ObserveLifecycle();
        Require(!b.unsafeIntegration,"native_integration_preflight_failed");
        Require(!b.legacyPhysicsPending,"pending_legacy_physics_edit");
        Require(!b.postTuneActive,"concurrent_native_posttune");
        Require(Detail::Finite(time),"invalid_native_time");
        Require(bool(request.readModelRevision) && !request.baseModelRevision.empty(),"missing_native_revision_provider");
        Require(request.specification.begin==0. && request.specification.baseline.empty() &&
            request.specification.modelRevision.empty() && !request.specification.recordSpikes,
            "unsupported_native_probe_specification");
        Require(b.branch==(request.routing==ProbeRouting::BranchSoloBurst),"native_routing_mismatch");
        Require(b.routingComplete,"missing_native_routing_component");
        Require(!b.branch || (request.activeInput>=0 && request.activeInput<b.inputCount &&
            !b.soloSynapses.empty()),"invalid_native_solo_input");
        CheckPattern(b);
        auto generator=Generator(b);
        capturedGenerator=generator;
        frequency=b.owner->GetTimeStep();
        Require(Detail::Finite(frequency) && frequency>0.,"invalid_native_frequency");
        for(auto ancestor=b.owner->GetOwner();ancestor;ancestor=ancestor->GetOwner())
            Require(double(ancestor->GetTimeStep())==frequency,"unsupported_native_multirate");
        std::vector<UEPtr<UContainer>> nodes;std::set<UContainer*> seen;
        Collect(b.neuron.Get(),nodes,seen);Collect(b.dataset.Get(),nodes,seen);
        for(const auto& node:nodes)
            Require(node->IsReady() && node->IsInit() && double(node->GetTimeStep())==frequency,
                "unready_or_multirate_native_graph");
        Require(!generator->UseTransitSignal.GetData() && !generator->UsePatternOutput.GetData() &&
            generator->FrequencyDeviation.GetData()==0. && Detail::Finite(generator->PulseLength.GetData()) &&
            generator->PulseLength.GetData()>=1./frequency && generator->Delay.GetData()==0. &&
            Detail::Finite(generator->Amplitude.GetData()) && generator->Amplitude.GetData()>0.,
            "unsupported_native_generator_mode");
        generatorLife=generator->ObserveLifecycle();
        generatorParameters=SourceParameters(generator);
        Require(b.somas.size()==(b.branch?1u:std::size_t(b.inputCount)),"missing_native_physical_somas");
        Require(request.specification.maximumSampleGap>0. &&
            !Detail::After(request.specification.maximumSampleGap,1./frequency),"unsupported_native_sample_gap");
        Require(request.readModelRevision()==request.baseModelRevision,"native_base_revision_changed");
        // Snapshot all stimulus inputs before any transient Branch routing edit.
        // The standard typed input API is used; persistent UNet link records
        // remain untouched and are correct again when this overlay is restored.
        if(b.branch)
        {
            std::set<NPulseSynapseCommon*> selected(b.soloSynapses.begin(),b.soloSynapses.end());
            for(const auto& node:nodes)if(auto synapse=dynamic_pointer_cast<NPulseSynapseCommon>(node))
            {
                Require(!synapse->Input.UsesExternalDataSource(),"unsupported_native_external_binding");
                const auto sources=synapse->Input.GetDataSources();
                Require(sources.size()<=1,"unsupported_native_multi_binding");
                Require(sources.empty() || sources[0]==&generator->Output,"foreign_native_stimulus_binding");
                if(sources.empty())
                    Require(synapse->Input.GetData().GetSize()==0 ||
                        (synapse->Input.GetRows()==1 && synapse->Input.GetCols()==1 && synapse->Input(0,0)==0.),
                        "unsupported_native_unconnected_stimulus");
                savedInputs.push_back({synapse,sources.empty()?nullptr:&generator->Output,
                    synapse->GetLongName(b.owner),synapse->ObserveLifecycle()});
                selected.erase(synapse.Get());
            }
            Require(selected.empty(),"native_solo_synapse_outside_graph");
        }
        oldDelay=b.dataset->Delay.GetData();oldAdvance=b.dataset->AdvanceSampleAfterBurst.GetData();sourceSaved=true;
        StopSource();b.neuron->Reset();b.dataset->Reset();
        Require(request.readModelRevision()==request.baseModelRevision,"native_reset_changed_model");
        for(const auto& saved:savedInputs)
            Require(saved.lifetime.IsAlive() && b.owner->GetComponentL(saved.path,true).Get()==saved.synapse.Get(),
                "native_reset_replaced_graph");
        if(b.branch)
        {
            const std::set<NPulseSynapseCommon*> selected(b.soloSynapses.begin(),b.soloSynapses.end());
            for(auto& saved:savedInputs)
                if(selected.count(saved.synapse.Get()))saved.synapse->Input.SetPointer(0,&generator->Output);
                else
                {
                    saved.synapse->Input.DetachFrom();
                    saved.synapse->Input.Assign(1,1,0.);
                }
        }
        ProbeSpec spec=request.specification;
        const double duration=spec.end;
        Require(Detail::Finite(duration) && duration>0. && Detail::Finite(time+duration),"invalid_native_duration");
        spec.begin=time;spec.end=time+duration;end=spec.end;
        for(auto& window:spec.windows){window.begin+=time;window.end+=time;}
        for(const auto& soma:b.somas)
        {
            Require(soma && soma->SumPotential.GetRows()==1 && soma->SumPotential.GetCols()==1,
                "unsupported_native_soma_dimensions");
            spec.baseline.push_back(soma->SumPotential(0,0));
        }
        spec.modelRevision=request.readModelRevision();
        recorder=std::make_unique<ProbeRecorder>(spec);
        b.dataset->Delay=std::max(oldDelay,duration+2./frequency);
        b.dataset->AdvanceSampleAfterBurst=false;b.dataset->StateGeneration=2;
        nodes.clear();seen.clear();Collect(b.neuron.Get(),nodes,seen);
        for(const auto& node:nodes)watched.push_back({node,node->GetLongName(b.owner),node->ObserveLifecycle(),true});
        nodes.clear();seen.clear();Collect(b.dataset.Get(),nodes,seen);
        for(const auto& node:nodes)watched.push_back({node,node->GetLongName(b.owner),node->ObserveLifecycle(),node.Get()==b.dataset.Get()});
        prepared=PreparedVerificationProbe{request.routing,request.inputId,request.protocolRevision,
            request.baseModelRevision,spec.modelRevision,request.modelEpoch};
        if(request.onPrepared)request.onPrepared(*prepared,spec);
        CheckLiveGraph(b);CheckPattern(b);CheckSource(b);
        Require(request.readModelRevision()==spec.modelRevision,"native_prepared_callback_changed_model");
        state=NativeProbeStatus::Running;
    }
    catch(const std::exception& error){Failure(time,error.what());}
    catch(...){Failure(time,"native_preparation_exception");}
}

void NativeProbeExecutor::AfterChildren(const NativeProbeBindings& b,double time)
{
    if(state!=NativeProbeStatus::Running)return;
    lastTime=time;
    try
    {
        CheckLiveGraph(b);CheckPattern(b);CheckSource(b);
        const auto revision=request.readModelRevision();
        auto generator=Generator(b);
        NativeProbeSample sample;sample.time=time;sample.effectiveModelRevision=revision;
        Require(generator->Output.GetRows()==1 && generator->Output.GetCols()==1 &&
            Detail::Finite(generator->Output(0,0)),"invalid_native_stimulus_sample");
        sample.stimulus=generator->Output(0,0);
        for(int i=0;i<generator->OutputPulseTimes.GetCols();++i)
        {
            const double emission=generator->OutputPulseTimes(0,i);
            Require(Detail::Finite(emission) && !Detail::After(emission,time),"invalid_native_emission_time");
            if(emission>=end-request.specification.end && (!lastEmission || emission>*lastEmission))
            {
                const auto& ids=request.specification.expectedStimulusIds;
                const auto id=emitted<ids.size()?ids[emitted]:"unexpected_native_emission";
                recorder->RecordStimulus(id,emission);sample.newStimuli.push_back({id,emission});
                ++emitted;lastEmission=emission;
            }
        }
        std::vector<double> values;
        for(const auto& soma:b.somas)
        {
            Require(soma && soma->SumPotential.GetRows()==1 && soma->SumPotential.GetCols()==1,
                "unsupported_native_soma_dimensions");
            values.push_back(soma->SumPotential(0,0));
        }
        recorder->Observe(time,values,revision);
        if(request.onSample)
        {sample.potentials=std::move(values);request.onSample(sample);}
        if(!Detail::After(end,time))Finish(time,"window_closed",NativeProbeStatus::Completed);
    }
    catch(const std::exception& error){Failure(time,error.what());}
    catch(...){Failure(time,"native_observation_exception");}
}

void NativeProbeExecutor::StopSource()
{
    if(!SameLiveRoots())return;
    physical->dataset->StateGeneration=0;
    auto generator=physical->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    if(generator && generatorLife.IsAlive()){generator->Frequency=0.;generator->Reset();}
}
void NativeProbeExecutor::Restore()
{
    StopSource();
    for(auto& saved:savedInputs)
        if(SameLiveRoots() && generatorLife.IsAlive() && saved.lifetime.IsAlive() &&
            physical->owner->GetComponentL(saved.path,true).Get()==saved.synapse.Get())
        {
            if(saved.source)saved.synapse->Input.SetPointer(0,saved.source);
            else saved.synapse->Input.DetachFrom();
        }
    savedInputs.clear();
    if(SameLiveRoots() && sourceSaved)
    {
        physical->dataset->Delay=oldDelay;
        physical->dataset->AdvanceSampleAfterBurst=oldAdvance;
    }
    sourceSaved=false;
}
void NativeProbeExecutor::Finish(double time,const std::string& reason,NativeProbeStatus status)
{
    NativeProbeResult completed;completed.status=status;completed.prepared=prepared;
    try
    {
        if(recorder)completed.observation=recorder->Close(time,CanReadModel()?request.readModelRevision():"",reason);
    }
    catch(const std::exception& error){completed.reasons.push_back(error.what());completed.status=NativeProbeStatus::Rejected;}
    try
    {
        Restore();
        if(CanReadModel() && request.readModelRevision)
            completed.currentBaseRevision=request.readModelRevision();
        if(completed.currentBaseRevision!=request.baseModelRevision)
        {completed.reasons.push_back("native_base_revision_changed");completed.status=NativeProbeStatus::Rejected;}
    }
    catch(const std::exception& error){completed.reasons.push_back(error.what());completed.status=NativeProbeStatus::Rejected;}
    if(status!=NativeProbeStatus::Completed)completed.reasons.push_back(reason);
    completed.requiresFreshVerification=completed.status!=NativeProbeStatus::Completed ||
        (completed.observation && std::find(completed.observation->reasons.begin(),completed.observation->reasons.end(),
            "model_revision_changed")!=completed.observation->reasons.end());
    if(completed.observation && (completed.status!=NativeProbeStatus::Completed || !completed.reasons.empty()))
    {
        completed.observation->valid=false;
        completed.observation->reasons.insert(completed.observation->reasons.end(),completed.reasons.begin(),completed.reasons.end());
    }
    state=completed.status;result=std::move(completed);recorder.reset();physical.reset();watched.clear();
}
void NativeProbeExecutor::Failure(double time,const std::string& reason)
{ Finish(time,reason,NativeProbeStatus::Rejected); }
void NativeProbeExecutor::Cancel(const std::string& reason)
{
    if(state==NativeProbeStatus::Queued || state==NativeProbeStatus::Running)
        Finish(lastTime,reason,NativeProbeStatus::Cancelled);
}
void NativeProbeExecutor::ResetControl()
{
    Cancel("learner_reset");Restore();physical.reset();recorder.reset();result.reset();prepared.reset();
    request=NativeProbeRequest{};seenTrials.clear();state=NativeProbeStatus::Idle;
}
std::optional<NativeProbeResult> NativeProbeExecutor::TakeResult()
{ auto value=std::move(result);result.reset();return value; }
const char* NativeProbeStatusName(NativeProbeStatus status)
{
    switch(status)
    {
    case NativeProbeStatus::Idle:return "idle";case NativeProbeStatus::Queued:return "queued";
    case NativeProbeStatus::Running:return "running";case NativeProbeStatus::Completed:return "completed";
    case NativeProbeStatus::Rejected:return "rejected";case NativeProbeStatus::Cancelled:return "cancelled";
    }
    return "unknown";
}
}
