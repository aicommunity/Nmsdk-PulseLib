#include "CompositeTrainingMeasurement.h"
#include "FinalVerification.h"
#include "CandidateWindowContractValidation.h"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace { void CompositeRequire(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);} }
void ValidateCompositeMeasurementContract(const VerificationProtocol& p) {
    CompositeRequire(p.schemaVersion==4 && bool(p.compositeMeasurement),"composite_protocol_required");
    const auto& c=*p.compositeMeasurement;const auto& m=c.contextPolicy;const auto& g=c.geometryPolicy;
    const auto n=p.inputs.size();
    CompositeRequire(c.schemaVersion==1 && c.methodId==DeclaredWindowBasinResult::MethodId() && n>=2 &&
        c.inputWindows.size()==n && p.stimulusProfile && g.schemaVersion==1 && m.schemaVersion==1,
        "invalid_composite_measurement_contract");
    CompositeRequire(n<=std::size_t(std::numeric_limits<int>::max()) && ValidStimulusProfile(*p.stimulusProfile) &&
        p.maximumSampleGap==1./p.stimulusProfile->frequencyHz,"invalid_composite_native_dimensions_or_sampling");
    for(const auto value:{m.comparisonRadiusTicks,m.maximumSamples,m.maximumTotalSamples,m.maximumNativeProbes,
        g.maximumInputs,g.maximumSamples,g.maximumTotalSamples})
        CompositeRequire(value<=9007199254740991ULL,"inexact_composite_policy_count");
    CompositeRequire(n<=std::numeric_limits<std::size_t>::max()-2 && g.maximumInputs>=n &&
        g.maximumSamples>=3 && g.maximumTotalSamples>=g.maximumSamples &&
        m.maximumSamples>=3 && m.maximumTotalSamples>=m.maximumSamples && m.maximumNativeProbes>=n+2 &&
        m.comparisonRadiusTicks>0 && m.comparisonRadiusTicks<m.maximumSamples &&
        std::isfinite(m.timeComparisonTolerance) && m.timeComparisonTolerance>=0. &&
        m.timeComparisonTolerance<.5/p.stimulusProfile->frequencyHz,
        "invalid_composite_measurement_budget_or_grid");
    for(double value:{m.baselineAbsolute,m.repeatabilityAbsolute,m.repeatabilityRelative,
        m.influenceAbsolute,m.influenceRelative,g.repeatabilityAbsolute})
        CompositeRequire(std::isfinite(value) && value>=0.,"invalid_composite_measurement_tolerance");
    for(const auto& w:c.inputWindows)
        CompositeRequire(std::isfinite(w.beginOffset) && std::isfinite(w.endOffset) && w.beginOffset>=0. &&
            w.endOffset>w.beginOffset,"invalid_composite_declared_window");
}
const std::string& PreparedCompositeMeasurement::GroupId() const {return identity->groupId;}
const std::string& PreparedCompositeMeasurement::LogicalTrialId() const {return identity->trialId;}
void PreparedCompositeMeasurement::Require(const VerificationProtocol& p,const std::string& run,const std::string& trial,
    std::size_t input,const std::string& revision,std::size_t epoch,CompositeMeasurementPurpose purpose,
    const std::string& ownerScope,const std::string& sourceOrigin,bool closedMeasurement) const {
    if(p.schemaVersion==5) {
        ValidateCandidateWindowMeasurementContract(p);
        CompositeRequire(purpose==CompositeMeasurementPurpose::Reference && !identity->candidateWindowProposal,
            "candidate_window_training_requires_proposal_admission");
    } else ValidateCompositeMeasurementContract(p);
    const auto live=identity->lease.lock();
    CompositeRequire(bool(live) && bool(live->requireCurrent),"expired_composite_training_attempt");
    live->requireCurrent();
    CompositeRequire(identity->protocol && *identity->protocol==p && identity->runId==run &&
        identity->trialId==trial && identity->inputIndex==input && identity->baseRevision==revision &&
        identity->modelEpoch==epoch && identity->purpose==purpose && !identity->effectiveRevision.empty() &&
        identity->generation>0 && identity->channel==(p.routing==ProbeRouting::ClassicBurst?input:0u) &&
        !identity->groupId.empty() && !identity->candidateWitnessId.empty() && !identity->ownerScope.empty() && !identity->sourceOrigin.empty() &&
        (ownerScope.empty() || ownerScope==identity->ownerScope) &&
        (sourceOrigin.empty() || sourceOrigin==identity->sourceOrigin),"composite_training_context_mismatch");
    CompositeRequire(bool(identity->requireLiveModel),"missing_composite_live_model_guard");
    identity->requireLiveModel(closedMeasurement);
}
void ValidatedCompositeMeasurement::RequirePrepared(const PreparedCompositeMeasurement& expected) const {
    CompositeRequire(prepared.identity==expected.identity,"composite_group_preparation_mismatch");
    CompositeRequire(!semanticProofId.empty() && std::isfinite(rawPotential) && std::isfinite(arrival) &&
        std::isfinite(plateauEnd) && plateauEnd>=arrival,"invalid_composite_training_scalar");
}
}
