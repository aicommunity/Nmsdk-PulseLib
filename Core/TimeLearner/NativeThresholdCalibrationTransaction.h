#pragma once
#include "NativeCalibratedState.h"
#include "NativeRecognitionObservationValidation.h"
#include "NativeRecognitionTransactionScope.h"
#include "ModelIdentity.h"
#include "../../../../Rdk/Core/Engine/UEnvironment.h"
#include <QJsonArray>
#include <algorithm>
#include <map>
#include <set>

namespace NMSDK::TimeLearner {
struct NativeThresholdGridSpec {
    NativeThresholdConfirmationSpec confirmation;
    std::vector<double> thresholds;
    std::size_t maximumCandidates=0;
    // Presentation order is vector order. Run IDs are allocated by the
    // transaction; all other fields are declared before any physical edit.
    std::vector<NativeRecognitionTrialRequest> trials;
};
enum class NativeCalibrationTransactionStatus {
    Running,Calibrated,NonSeparable,BudgetExhausted,ConfirmationFailed,
    Cancelled,Failed,RecoveryRequired
};

// An outer-loop driver for either native learner, using its public import and
// recognition-trial API. Advance is called BETWEEN native model ticks. It never
// calls Calculate or advances the model clock. The normal scheduler executes
// children; the driver consumes only complete results. No work at construction
// changes the live owner. Continuation in the middle of a probe is unsupported.
template<class Owner> class NativeThresholdCalibrationTransaction {
public:
    NativeThresholdCalibrationTransaction(Owner& owner,NativeThresholdGridSpec specification,
        std::string implementationId,std::shared_ptr<NativeRecognitionTransactionScope<Owner>> sharedScope={})
        :owner(owner),spec(std::move(specification)),implementation(std::move(implementationId)),
         scope(sharedScope?std::move(sharedScope):std::make_shared<NativeRecognitionTransactionScope<Owner>>(owner,implementation,"calibration_transaction"))
    {
        Require(owner.IsNativeRecognitionTrialIdle(),"calibration_transaction_requires_idle_recognition");
        source=owner.ExportNativeRecognitionState(implementation);
        Require(source["kind"]==QJsonValue("timelearner_recognition_state"),"calibration_transaction_requires_base_recognition");
        expectedPackage=source;scope->CheckOwner(owner,implementation);scope->CheckPackage(source);Validate();
    }
    NativeThresholdCalibrationTransaction(const NativeThresholdCalibrationTransaction&)=delete;
    NativeThresholdCalibrationTransaction& operator=(const NativeThresholdCalibrationTransaction&)=delete;
    ~NativeThresholdCalibrationTransaction(){if(!Done())Cancel("transaction_destroyed");}
    bool Done() const {return status!=NativeCalibrationTransactionStatus::Running;}
    NativeCalibrationTransactionStatus Status() const {return status;}
    const std::string& Reason() const {return reason;}
    const QJsonObject& CalibratedPackage() const {return calibratedPackage;}

    void Advance()
    {
        if(Done())return;
        try
        {
            CheckLifecycle();
            if(waiting)
            {
                CheckTrialOwnership();
                auto observation=owner.TakeNativeRecognitionTrialResult();
                if(!observation)return;
                waiting=false;
                const auto& request=current.requests.back();
                current.observations.push_back(std::move(*observation));
                const auto& observed=current.observations.back();
                ValidateNativeRecognitionObservation(request,observed);
                Require(observed.modelBefore==expectedPackage["model_revision"].toString().toStdString(),
                    "calibration_transaction_observation_model_changed");
            }
            CheckPackage();
            if(!installed)
            {
                if(confirming)
                {
                    auto rebuilt=Candidate(spec.thresholds[*selected]);
                    Require(rebuilt==records[*selected].candidate,"calibration_transaction_selected_rebuild_changed");
                    Install(rebuilt);
                }
                else Install(Candidate(spec.thresholds[records.size()]));
                installed=true;
            }
            if(current.requests.size()<spec.trials.size())
            {
                auto q=spec.trials[current.requests.size()];
                q.runId=spec.confirmation.attemptId+(confirming?"/confirmation":"/search/"+std::to_string(records.size()));
                owner.QueueNativeRecognitionTrial(q);current.requests.push_back(std::move(q));waiting=true;return;
            }
            auto assessment=AssessNativeThresholdResponses(current.requests,current.observations,targetHash,spec.confirmation.window);
            if(confirming)
            {
                confirmation=current;
                if(!assessment.criterionMet){Restore(NativeCalibrationTransactionStatus::ConfirmationFailed,"fresh_confirmation_criterion_not_met");return;}
                const auto complete=CreateNativeCalibratedState(records[*selected].candidate,spec.confirmation,
                    records[*selected].matrix,confirmation,owner.GetStorage(),owner.GetEnvironment(),implementation);
                Install(complete);calibratedPackage=complete;status=NativeCalibrationTransactionStatus::Calibrated;return;
            }
            records.push_back({expectedPackage,std::move(current),std::move(assessment)});
            current={};installed=false;
            if(records.size()<budget)return;
            for(std::size_t i=0;i<records.size();++i)
                if(records[i].assessment.criterionMet){selected=i;break;}
            if(!selected)
            {
                Restore(budget==spec.thresholds.size()?NativeCalibrationTransactionStatus::NonSeparable:
                    NativeCalibrationTransactionStatus::BudgetExhausted,"no_successful_measured_candidate");return;
            }
            confirming=true;
        }
        catch(const std::exception& error){Fail(error.what());}
    }
    // Non-mutating detach for Reset/Default/teardown hooks. Ordinary Cancel
    // still performs guarded rollback at an idle outer boundary.
    void InterruptOwnerLifecycle(const std::string& why)
    {
        if(Done())return;
        status=NativeCalibrationTransactionStatus::RecoveryRequired;
        reason=why.empty()?"owner_lifecycle_interrupted":why;
    }
    void Cancel(const std::string& why)
    {
        if(Done())return;
        try{Restore(NativeCalibrationTransactionStatus::Cancelled,why.empty()?"external_cancel":why);}
        catch(const std::exception& error){status=NativeCalibrationTransactionStatus::RecoveryRequired;reason=error.what();}
    }
    QJsonObject Report() const
    {
        QJsonArray thresholds,trials,rows;
        for(double value:spec.thresholds)thresholds.append(value);
        for(const auto& q:spec.trials)trials.append(EncodeNativeCalibrationRequest(q));
        for(std::size_t i=0;i<records.size();++i)rows.append(QJsonObject{
            {"candidate_index",double(i)},{"candidate",records[i].candidate},
            {"matrix",EncodeNativeCalibrationMatrix(records[i].matrix)},{"assessment",records[i].assessment.evidence}});
        QJsonObject report{{"kind","native_threshold_calibration_transaction"},{"schema_version",1},
            {"algorithm","declared-grid-fresh-confirmation-native-driver-v1"},
            {"source_recognition_sha256",source["sha256"]},{"source_recognition",source},{"thresholds",thresholds},
            {"maximum_candidates",double(spec.maximumCandidates)},{"trials",trials},
            {"attempt_id",QString::fromStdString(spec.confirmation.attemptId)},
            {"clock_id",QString::fromStdString(spec.confirmation.clockId)},
            {"protocol",QString::fromStdString(spec.confirmation.protocol)},
            {"data_partition_id",QString::fromStdString(spec.confirmation.dataPartitionId)},
            {"status",StatusName(status)},{"reason",QString::fromStdString(reason)},
            {"candidates",rows},{"selected_index",selected?QJsonValue(double(*selected)):QJsonValue(QJsonValue::Null)},
            {"confirmation",EncodeNativeCalibrationMatrix(confirmation)},
            {"active_matrix",EncodeNativeCalibrationMatrix(current)},
            {"criterion",QJsonObject{{"anchor",spec.confirmation.window.anchor==CalibrationAnchor::Onset?"onset":"last_stimulus"},
                {"begin_offset",spec.confirmation.window.beginOffset},
                {"end_offset",spec.confirmation.window.endOffset?QJsonValue(*spec.confirmation.window.endOffset):QJsonValue(QJsonValue::Null)},
                {"reject_target_outside_window",spec.confirmation.window.rejectTargetOutsideWindow}}},
            {"calibrated_package",calibratedPackage.isEmpty()?QJsonValue(QJsonValue::Null):QJsonValue(calibratedPackage)},
            {"calibrated",status==NativeCalibrationTransactionStatus::Calibrated},
            {"source_restored",sourceRestored},{"dynamic_checkpoint",false},
            {"grid_selection_embedded_in_model_proof",false},{"statistical_independence_verified",false},
            {"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
        report["sha256"]=NmsdkTools::CanonicalModelDataSha256(report);return report;
    }
private:
    struct Record {QJsonObject candidate;NativeCalibrationMatrix matrix;NativeCalibrationAssessment assessment;};
    Owner& owner;
    NativeThresholdGridSpec spec;
    std::string implementation,targetHash,reason;
    std::shared_ptr<NativeRecognitionTransactionScope<Owner>> scope;
    QJsonObject source,expectedPackage,calibratedPackage;
    NativeCalibrationMatrix current,confirmation;
    std::vector<Record> records;
    std::optional<std::size_t> selected;
    std::size_t budget=0;
    bool waiting=false,installed=false,confirming=false,sourceRestored=false;
    NativeCalibrationTransactionStatus status=NativeCalibrationTransactionStatus::Running;
    static void Require(bool value,const char* why){if(!value)throw std::logic_error(why);}
    static const char* StatusName(NativeCalibrationTransactionStatus s)
    {
        switch(s){case NativeCalibrationTransactionStatus::Running:return "running";
        case NativeCalibrationTransactionStatus::Calibrated:return "calibrated";
        case NativeCalibrationTransactionStatus::NonSeparable:return "nonseparable";
        case NativeCalibrationTransactionStatus::BudgetExhausted:return "budget_exhausted";
        case NativeCalibrationTransactionStatus::ConfirmationFailed:return "confirmation_failed";
        case NativeCalibrationTransactionStatus::Cancelled:return "cancelled";
        case NativeCalibrationTransactionStatus::Failed:return "failed";
        default:return "recovery_required";}
    }
    QJsonObject Candidate(double threshold)
    {return CreateNativeThresholdCandidate(source,threshold,owner.GetStorage(),owner.GetEnvironment(),implementation);}
    void Validate()
    {
        ValidateCalibrationResponseWindow(spec.confirmation.window);
        Require(!implementation.empty() && !spec.confirmation.attemptId.empty() && !spec.confirmation.clockId.empty() &&
            !spec.confirmation.dataPartitionId.empty() && (spec.confirmation.protocol=="historical_same_matrix" ||
            spec.confirmation.protocol=="frozen_independent_calibration"),"invalid_calibration_transaction_scope");
        Require(!spec.thresholds.empty() && spec.thresholds.size()<=256 && spec.maximumCandidates>0 &&
            spec.maximumCandidates<=256,"invalid_calibration_transaction_grid_budget");
        std::set<double> unique;
        for(double threshold:spec.thresholds)Require(Detail::Finite(threshold) && unique.insert(threshold).second,
            "duplicate_or_nonfinite_calibration_threshold");
        budget=std::min(spec.maximumCandidates,spec.thresholds.size());
        Require(spec.trials.size()>=2 && spec.trials.size()<=10000,"invalid_calibration_transaction_design_size");
        const auto solo=source["source_solo"].toObject();
        targetHash=solo["protocol"].toObject()["pattern_hash"].toString().toStdString();
        auto prepared=PrepareNativeRecognitionState(source,owner.GetStorage(),owner.GetEnvironment(),implementation);
        std::set<std::string> ids;std::map<RecognitionResetPolicy,std::set<std::string>> labels;
        std::map<std::string,std::pair<std::string,std::string>> samples;
        for(const auto& q:spec.trials)
        {
            Require(!q.trialId.empty() && ids.insert(q.trialId).second && !q.sampleId.empty() &&
                (q.label=="target" || q.label=="foil") && q.pattern.GetCols()==1 &&
                q.pattern.GetRows()==int(prepared->parameters.size()),"invalid_calibration_transaction_trial");
            Require(q.resetPolicy==RecognitionResetPolicy::PerTrial || q.resetPolicy==RecognitionResetPolicy::PreserveNeuronDynamics,
                "invalid_calibration_transaction_reset_policy");
            double span=0.;QJsonArray values;
            for(int i=0;i<q.pattern.GetRows();++i){const double value=q.pattern(i,0);
                Require(Detail::Finite(value) && value>=0.,"invalid_calibration_transaction_pattern");span+=value;values.append(value);}
            Require(q.patternHash==NmsdkTools::CanonicalModelDataSha256(values).toStdString() &&
                ((q.label=="target")==(!targetHash.empty() && q.patternHash==targetHash)),"calibration_transaction_pattern_identity_mismatch");
            const auto signature=std::make_pair(q.label,q.patternHash);
            const auto sample=samples.emplace(q.sampleId,signature);
            Require(sample.second || sample.first->second==signature,"calibration_transaction_sample_collision");
            const double ticks=q.duration*prepared->configuration.model.frequencyHz;
            Require(Detail::Finite(q.duration) && q.duration>span && Detail::Finite(ticks) &&
                !Detail::After(ticks,std::round(ticks)) && !Detail::After(std::round(ticks),ticks) && q.maximumSamples>=2 && q.maximumSamples<=10000000 &&
                ticks+1.<=double(q.maximumSamples),"invalid_calibration_transaction_window_or_budget");
            labels[q.resetPolicy].insert(q.label);
        }
        for(const auto& entry:labels)Require(entry.second==std::set<std::string>{"foil","target"},
            "unbalanced_calibration_transaction_design");
        // Capability check before editing the live model, including legacy LT
        // image aliases. Remaining candidates use the same immutable physics.
        Candidate(spec.thresholds.front());
    }
    void CheckLifecycle()
    {scope->CheckLifecycle();}
    void CheckTrialOwnership() const
    {
        Require(!current.requests.empty(),"calibration_transaction_trial_ownership_changed");
        scope->CheckTrialOwnership(current.requests.back().runId,current.requests.back().trialId);
    }
    void CheckPackage()
    {scope->CheckPackage(expectedPackage);}
    void Install(const QJsonObject& package)
    {scope->Install(package,expectedPackage);}
    void Restore(NativeCalibrationTransactionStatus terminal,const std::string& why)
    {
        scope->CheckRollback(expectedPackage);
        if(waiting)
        {
            Require(!current.requests.empty(),"calibration_transaction_trial_ownership_changed");
            current.observations.push_back(scope->CancelOwned(current.requests.back(),"calibration_transaction_cancel"));waiting=false;
        }
        Install(source);sourceRestored=true;status=terminal;reason=why;
    }
    void Fail(const std::string& why)
    {
        try{Restore(NativeCalibrationTransactionStatus::Failed,why);}
        catch(const std::exception& error){status=NativeCalibrationTransactionStatus::RecoveryRequired;
            reason=why+"; rollback: "+error.what();}
    }
};
}
