#include "NativeTrainingProof.h"
namespace NMSDK::TimeLearner {
NativeTrainingProof ReplayNativeTrainingProof(const VerificationProtocol& protocol,const VerificationTolerances& tolerances,
    const std::vector<NativeProbeResult>& references,const std::vector<NativeProbeResult>& finals,
    const std::string& revision,std::size_t epoch,std::size_t maximumInvalidObservations)
{
    const auto check=[](const NativeProbeResult& p) {
        if(p.status!=NativeProbeStatus::Completed || !p.prepared || !p.observation ||
            !p.reasons.empty() || p.requiresFreshVerification || p.currentBaseRevision!=p.prepared->baseModelRevision)
            throw std::invalid_argument("unusable_native_boundary_packet");
    };
    std::vector<ReferenceEvidence> evidence;
    for(const auto& packet:references){check(packet);evidence.push_back({*packet.prepared,*packet.observation});}
    auto captured=CapturedReferences::Capture(protocol,evidence);
    if(finals.empty() || revision.empty())throw std::invalid_argument("missing_native_boundary_final_evidence");
    check(finals.front());
    FinalVerificationSession replay(captured,{finals.front().observation->specification.runId,revision,finals.size(),
        maximumInvalidObservations,epoch,tolerances});
    for(const auto& packet:finals)
    {
        check(packet);const auto request=replay.NextRequest();
        if(!request || packet.currentBaseRevision!=revision || packet.prepared->modelEpoch!=epoch ||
            request->runId!=packet.observation->specification.runId || request->trialId!=packet.observation->specification.trialId)
            throw std::invalid_argument("native_boundary_final_sequence_mismatch");
        replay.Arm(*packet.prepared);replay.Consume(*packet.observation,revision);
    }
    if(!replay.Certificate() || (replay.Status()!=VerificationStatus::Exact && replay.Status()!=VerificationStatus::OutsideTolerance) ||
        !replay.Certificate()->Matches(revision,captured,tolerances,ModelStateKind::LearnedSolo,epoch))
        throw std::invalid_argument("native_boundary_verification_incomplete");
    return {std::move(captured),*replay.Certificate()};
}
}
