#pragma once
#include "NativeTrainingService.h"
namespace NMSDK::TimeLearner {
struct NativeTrainingProof { CapturedReferences references;TrainingCertificate certificate; };
NativeTrainingProof ReplayNativeTrainingProof(const VerificationProtocol& protocol,const VerificationTolerances& tolerances,
    const std::vector<NativeProbeResult>& references,const std::vector<NativeProbeResult>& finals,
    const std::string& modelRevision,std::size_t epoch,std::size_t maximumInvalidObservations);
}
