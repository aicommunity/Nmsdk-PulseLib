#include "NativeMatchedOmissionGroup.h"
#include "NativeCalculationPolicy.h"
#include "NumericChecks.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason) {if(!value)throw std::invalid_argument(reason);}
constexpr const char* ResetBinding="native-neuron-dataset-reset-before-children-v1";
}
void NativeMatchedOmissionGroup::Start(NativeMatchedOmissionPlan value,const NativeProbeBindings& b,
                                      double modelTime,double wallTime)
{
    Require(state==NativeMatchedOmissionState::Idle,"omission_group_already_started");
    // A failed preparation cannot be reused as an apparently untouched group.
    state=NativeMatchedOmissionState::Rejected;
    result.reason="omission_group_preflight_rejected";
    const auto& request=value.fullBurst;const auto& spec=request.specification;
    const auto n=request.expectedInputIsi.size();
    Require(b.owner && b.neuron && b.dataset && b.inputCount>1 && n==std::size_t(b.inputCount),
            "omission_group_requires_multi_input_native_model");
    Require(!b.legacyPhysicsPending && !b.unsafeIntegration && !b.postTuneActive && b.routingComplete,
            "omission_group_unsupported_native_state");
    Require(request.activeInput>=0 && std::size_t(request.activeInput)<n &&
            request.routing==(b.branch?ProbeRouting::BranchSoloBurst:ProbeRouting::ClassicBurst) &&
            value.channel==(b.branch?0u:std::size_t(request.activeInput)),"omission_group_input_mapping");
    Require(!value.groupId.empty() && !request.baseModelRevision.empty() && !request.protocolRevision.empty() &&
            !spec.runId.empty() && !spec.sampleId.empty() && spec.schemaVersion==3 &&
            spec.begin==0. && spec.baseline.empty() && spec.modelRevision.empty() &&
            !spec.characterization && !spec.stimulusSubset && spec.stimulusProfile &&
            !spec.recordSpikes && bool(request.readModelRevision),"omission_group_invalid_prototype");
    Require(value.physicalChannelPaths.size()==b.somas.size() && value.channel<b.somas.size() &&
            std::set<std::string>(value.physicalChannelPaths.begin(),value.physicalChannelPaths.end()).size()==b.somas.size(),
            "omission_group_channel_paths");
    for(std::size_t i=0;i<b.somas.size();++i)
        Require(b.somas[i] && value.physicalChannelPaths[i]==b.somas[i]->GetLongName(b.owner),
                "omission_group_channel_path_mismatch");
    Require(spec.expectedStimulusIds.size()==n && n<=std::numeric_limits<std::size_t>::max()-2 &&
            value.policy.maximumNativeProbes>=n+2 && value.policy.maximumSamples>=spec.maximumSamples &&
            spec.maximumSamples>0 && value.policy.maximumTotalSamples>0,"omission_group_probe_budget");
    const auto policy=ValidateMatchedOmissionPolicy(value.policy,*spec.stimulusProfile,n);
    if(policy.validity!=MatchedOmissionValidity::Valid)
        throw std::invalid_argument(policy.reasons.empty()?"omission_group_invalid_policy":policy.reasons.front());
    QJsonArray pattern;
    for(std::size_t i=0;i<n;++i) {
        Require(spec.expectedStimulusIds[i]=="pulse"+std::to_string(i),"omission_group_stimulus_identity");
        pattern.append(request.expectedInputIsi[i]);
    }
    Require(NmsdkTools::CanonicalModelDataSha256(pattern).toStdString()==spec.patternHash,
            "omission_group_source_pattern_hash");
    for(double t:{modelTime,wallTime,value.maximumModelSeconds,value.maximumWallSeconds})
        Require(std::isfinite(t),"omission_group_invalid_clock");
    Require(value.maximumModelSeconds>0. && value.maximumWallSeconds>0.,"omission_group_invalid_time_budget");
    RequireNativeCalculationSchedule(*b.owner);
    plan=value;bindings.emplace(b);ownerLifetime=b.owner->ObserveLifecycle();
    std::set<RDK::UContainer*> seen;
    const auto visit=[&](auto&& self,RDK::UEPtr<RDK::UContainer> node)->void {
        Require(node && seen.insert(node.Get()).second,"omission_group_invalid_graph");
        watched.push_back({node,node->GetLongName(b.owner),node->ObserveLifecycle()});
        std::vector<RDK::NameT> names;node->GetComponentsList(names);
        for(const auto& name:names)self(self,node->GetComponent(name,true));
    };
    visit(visit,b.neuron);visit(visit,b.dataset);
    std::ostringstream identity;identity<<plan.groupId;
    for(const auto& node:watched)identity<<'|'<<node.path<<':'<<node.node.Get();
    QJsonArray instance{QString::fromStdString(identity.str())};
    auto& s=result.specification;
    s.runId=spec.runId;s.groupId=plan.groupId;s.sampleId=spec.sampleId;s.protocolRevision=request.protocolRevision;
    s.patternHash=spec.patternHash;s.inputId=request.inputId;s.baseModelRevision=request.baseModelRevision;
    s.resetBinding=ResetBinding;s.modelInstanceBinding=NmsdkTools::CanonicalModelDataSha256(instance).toStdString();
    s.modelEpoch=request.modelEpoch;s.assignedInput=std::size_t(request.activeInput);s.channel=plan.channel;
    s.routing=request.routing;s.stimulusIds=spec.expectedStimulusIds;s.physicalChannelPaths=plan.physicalChannelPaths;
    s.stimulusProfile=*spec.stimulusProfile;s.duration=spec.end;s.minimumAmplitude=spec.minimumAmplitude;
    Require(b.dataset->MatrixClasses.GetRows()==1 && b.dataset->MatrixClasses.GetCols()==1,
            "omission_group_class_shape");
    initialClass=b.dataset->MatrixClasses(0,0);CheckRestoredPresentation();
    Require(request.readModelRevision()==request.baseModelRevision,"omission_group_initial_model_changed");
    startModel=lastModel=modelTime;startWall=lastWall=wallTime;result.reason.clear();state=NativeMatchedOmissionState::Running;
}
void NativeMatchedOmissionGroup::CheckInstances() const
{
    Require(bindings && ownerLifetime.IsCurrent(),"omission_group_owner_lifecycle_changed");
    const auto& b=*bindings;
    for(const auto& node:watched)
        Require(node.lifetime.IsAlive() && node.node &&
                b.owner->GetComponentL(node.path,true).Get()==node.node.Get(),"omission_group_graph_replaced");
    std::set<RDK::UContainer*> current;
    const auto visit=[&](auto&& self,RDK::UEPtr<RDK::UContainer> node)->void {
        Require(node && current.insert(node.Get()).second,"omission_group_invalid_graph");
        std::vector<RDK::NameT> names;node->GetComponentsList(names);
        for(const auto& name:names)self(self,node->GetComponent(name,true));
    };
    visit(visit,b.neuron);visit(visit,b.dataset);
    Require(current.size()==watched.size(),"omission_group_graph_changed");
    RequireNativeCalculationSchedule(*b.owner);
}
void NativeMatchedOmissionGroup::CheckRestoredPresentation() const
{
    CheckInstances();const auto& d=*bindings->dataset;const auto& isi=plan.fullBurst.expectedInputIsi;
    Require(d.NumSamples.GetData()==1 && d.NumFeatures.GetData()==1 &&
            d.MaxSpikesPerFeature.GetData()==int(isi.size()) && d.MatrixData.GetRows()==int(isi.size()) &&
            d.MatrixData.GetCols()==1 && d.MatrixClasses.GetRows()==1 && d.MatrixClasses.GetCols()==1 &&
            d.MatrixClasses(0,0)==initialClass,"omission_group_full_mask_not_restored");
    for(std::size_t i=0;i<isi.size();++i)
        Require(d.MatrixData(int(i),0)==isi[i],"omission_group_full_pattern_not_restored");
}
void NativeMatchedOmissionGroup::Fail(const NativeMatchedOmissionAdapter& a,NativeMatchedOmissionState status,
                                     const std::string& reason)
{
    state=status;result.reason=reason;waiting=false;result.assignment.reset();
    try {if(a.cancel)a.cancel(reason);if(a.takeResult)if(auto packet=a.takeResult())result.packets.push_back(std::move(*packet));}
    catch(...) {result.reason+=";omission_group_cancellation_or_restore_failed";}
}
bool NativeMatchedOmissionGroup::CheckClock(const NativeMatchedOmissionAdapter& a,double modelTime,double wallTime)
{
    Require(std::isfinite(modelTime) && std::isfinite(wallTime) && modelTime>=lastModel && wallTime>=lastWall,
            "omission_group_nonmonotonic_clock");
    lastModel=modelTime;lastWall=wallTime;
    if(modelTime-startModel>=plan.maximumModelSeconds || wallTime-startWall>=plan.maximumWallSeconds) {
        Fail(a,NativeMatchedOmissionState::BudgetExhausted,"omission_group_time_budget_exhausted");return false;
    }
    return true;
}
void NativeMatchedOmissionGroup::BeforeTick(const NativeMatchedOmissionAdapter& a,double modelTime,double wallTime)
{
    if(state!=NativeMatchedOmissionState::Running)return;
    try {
        Require(a.queue && a.probeState && a.takeResult && a.cancel && a.readModelRevision,"omission_group_adapter_incomplete");
        if(a.requested && !a.requested()){Cancel(a,"omission_group_external_stop");return;}
        if(!CheckClock(a,modelTime,wallTime) || waiting)return;
        CheckRestoredPresentation();Require(a.readModelRevision()==plan.fullBurst.baseModelRevision,"omission_group_model_changed");
        const auto n=plan.fullBurst.expectedInputIsi.size();
        Require(issued<n+2 && issued<plan.policy.maximumNativeProbes,"omission_group_native_probe_budget");
        Require(plan.fullBurst.specification.maximumSamples<=plan.policy.maximumTotalSamples-totalSamples,
                "omission_group_remaining_sample_budget");
        auto request=plan.fullBurst;auto& s=request.specification;
        activeMask.resize(n);std::iota(activeMask.begin(),activeMask.end(),0);
        if(issued>0 && issued<=n)activeMask.erase(activeMask.begin()+std::ptrdiff_t(issued-1));
        const auto presented=PlanStimulusSubset(request.expectedInputIsi,activeMask);QJsonArray values;
        for(double value:presented.presentedIsi)values.append(value);
        s.schemaVersion=6;s.trialId=plan.groupId+"/native/"+std::to_string(issued);
        s.windows.clear();s.characterization.reset();s.waveAssignment=WaveAssignmentPolicy::WindowOnly;
        s.peakSelection=PeakSelectionPolicy::IsolatedWindow;s.baseline.clear();s.modelRevision.clear();
        s.stimulusSubset=StimulusSubsetCharacterization{result.specification.patternHash,n,activeMask};
        s.patternHash=NmsdkTools::CanonicalModelDataSha256(values).toStdString();s.expectedStimulusIds.clear();
        for(auto ordinal:activeMask)s.expectedStimulusIds.push_back(result.specification.stimulusIds[ordinal]);
        capture=std::make_unique<NativeResponseDomainCapture>();
        const auto prepared=request.onPrepared;const auto sample=request.onSample;
        request.onPrepared=[this,prepared](const auto& p,const auto& actual) {
            CheckInstances();capture->Arm(p,actual,plan.physicalChannelPaths);if(prepared)prepared(p,actual);
        };
        request.onSample=[this,sample](const auto& frame) {capture->Append(frame);if(sample)sample(frame);};
        a.queue(std::move(request));++issued;waiting=true;
    }
    catch(const std::exception& error){Fail(a,NativeMatchedOmissionState::Rejected,error.what());}
    catch(...){Fail(a,NativeMatchedOmissionState::Rejected,"omission_group_boundary_exception");}
}
void NativeMatchedOmissionGroup::AfterTick(const NativeMatchedOmissionAdapter& a,double modelTime,double wallTime)
{
    if(state!=NativeMatchedOmissionState::Running)return;
    try {
        if(a.requested && !a.requested()){Cancel(a,"omission_group_external_stop");return;}
        if(!CheckClock(a,modelTime,wallTime) || !waiting)return;
        const auto status=a.probeState();if(status==NativeProbeStatus::Queued || status==NativeProbeStatus::Running)return;
        auto packet=a.takeResult();Require(bool(packet),"omission_group_missing_native_result");waiting=false;
        result.packets.push_back(*packet);capture->CloseNative(*packet);CheckRestoredPresentation();
        Require(a.readModelRevision()==plan.fullBurst.baseModelRevision,"omission_group_model_changed");
        MatchedOmissionTrace trace;trace.groupId=plan.groupId;trace.sourcePatternHash=result.specification.patternHash;
        trace.resetBinding=ResetBinding;trace.modelInstanceBinding=result.specification.modelInstanceBinding;
        trace.currentBaseRevision=packet->currentBaseRevision;trace.prepared=capture->Context();
        trace.observation=capture->Observation();trace.physicalChannelPaths=capture->PhysicalChannelPaths();
        trace.presentedOrdinals=activeMask;trace.stimulusProfile=*packet->observation->specification.stimulusProfile;
        trace.stimulusSamples=capture->StimulusSamples();trace.nativeCompleted=true;trace.fullMaskRestored=true;
        const auto count=trace.stimulusSamples.size();
        Require(count<=plan.policy.maximumTotalSamples-totalSamples,"omission_group_total_sample_budget");totalSamples+=count;
        if(issued==1) {
            result.specification.effectiveModelRevision=trace.prepared.effectiveModelRevision;
            result.witness.fullBefore=std::move(trace);
        }
        else if(issued<=plan.fullBurst.expectedInputIsi.size()+1)result.witness.omitEachInput.push_back(std::move(trace));
        else {
            result.witness.fullAfter=std::move(trace);
            result.assignment=SelectMatchedOmissionMaximum(result.specification,plan.policy,result.witness);
            state=NativeMatchedOmissionState::Completed;result.reason="omission_group_closed";
        }
        capture.reset();
    }
    catch(const std::exception& error){Fail(a,NativeMatchedOmissionState::Rejected,error.what());}
    catch(...){Fail(a,NativeMatchedOmissionState::Rejected,"omission_group_observation_exception");}
}
void NativeMatchedOmissionGroup::Cancel(const NativeMatchedOmissionAdapter& a,const std::string& reason)
{if(state==NativeMatchedOmissionState::Running)Fail(a,NativeMatchedOmissionState::Cancelled,reason);}
}
