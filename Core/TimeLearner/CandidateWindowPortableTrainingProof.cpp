#include "CandidateWindowPortableTrainingProof.h"
#include "CandidateWindowPortableReferences.h"
#include "CandidateWindowCoreValidation.h"
#include "CandidateWindowMeasurementEvidence.h"
#include "CandidateWindowContractValidation.h"
#include "ConfiguredNativeTraining.h"
#include "ProbeEvidenceJson.h"
#include "ModelIdentity.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
const NativeCompositeMeasurementRecord& Record(const DecodedNativeCompositeArchive& archive,const std::string& id) {
    const auto& records=archive.Records();const auto found=std::find_if(records.begin(),records.end(),[&](const auto& r){return r.groupId==id;});
    Require(found!=records.end() && found->consumed && found->trainingRequestedAtConsume==true && !found->semanticProofId.empty() &&
        found->groupState==NativeMatchedOmissionState::Completed && found->selection &&
        found->selection->status==NativeDeclaredWindowBasinStatus::Completed && found->selection->geometry &&
        found->selection->geometry->validity==DeclaredWindowBasinValidity::Valid && found->selection->geometry->measuredMaximum,
        "portable_candidate_window_accepted_measurement_missing");
    Require(found->semanticProofId==CandidateWindowTrainingSemanticProofId(archive.Protocol(),*found),
        "portable_candidate_window_measurement_proof_mismatch");return *found;
}
std::size_t Decimal(const std::string& text) {
    Require(!text.empty() && (text=="0" || text.front()!='0') && std::all_of(text.begin(),text.end(),[](char c){return c>='0' && c<='9';}),
        "portable_candidate_window_serial");
    bool ok=false;const auto value=QString::fromStdString(text).toULongLong(&ok);
    Require(ok && value<=9007199254740991ULL && value<=std::numeric_limits<std::size_t>::max(),"portable_candidate_window_serial");
    return std::size_t(value);
}
std::size_t TrialSerial(const std::string& trial) {
    const auto slash=trial.rfind('/');Require(slash!=std::string::npos,"portable_candidate_window_final_trial");
    const auto serial=Decimal(trial.substr(slash+1));Require(serial>0,"portable_candidate_window_final_trial");return serial;
}
std::string LatestObservedFinalRun(const DecodedNativeCompositeArchive& archive) {
    const auto prefix=archive.CurrentAttemptId()+"/final/";std::optional<std::size_t> latest;std::string run;
    // Include rejected/partial groups: a later observed sweep invalidates an
    // older whole selection, even when it has not accepted its first input.
    for(const auto& r:archive.Records())if(r.purpose==CompositeMeasurementPurpose::Final && r.attemptId==archive.CurrentAttemptId()) {
        Require(r.logical.runId.starts_with(prefix),"portable_candidate_window_final_namespace");
        const auto ordinal=Decimal(r.logical.runId.substr(prefix.size()));if(!latest || ordinal>*latest){latest=ordinal;run=r.logical.runId;}
    }
    return run;
}
}
CandidateWindowPortableTrainingProof CandidateWindowPortableTrainingProof::Replay(DecodedNativeCompositeArchive value,
    const VerificationTolerances& tolerances,const std::string& revision,std::size_t epoch) {
    CandidateWindowPortableTrainingProof result(std::move(value));const auto& a=result.archive;const auto& p=a.Protocol();
    Require(a.Document()["schema_version"]==QJsonValue(2),"portable_candidate_window_archive2_required");
    ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);
    Require(!revision.empty() && epoch<=9007199254740991ULL,"portable_candidate_window_current_boundary");
    for(double tolerance:{tolerances.amplitudeAbsolute,tolerances.amplitudeRelative,tolerances.sync})
        Require(std::isfinite(tolerance) && tolerance>=0.,"portable_candidate_window_tolerance");
    result.modelRevision=revision;result.modelEpoch=epoch;
    const auto n=p.inputs.size();Require(a.FinalGroups().empty() || a.ReferenceGroups().size()==n,"portable_candidate_window_final_without_targets");
    if(a.ReferenceGroups().size()!=n)return result;

    // Only the existing strict factory closes original P0/C0/source/grid and
    // actual reference plateaux. Current trained physics is never recaptured.
    auto closed=CandidateWindowPortableReferenceFactory::Replay(a);
    CapturedReferences references;references.protocol=p;references.compositeOwnerScope=a.OwnerFamilyBinding();
    references.compositeSourceOrigin=a.SourceOrigin();references.initialModelRevision=closed.InitialModelRevision();
    references.initialModelEpoch=closed.InitialModelEpoch();
    for(std::size_t i=0;i<n;++i) {
        const auto& r=Record(a,a.ReferenceGroups()[i]);
        Require(r.purpose==CompositeMeasurementPurpose::Reference && r.logical.inputIndex==i,
            "portable_candidate_window_reference_order");
        references.inputs.push_back({p.inputs[i].id,r.logical.runId,r.logical.trialId,r.group.specification.effectiveModelRevision,
            r.selection->geometry->measuredMaximum->rawPotential,r.groupId,r.semanticProofId});
    }
    CandidateWindowCoreDetail::RequireCapturedReferences(references,closed);
    result.references=references;result.windowReferences=closed;
    if(a.FinalGroups().size()!=n)return result;

    const auto finalRun=LatestObservedFinalRun(a);Require(!finalRun.empty(),"portable_candidate_window_final_run_missing");
    FinalVerificationSession replay(references,closed,{finalRun,revision,n,0,epoch,tolerances});
    const auto origin=std::find(p.stimulusIds.begin(),p.stimulusIds.end(),p.originStimulusId)-p.stimulusIds.begin();
    std::size_t previousSerial=0;std::string candidateWitness;
    for(std::size_t position=0;position<n;++position) {
        const auto input=replay.order.at(position);const auto& r=Record(a,a.FinalGroups().at(input));const auto serial=TrialSerial(r.logical.trialId);
        Require(r.purpose==CompositeMeasurementPurpose::Final && r.logical.inputIndex==input && r.logical.runId==finalRun &&
            r.attemptId==a.CurrentAttemptId() && r.logical.baseModelRevision==revision && r.logical.epoch==epoch &&
            r.windowProposal && r.candidateWindowSpecification &&
            r.candidateWindowSpecification->coordinateMode==CandidateWindowCoordinateMode::CandidateFrame &&
            serial>previousSerial && replay.seenTrials.insert(r.logical.trialId).second &&
            replay.seenCompositeGroups.insert(r.groupId).second && replay.seenCompositeProofs.insert(r.semanticProofId).second,
            "portable_candidate_window_fresh_final_context");previousSerial=serial;
        const auto& proposal=*r.windowProposal;
        Require(proposal.attemptId==a.CurrentAttemptId() && proposal.runId==finalRun && !proposal.windowRecipeId.empty() &&
            !proposal.measurementInstanceId.empty() && proposal.protocolBinding==closed.ProtocolBinding(),
            "portable_candidate_window_final_proposal_context");
        const auto binding=ParseStrictNativeJsonObject(r.candidateBindingJson,128u*1024u*1024u);
        const auto witness=NmsdkTools::CanonicalModelDataSha256(binding).toStdString();
        if(!position)candidateWitness=witness;
        Require(witness==candidateWitness && binding["model_identity"].toObject().value("sha256")==QJsonValue(QString::fromStdString(revision)),
            "portable_candidate_window_mixed_final_candidate");
        const auto& events=r.group.witness.fullBefore.observation.events;Require(std::size_t(origin)<events.size(),"portable_candidate_window_actual_origin_missing");
        const auto& peak=*r.selection->geometry->measuredMaximum;const double originTime=events.at(std::size_t(origin)).time;
        // Scalar arrival is relative to THIS full-before's actual protocol
        // origin, never absolute model time, assigned-delay or a predictor.
        replay.pending=r.logical;replay.AcceptValidatedScalar(input,peak.rawPotential,peak.time-originTime,peak.plateauEnd-originTime,
            r.group.specification.effectiveModelRevision);
        auto& accepted=*replay.accepted[input];accepted.compositeProofId=r.semanticProofId;accepted.candidateWitnessId=witness;
        accepted.candidateWindowRecipeId=proposal.windowRecipeId;accepted.candidateWindowInstanceId=proposal.measurementInstanceId;
        accepted.candidateWindowAttemptId=proposal.attemptId;accepted.candidateWindowFinalRunId=proposal.runId;
        ++replay.issued;replay.CompleteValidatedInput();
    }
    Require(replay.Certificate() && replay.Certificate()->SchemaVersion()==3 &&
        (replay.Status()==VerificationStatus::Exact || replay.Status()==VerificationStatus::OutsideTolerance) &&
        replay.Certificate()->Matches(revision,references,tolerances,ModelStateKind::LearnedSolo,epoch),
        "portable_candidate_window_final_math_incomplete");
    result.certificate=*replay.Certificate();return result;
}
void CandidateWindowPortableTrainingProof::RequireCertificate(const QJsonObject& claimed) const {
    Require(certificate && claimed==EncodeTrainingCertificate(*certificate),"portable_candidate_window_certificate_claim_mismatch");
}
}
