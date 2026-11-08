#include "CompositePortableTrainingProof.h"
#include "ConfiguredNativeTraining.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <limits>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);}
const NativeCompositeMeasurementRecord& Record(const DecodedNativeCompositeArchive& a,const std::string& id) {
    const auto found=std::find_if(a.Records().begin(),a.Records().end(),[&](const auto& r){return r.groupId==id;});
    Require(found!=a.Records().end() && found->consumed && found->trainingRequestedAtConsume==true && !found->semanticProofId.empty() &&
        found->selection && found->selection->status==NativeDeclaredWindowBasinStatus::Completed && found->selection->geometry &&
        found->selection->geometry->validity==DeclaredWindowBasinValidity::Valid && found->selection->geometry->measuredMaximum,
        "portable_composite_accepted_measurement_missing");return *found;
}
std::size_t Serial(const std::string& trial){const auto slash=trial.rfind('/');Require(slash!=std::string::npos,"portable_composite_final_trial");
    bool ok=false;const auto value=QString::fromStdString(trial.substr(slash+1)).toULongLong(&ok);
    Require(ok && value>0 && value<=9007199254740991ULL && value<=std::numeric_limits<std::size_t>::max(),"portable_composite_final_serial");return std::size_t(value);}
}
CompositePortableTrainingProof CompositePortableTrainingProof::Replay(DecodedNativeCompositeArchive value,
    const VerificationTolerances& tolerances,const std::string& revision,std::size_t epoch) {
    CompositePortableTrainingProof result(std::move(value));const auto& a=result.archive;const auto& p=a.Protocol();
    Require(!revision.empty() && epoch<=9007199254740991ULL,"portable_composite_current_boundary");result.modelRevision=revision;result.modelEpoch=epoch;
    for(double tolerance:{tolerances.amplitudeAbsolute,tolerances.amplitudeRelative,tolerances.sync})
        Require(std::isfinite(tolerance) && tolerance>=0.,"portable_composite_tolerance");
    const auto n=p.inputs.size();Require(a.FinalGroups().empty() || a.ReferenceGroups().size()==n,"portable_composite_final_without_captured_target");
    if(a.ReferenceGroups().size()!=n)return result;
    CapturedReferences references;references.protocol=p;references.compositeOwnerScope=a.OwnerFamilyBinding();references.compositeSourceOrigin=a.SourceOrigin();
    for(std::size_t i=0;i<n;++i) {
        const auto& r=Record(a,a.ReferenceGroups()[i]);Require(r.purpose==CompositeMeasurementPurpose::Reference && r.logical.inputIndex==i,
            "portable_composite_reference_order");if(!i){references.initialModelRevision=r.logical.baseModelRevision;references.initialModelEpoch=r.logical.epoch;}
        Require(r.logical.baseModelRevision==references.initialModelRevision && r.logical.epoch==references.initialModelEpoch,
            "portable_composite_mixed_reference_model");
        references.inputs.push_back({p.inputs[i].id,r.logical.runId,r.logical.trialId,r.group.specification.effectiveModelRevision,
            r.selection->geometry->measuredMaximum->rawPotential,r.groupId,r.semanticProofId});
    }
    result.references=references;if(a.FinalGroups().size()!=n)return result;
    const auto& first=Record(a,a.FinalGroups().front());
    FinalVerificationSession replay(references,{first.logical.runId,revision,n,0,epoch,tolerances});std::size_t previousSerial=0;
    // The archive already checked native/context/geometry/anti-replay and each
    // consumed event. Replay accepted values in the SAME protocol order. Gaps
    // between accepted serials may describe failed trials; this boundary proof
    // does not recreate or claim their controller/invalid-budget history.
    for(std::size_t position=0;position<n;++position) {
        const auto input=replay.order.at(position);const auto& r=Record(a,a.FinalGroups().at(input));const auto serial=Serial(r.logical.trialId);
        Require(r.purpose==CompositeMeasurementPurpose::Final && r.logical.inputIndex==input && r.logical.runId==first.logical.runId &&
            r.attemptId==a.CurrentAttemptId() && r.logical.baseModelRevision==revision && r.logical.epoch==epoch && serial>previousSerial &&
            replay.seenTrials.insert(r.logical.trialId).second && replay.seenCompositeGroups.insert(r.groupId).second &&
            replay.seenCompositeProofs.insert(r.semanticProofId).second,"portable_composite_fresh_final_context");previousSerial=serial;
        replay.pending=r.logical;const auto& peak=*r.selection->geometry->measuredMaximum;
        const auto origin=std::find(p.stimulusIds.begin(),p.stimulusIds.end(),p.originStimulusId)-p.stimulusIds.begin();
        const double originTime=r.group.witness.fullBefore.observation.events.at(origin).time;
        replay.AcceptValidatedScalar(input,peak.rawPotential,peak.time-originTime,peak.plateauEnd-originTime,r.group.specification.effectiveModelRevision);
        replay.accepted[input]->compositeProofId=r.semanticProofId;
        replay.accepted[input]->candidateWitnessId=NmsdkTools::CanonicalModelDataSha256(ParseStrictNativeJsonObject(r.candidateBindingJson)).toStdString();
        ++replay.issued;replay.CompleteValidatedInput();
    }
    Require(replay.Certificate() && (replay.Status()==VerificationStatus::Exact || replay.Status()==VerificationStatus::OutsideTolerance) &&
        replay.Certificate()->Matches(revision,references,tolerances,ModelStateKind::LearnedSolo,epoch),"portable_composite_final_math_incomplete");
    result.certificate=*replay.Certificate();return result;
}
}
