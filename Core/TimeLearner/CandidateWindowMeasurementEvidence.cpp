#include "CandidateWindowMeasurementEvidence.h"
#include "CandidateWindowProposalReplay.h"
#include "NativeCompositeTrainingEvidence.h"
#include "MatchedOmissionValidation.h"
#include "NativeBasinValidation.h"
#include "ModelIdentity.h"
#include "ProbeEvidenceJson.h"
#include "ConfiguredNativeTraining.h"
#include "CandidateWindowLogicalContext.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);}
QString Text(const std::string& s){return QString::fromStdString(s);}
double Count(std::size_t n){Require(n<=CandidateWindowMaximumExactCount,"candidate_window_proof_count");return double(n);}
std::string GroupContentId(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r,bool rejected) {
    ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);
    ValidateCandidateWindowLogicalContext(p,r.logical,r.attemptId,r.groupId,r.purpose);
    Require(r.groupState==NativeMatchedOmissionState::Completed && r.candidateWindowSpecification && r.selection && r.selection->status==NativeDeclaredWindowBasinStatus::Completed &&
        r.selection->geometry &&
        !r.sourceOrigin.empty() && !r.ownerFamilyBinding.empty() && !r.attemptId.empty(),"candidate_window_proof_incomplete");
    if(rejected)Require(r.purpose==CompositeMeasurementPurpose::Search &&
        (r.selection->geometry->validity==DeclaredWindowBasinValidity::Missing ||
         r.selection->geometry->validity==DeclaredWindowBasinValidity::Ambiguous) &&
        !r.selection->geometry->measuredMaximum,"candidate_window_rejection_not_eligible");
    else Require(r.selection->geometry->validity==DeclaredWindowBasinValidity::Valid &&
        r.selection->geometry->measuredMaximum,"candidate_window_proof_incomplete");
    const auto& spec=*r.candidateWindowSpecification;
    Require(r.logical.inputIndex<p.inputs.size() && spec.assignedInput==r.logical.inputIndex && spec.groupId==r.groupId &&
        spec.inputId==p.inputs[r.logical.inputIndex].id && spec.channel==(p.routing==ProbeRouting::ClassicBurst?r.logical.inputIndex:0),
        "candidate_window_proof_specification_binding");
    Require(r.group.specification.runId==r.logical.runId && r.group.specification.groupId==r.groupId &&
        r.group.specification.sampleId==p.sampleId && r.group.specification.protocolRevision==p.revision &&
        r.group.specification.patternHash==p.patternHash && r.group.specification.baseModelRevision==r.logical.baseModelRevision &&
        r.group.specification.modelEpoch==r.logical.epoch,"candidate_window_proof_native_context_binding");
    QJsonValue proposal(QJsonValue::Null);QString purpose;
    if(r.purpose==CompositeMeasurementPurpose::Reference) {
        Require(!r.windowProposal && spec.coordinateMode==CandidateWindowCoordinateMode::ReferenceTime && spec.referenceTime &&
            *spec.referenceTime==p.candidateWindowMeasurement->referenceInputWindows.at(r.logical.inputIndex),"candidate_window_reference_proof_window");purpose="reference";
    } else {
        Require((r.purpose==CompositeMeasurementPurpose::Search || r.purpose==CompositeMeasurementPurpose::Final) && r.windowProposal &&
            spec.coordinateMode==CandidateWindowCoordinateMode::CandidateFrame && spec.candidateFrame &&
            r.windowProposal->computation.window==spec.candidateFrame,"candidate_window_candidate_proof_window");
        proposal=EncodeCandidateWindowProposalData(*r.windowProposal);purpose=r.purpose==CompositeMeasurementPurpose::Search?"search":"final";
    }
    const auto checked=MatchedOmissionDetail::CheckMatchedOmissionContext(r.group.specification,r.group.policy,r.group.witness);
    NativeBasinDetail::CheckNativeEnvelope(r.group,*checked);
    const auto fresh=SelectNativeDeclaredWindowBasinMaximum(spec,p.candidateWindowMeasurement->geometryPolicy,r.group);
    Require(EncodeNativeCompositeSelection(fresh)==EncodeNativeCompositeSelection(*r.selection),"candidate_window_proof_stale_selection");
    std::vector<const MatchedOmissionTrace*> traces{checked->Before().trace};for(const auto& c:checked->Controls())traces.push_back(c.trace);traces.push_back(checked->After().trace);
    Require(traces.size()==r.group.packets.size(),"candidate_window_proof_primary_count");QJsonArray primary;
    for(std::size_t i=0;i<traces.size();++i) {
        const auto bytes=EncodeMatchedOmissionPrimaryTrace(*traces[i],r.group.packets[i]);
        primary.append(QJsonObject{{"native",EncodeNativeProbeEvidence(r.group.packets[i])},
            {"csv_sha256",QString::fromLatin1(QCryptographicHash::hash(bytes,QCryptographicHash::Sha256).toHex())}});
    }
    const auto& l=r.logical;const auto& a=r.attemptLimits;
    const QJsonObject logical{{"run_id",Text(l.runId)},{"trial_id",Text(l.trialId)},{"input_id",Text(l.inputId)},
        {"sample_id",Text(l.sampleId)},{"pattern_hash",Text(l.patternHash)},{"protocol_revision",Text(l.protocolRevision)},
        {"base_model_revision",Text(l.baseModelRevision)},{"routing",p.routing==ProbeRouting::ClassicBurst?"classic_burst":"branch_solo_burst"},
        {"model_epoch",Count(l.epoch)},{"input_index",Count(l.inputIndex)}};
    QJsonObject content{{"kind","live-composite-training-measurement-v2"},{"method_binding",EncodeVerificationProtocol(p)},
        {"source_origin",Text(r.sourceOrigin)},{"owner_family_binding",Text(r.ownerFamilyBinding)},
        {"candidate_witness",NmsdkTools::CanonicalModelDataSha256(ParseStrictNativeJsonObject(r.candidateBindingJson,128u*1024u*1024u))},
        {"attempt_id",Text(r.attemptId)},{"logical_request",logical},{"group_id",Text(r.groupId)},{"purpose",purpose},
        {"method_specification",EncodeCandidateWindowBasinSpecification(spec)},{"window_proposal",proposal},
        {"native_specification",EncodeMatchedOmissionSpecification(r.group.specification)},
        {"attempt_limits",QJsonArray{Count(a.maximumGroups),Count(a.maximumNativeProbes),Count(a.maximumTotalSamples),a.maximumModelSeconds,a.maximumWallSeconds}},
        {"group_limits",QJsonArray{r.maximumGroupModelSeconds,r.maximumGroupWallSeconds}},
        {"maximum_total_primary_bytes",Count(r.policy.maximumTotalPrimaryBytes)},
        {"primary",primary}};
    if(rejected) {
        content["kind"]="live-composite-search-rejection-content-v1";
        content["rejected_selection"]=EncodeNativeCompositeSelection(fresh);
    } else {
        const auto& peak=*fresh.geometry->measuredMaximum;
        content["maximum"]=QJsonObject{{"first_sample",Count(peak.firstSample)},{"last_sample",Count(peak.lastSample)},
            {"amplitude",peak.amplitude},{"raw_potential",peak.rawPotential},{"time",peak.time},{"plateau_end",peak.plateauEnd}};
    }
    return NmsdkTools::CanonicalModelDataSha256(content).toStdString();
}
}
std::string CandidateWindowTrainingSemanticProofId(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r) {
    return GroupContentId(p,r,false);
}
std::string CandidateWindowRejectedGroupContentId(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r) {
    return GroupContentId(p,r,true);
}
}
