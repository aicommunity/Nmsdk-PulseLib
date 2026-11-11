#pragma once
#include "MeasuredResponse.h"
#include <string>

namespace NMSDK::TimeLearner {
struct ResponseDomainEvent {std::string id;double time=0.;};
struct ResponseDomainChannel {
    std::size_t channel=0;
    double baseline=0.;
    std::vector<ResponseSample> samples;
};
struct ResponseDomainObservation {
    std::string runId,trialId,sampleId,patternHash;
    std::string modelRevision,protocolRevision;
    double begin=0.,end=0.,maximumSampleGap=0.,minimumAmplitude=1e-6;
    bool complete=false;
    std::vector<ResponseDomainEvent> events;
    std::vector<ResponseDomainChannel> channels;
};
struct DeclaredResponseDomain {
    std::size_t channel=0;
    std::string stimulusId;
    double begin=0.,end=0.;
};
struct ResponseDomainSpec {
    unsigned schemaVersion=1;
    std::string modelRevision,protocolRevision,patternHash;
    std::vector<std::string> stimulusIds;
    std::vector<std::size_t> channelIds;
    // Exactly one domain per actual stimulus on each observed physical channel.
    // These are declared hypotheses, not proven physical delay bounds.
    std::vector<DeclaredResponseDomain> domains;
};
enum class DomainAssociationStatus {
    Hypothesis, NoCandidate, Ambiguous, Clipped, Incomplete,
    OutsideApplicability, InvalidSpecification, InvalidSamples
};
struct DomainChannelFeatures {
    std::size_t channel=0,firstSourceSample=0;
    MeasuredResponseFeatures features;
};
struct ResponseDomainAssociation {
    std::size_t channel=0;
    std::string stimulusId;
    DomainAssociationStatus status=DomainAssociationStatus::InvalidSpecification;
    std::string reason;
    // Indices into that channel's measured positiveMaxima, never predictions.
    std::vector<std::size_t> candidateMaxima;
    // Observed maximum and signed temporal offsets may be reported even when
    // association fails. These offsets are not proven propagation delays.
    std::optional<std::size_t> largestMeasuredMaximum;
    std::optional<double> actualStimulusTime,maximumOffsetLow,maximumOffsetHigh;
    std::optional<std::size_t> selectedMaximum;
    std::vector<std::string> competingStimulusIds;
};
struct ResponseDomainResult {
    std::string runId,trialId,sampleId,patternHash,modelRevision,protocolRevision;
    std::vector<DomainChannelFeatures> channels;
    std::vector<ResponseDomainAssociation> associations;
    std::vector<std::string> reasons;
    static constexpr const char* MethodId(){return "declared-response-domains-diagnostic-v1";}
    // Window exclusivity alone does not prove response origin. There is no
    // conversion to PeakObservation or CapturedReferences in this contract.
    static constexpr bool TrainingCertificateEligible(){return false;}
};
ResponseDomainResult AnalyzeDeclaredResponseDomains(const ResponseDomainSpec& spec,
    const ResponseDomainObservation& observation);
const char* DomainAssociationStatusName(DomainAssociationStatus status);
}
