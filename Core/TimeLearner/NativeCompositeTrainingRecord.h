#pragma once
#include "NativeDeclaredWindowBasinAssignment.h"
#include <QByteArray>

namespace NMSDK::TimeLearner {
struct CompositeAttemptLimits {
    std::size_t maximumGroups=0,maximumNativeProbes=0,maximumTotalSamples=0;
    double maximumModelSeconds=0.,maximumWallSeconds=0.;
};
// Actual callback records, including an interrupted last native trial. These
// are evidence data, never a live admission token or a completed-group flag.
struct NativeCompositeRawTrial {
    PreparedVerificationProbe prepared;
    ProbeSpec actual;
    std::vector<NativeProbeSample> samples;
    std::optional<NativeProbeResult> packet;
};
struct NativeCompositeTrainingPolicy {
    unsigned schemaVersion=1;
    std::string methodId=DeclaredWindowBasinResult::MethodId();
    MatchedOmissionPolicy contextPolicy;
    DeclaredWindowBasinPolicy geometryPolicy;
    std::size_t maximumGroups=0,maximumNativeProbes=0,maximumTotalSamples=0,maximumTotalPrimaryBytes=0;
    double maximumGroupModelSeconds=0.,maximumGroupWallSeconds=0.;
    bool operator==(const NativeCompositeTrainingPolicy&) const=default;
};
struct NativeCompositeMeasurementRecord {
    VerificationRequest logical;
    CompositeMeasurementPurpose purpose=CompositeMeasurementPurpose::Reference;
    std::string groupId,sourceOrigin,ownerFamilyBinding,liveAttemptBinding,attemptId;
    std::string candidateBindingJson,semanticProofId,reason;
    NativeMatchedOmissionState groupState=NativeMatchedOmissionState::Rejected;
    NativeMatchedOmissionResult group;
    std::vector<NativeCompositeRawTrial> rawTrials;
    std::map<std::string,TrainingCapacitanceWitness> capacitanceWitnesses;
    std::map<std::string,std::string> nativeTrialWitnessIds;
    std::optional<NativeDeclaredWindowBasinResult> selection;
    CompositeAttemptLimits attemptLimits;
    NativeCompositeTrainingPolicy policy;
    std::size_t issuedNativeProbes=0,sampledFrames=0;
    double startModel=0.,startWall=0.,endModel=0.,endWall=0.;
    bool consumed=false;
    std::optional<bool> trainingRequestedAtConsume;
};
QByteArray EncodeCompositeRawPrimary(const NativeCompositeRawTrial& trial);
std::size_t CompositeRecordPrimaryBytes(const NativeCompositeMeasurementRecord& record);
}
