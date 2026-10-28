#pragma once
#include "NativeTrainingService.h"
namespace NMSDK::TimeLearner {
struct NativeTrainingProof { CapturedReferences references;TrainingCertificate certificate; };
// Accepted reference prefix at a stopped boundary. Incomplete capture never
// manufactures missing goals. Complete capture uses the existing production gate.
std::optional<CapturedReferences> ReplayNativeReferencePrefix(const VerificationProtocol& protocol,
    const std::vector<NativeProbeResult>& references,const std::string& captureRevision,
    std::size_t captureEpoch,bool complete);
NativeTrainingProof ReplayNativeTrainingProof(const VerificationProtocol& protocol,const VerificationTolerances& tolerances,
    const std::vector<NativeProbeResult>& references,const std::vector<NativeProbeResult>& finals,
    const std::string& modelRevision,std::size_t epoch,std::size_t maximumInvalidObservations);
}
