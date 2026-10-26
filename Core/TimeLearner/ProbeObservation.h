#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace NMSDK::TimeLearner {
struct StimulusProfile {
    std::string generatorClass;
    double amplitude=0.,pulseLength=0.,frequencyHz=0.,frequencyDeviation=0.;
    bool useTransitSignal=false,usePatternOutput=false;
    bool operator==(const StimulusProfile&) const=default;
};
bool ValidStimulusProfile(const StimulusProfile& profile);
// Internal training/characterization schema; distinct from the recognition CSV.
struct PeakWindow {
    std::string inputId;
    std::size_t channel;
    double begin, end;
    std::string stimulusId;
};
enum class WaveAssignmentPolicy { WindowOnly, OrderedWholeBurst };
struct ProbeSpec {
    unsigned schemaVersion=1;
    std::string runId, trialId, sampleId, patternHash, modelRevision;
    double begin=0., end=0., maximumSampleGap=0.;
    double minimumAmplitude=1e-6;
    std::size_t maximumSamples=0;
    bool recordSpikes=false; // Disabled means unknown spike response, not zero.
    // Explicit model rest-state values. Never inferred to be zero.
    std::vector<double> baseline;
    std::vector<PeakWindow> windows;
    std::vector<std::string> expectedStimulusIds;
    // Schema 1: unspecified source (legacy/internal controlled signals).
    // Schemas 2/3: actual declared generator physics, bound across probes.
    std::optional<StimulusProfile> stimulusProfile;
    // Schema 3 additionally requires a complete order-preserving response wave
    // sequence on every measured channel. No model delay fills a missing wave.
    WaveAssignmentPolicy waveAssignment=WaveAssignmentPolicy::WindowOnly;
};
enum class PeakValidity { Valid, Missing, Ambiguous, Clipped, Incomplete };
struct PeakObservation {
    std::string inputId, stimulusId;
    PeakValidity validity=PeakValidity::Incomplete;
    // Only valid when validity==Valid. Times are measured sample timestamps;
    // a plateau reports its first and last sample, never an interpolated peak.
    std::optional<double> amplitude, rawPotential, time, plateauEnd;
    // Unknown until the matching actual stimulus exists, even if a peak exists.
    std::optional<double> delayFromStimulus;
    std::size_t candidateCount=0;
};
struct StimulusObservation { std::string id; double time; };
struct SpikeObservation { std::size_t channel; double time; };
struct BurstWave {
    std::string stimulusId; // Empty when a unique full assignment was not obtained.
    double amplitude=0.,rawPotential=0.,time=0.,plateauEnd=0.;
};
struct BurstAssignmentObservation {
    std::size_t channel=0;
    PeakValidity validity=PeakValidity::Incomplete;
    std::vector<BurstWave> waves;
};
struct ProbeObservation {
    unsigned schemaVersion=1;
    ProbeSpec specification;
    bool complete=false, valid=false;
    std::string endReason;
    std::vector<std::string> reasons;
    std::vector<StimulusObservation> stimuli;
    std::vector<SpikeObservation> spikes; // Count is derived from this list.
    std::vector<PeakObservation> peaks;
    std::size_t sampleCount=0;
    std::vector<BurstAssignmentObservation> burstAssignments;
};

// Consumes observations only. The adapter owns native scheduling, physical
// reset, stimulus routing, and a semantic model-revision provider.
class ProbeRecorder {
public:
    explicit ProbeRecorder(ProbeSpec specification);
    void Observe(double time, const std::vector<double>& potentials,
                 const std::string& currentModelRevision);
    void RecordStimulus(const std::string& id, double time);
    void RecordSpike(std::size_t channel, double time);
    ProbeObservation Close(double time, const std::string& currentModelRevision,
                           const std::string& endReason);
private:
    struct Frame { double time; std::vector<double> potentials; };
    ProbeSpec spec;
    std::vector<Frame> frames;
    std::vector<StimulusObservation> stimuli;
    std::vector<SpikeObservation> spikes;
    std::vector<std::string> reasons;
    bool closed=false, coverageBroken=false;
    void Reason(const std::string& reason);
    PeakObservation Measure(const PeakWindow& window, bool complete) const;
    BurstAssignmentObservation AssignBurst(std::size_t channel,bool complete) const;
};
const char* PeakValidityName(PeakValidity validity);
}
