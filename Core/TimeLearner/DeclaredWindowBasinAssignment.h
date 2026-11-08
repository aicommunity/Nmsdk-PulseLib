#pragma once
#include "MeasuredResponse.h"
#include <optional>
#include <string>
#include <vector>

namespace NMSDK::TimeLearner {
// Geometry only: the caller declares a single shared grid for all N+2 traces.
// This API does not validate native execution, events, routing, reset, model
// identity or the physical source of a vertex. No training proof is created.
struct BasinGeometryTrace {
    double baseline=0.;
    std::vector<double> rawPotential;
};
struct BasinOmissionGeometry {
    std::size_t omittedInput=0;
    BasinGeometryTrace trace;
};
struct DeclaredWindowBasinWitness {
    std::vector<double> sampleTimes;
    BasinGeometryTrace fullBefore,fullAfter;
    std::vector<BasinOmissionGeometry> omissions;
};
struct DeclaredWindowBasinSpecification {
    unsigned schemaVersion=1;
    std::size_t inputCount=0,assignedInput=0;
    double windowBegin=0.,windowEnd=0.,minimumAmplitude=1e-6;
};
struct DeclaredWindowBasinPolicy {
    unsigned schemaVersion=1;
    std::size_t maximumInputs=0,maximumSamples=0,maximumTotalSamples=0;
    // Separate repeatability policy, never a training amplitude/time tolerance.
    double repeatabilityAbsolute=0.;
};
enum class BasinControlPresence { Preserved,AbsentWithinFixedBasin,Unknown };
enum class BasinCandidateStatus { UniqueNecessary,SharedNecessary,NoNecessaryInput,Unknown,Clipped };
enum class DeclaredWindowBasinValidity {
    Valid,Missing,Ambiguous,Clipped,NonRepeatable,Incomplete,
    InvalidSpecification,BudgetExhausted,Unsupported
};
struct BasinAnchor {
    std::size_t firstSample=0,lastSample=0;
    // True for a record boundary or a one-sided minimum plateau touching it.
    bool recordEdge=true;
    bool operator==(const BasinAnchor&) const=default;
};
struct DeclaredBasinCandidate {
    ResponseMaximum maximum;
    BasinAnchor left,right;
    BasinCandidateStatus status=BasinCandidateStatus::Unknown;
    std::vector<BasinControlPresence> controls; // indexed by original ordinal
    std::vector<std::size_t> necessaryInputs;
    std::vector<std::string> reasons;
};
struct DeclaredWindowBasinResult {
    DeclaredWindowBasinValidity validity=DeclaredWindowBasinValidity::InvalidSpecification;
    std::vector<std::string> reasons;
    // Full-trace extrema whose plateau intersects the declared window only.
    std::vector<DeclaredBasinCandidate> candidates;
    std::optional<std::size_t> selectedCandidate;
    std::optional<ResponseMaximum> measuredMaximum;
    static constexpr const char* MethodId(){return "declared-window-basin-necessity-v1";}
    static constexpr bool TrainingCertificateEligible=false;
};
DeclaredWindowBasinResult SelectDeclaredWindowBasinMaximum(
    const DeclaredWindowBasinSpecification& specification,
    const DeclaredWindowBasinPolicy& policy,const DeclaredWindowBasinWitness& witness);
const char* DeclaredWindowBasinValidityName(DeclaredWindowBasinValidity validity);
const char* BasinControlPresenceName(BasinControlPresence presence);
const char* BasinCandidateStatusName(BasinCandidateStatus status);
}
