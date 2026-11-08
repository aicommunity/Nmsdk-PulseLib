#pragma once
#include "DeclaredWindowBasinAssignment.h"
#include "MatchedOmissionPolicy.h"
#include "TrainingCapacitance.h"
#include "CompositeInputWindow.h"
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
struct VerificationProtocol;
class NativeCompositeTrainingAttempt;
class CapturedReferences;
class BoundedTrainingSession;
class FinalVerificationSession;
class NativeTrainingService;
// Opt-in protocol 4. Windows are declared before the first trial of a group;
// the selector never moves them or treats a timing prediction as an observation.
struct CompositeMeasurementContract {
    unsigned schemaVersion=1;
    std::string methodId=DeclaredWindowBasinResult::MethodId();
    MatchedOmissionPolicy contextPolicy;
    DeclaredWindowBasinPolicy geometryPolicy;
    std::vector<CompositeInputWindow> inputWindows;
    bool operator==(const CompositeMeasurementContract&) const=default;
};
enum class CompositeMeasurementPurpose { Reference,Search,Final };
// Control revocation, not a measurement-validity or admission flag.
class CompositeTrainingStopped : public std::runtime_error {
public:
    CompositeTrainingStopped():std::runtime_error("composite_training_request_not_active"){}
};
class CompositeTrainingBudgetExhausted : public std::runtime_error {
public:
    explicit CompositeTrainingBudgetExhausted(const char* reason):std::runtime_error(reason){}
};
namespace CompositeTrainingDetail {
// Factory-owned, process-local admission authority. A public boolean, decoded
// diagnostic, or copied native result cannot construct either closed type.
struct LiveLease { std::function<void()> requireCurrent; };
struct Identity {
    std::shared_ptr<const VerificationProtocol> protocol;
    std::weak_ptr<const LiveLease> lease;
    std::string ownerScope,sourceOrigin,candidateWitnessId,candidateBindingJson;
    std::string runId,trialId,groupId,baseRevision,effectiveRevision;
    std::size_t generation=0,inputIndex=0,channel=0,modelEpoch=0;
    CompositeMeasurementPurpose purpose=CompositeMeasurementPurpose::Reference;
    std::optional<TrainingCapacitance> capacitance;
    std::function<void(bool closedMeasurement)> requireLiveModel;
};
}
class PreparedCompositeMeasurement {
public:
    PreparedCompositeMeasurement(const PreparedCompositeMeasurement&)=default;
    PreparedCompositeMeasurement& operator=(const PreparedCompositeMeasurement&)=default;
    const std::string& GroupId() const;
    const std::string& LogicalTrialId() const;
private:
    explicit PreparedCompositeMeasurement(std::shared_ptr<const CompositeTrainingDetail::Identity> identity):identity(std::move(identity)){}
    std::shared_ptr<const CompositeTrainingDetail::Identity> identity;
    void Require(const VerificationProtocol&,const std::string& run,const std::string& trial,
        std::size_t input,const std::string& revision,std::size_t epoch,CompositeMeasurementPurpose purpose,
        const std::string& ownerScope={},const std::string& sourceOrigin={},bool closedMeasurement=false) const;
    friend class NativeCompositeTrainingAttempt;
    friend class CapturedReferences;
    friend class BoundedTrainingSession;
    friend class FinalVerificationSession;
    friend class ValidatedCompositeMeasurement;
    friend class NativeTrainingService;
};
class ValidatedCompositeMeasurement {
public:
    ValidatedCompositeMeasurement(const ValidatedCompositeMeasurement&)=default;
    ValidatedCompositeMeasurement& operator=(const ValidatedCompositeMeasurement&)=default;
    const PreparedCompositeMeasurement& Prepared() const {return prepared;}
    const std::string& SemanticProofId() const {return semanticProofId;}
    const std::string& CandidateBindingJson() const {return prepared.identity->candidateBindingJson;}
private:
    ValidatedCompositeMeasurement(PreparedCompositeMeasurement prepared,double raw,double arrival,double plateau,
        std::string proof):prepared(std::move(prepared)),rawPotential(raw),arrival(arrival),plateauEnd(plateau),semanticProofId(std::move(proof)){}
    PreparedCompositeMeasurement prepared;
    double rawPotential=0.,arrival=0.,plateauEnd=0.;
    std::string semanticProofId;
    void RequirePrepared(const PreparedCompositeMeasurement&) const;
    friend class NativeCompositeTrainingAttempt;
    friend class CapturedReferences;
    friend class BoundedTrainingSession;
    friend class FinalVerificationSession;
    friend class NativeTrainingService;
};
void ValidateCompositeMeasurementContract(const VerificationProtocol& protocol);
}
