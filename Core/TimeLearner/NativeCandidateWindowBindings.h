#pragma once
#include "CandidateWindowTrainingBoundary.h"
#include "NativeProbe.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
struct NativeModelSpec;
struct VerificationProtocol;
// Actual read-only DATA. Attachments name the owner's logical ordinal map;
// every bank/control knob/member/receiver/source is independently read from
// the graph. This function does not create a reference seed or live snapshot.
CandidateWindowPhysicalReadback ReadNativeCandidateWindowPhysical(const NativeProbeBindings&,
    const std::vector<int>& attachments,const VerificationProtocol&,const NativeModelSpec&,
    const std::optional<TrainingCapacitance>&);
QJsonObject EncodeCandidateWindowPhysicalReadback(const CandidateWindowPhysicalReadback&);
// Call with fresh bindings INSIDE actual onPrepared, after routing. The live
// owner/clock/lease guard remains the calling factory's responsibility.
void ValidateNativeCandidateWindowPreparedSelector(const NativeProbeBindings&,
    const CandidateWindowPhysicalReadback&,const VerificationProtocol&,const NativeModelSpec&,
    std::size_t assignedInput,const PreparedVerificationProbe&);
}
