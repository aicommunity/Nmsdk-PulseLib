#pragma once
#include "NativeThresholdCalibrationTransaction.h"
#include <limits>

namespace NMSDK::TimeLearner {
enum class NativePostTuneMetric { Auto, LTZone, SomaSum };
enum class NativePostTuneControllerMode { Off, KeepDone, CanonRmin, FlatLastR, SearchSynthetic };
enum class NativePostTuneFinalization { Calibrate, KeepFixedThreshold };
struct NativePostTuneTransactionSpec {
    NativePostTuneControllerMode mode=NativePostTuneControllerMode::KeepDone;
    NativePostTuneMetric metric=NativePostTuneMetric::Auto;
    double canonicalFloor=0.,canonicalLast=0.,minimumPositiveGap=0.;
    std::vector<RecognitionResistanceBounds> resistanceBounds;
    unsigned searchPasses=12;
    std::size_t maximumCandidates=256,maximumRecordedSamples=2000000;
    NativeThresholdGridSpec calibration;
    NativePostTuneFinalization finalization=NativePostTuneFinalization::Calibrate;
};
enum class NativePostTuneTransactionStatus {
    Running,Calibrated,SearchUnresolved,BudgetExhausted,Cancelled,Failed,RecoveryRequired,WeightsSelected
};

// Full-pattern weight search with explicit fixed-threshold finalization or the REAL threshold grid and
// fresh confirmation. Advance runs between native ticks; never Calculate here.
// The same guarded scope is handed to calibration, so an identical foreign
// import cannot masquerade as the model installed by either controller.
template<class Owner> class NativePostTuneTransaction {
public:
    NativePostTuneTransaction(Owner& owner,NativePostTuneTransactionSpec specification,std::string implementationId)
        :owner(owner),spec(std::move(specification)),implementation(std::move(implementationId)),
         scope(std::make_shared<NativeRecognitionTransactionScope<Owner>>(owner,implementation,"post_tune_transaction"))
    {
        Require(owner.IsNativeRecognitionTrialIdle(),"post_tune_requires_idle_recognition");
        source=owner.ExportNativeRecognitionState(implementation);expected=source;
        Require(source["kind"]==QJsonValue("timelearner_recognition_state"),"post_tune_requires_base_recognition");
        baseSpec=DecodeNativeRecognitionSpec(source["conversion_spec"].toObject());
        Require(!baseSpec.postTune,"post_tune_continuation_requires_explicit_completed_checkpoint");
        auto prepared=PrepareNativeRecognitionState(source,owner.GetStorage(),owner.GetEnvironment(),implementation);
        branch=prepared->configuration.model.branch;inputCount=prepared->parameters.size();Validate();
        scope->CheckPackage(source);
    }
    NativePostTuneTransaction(const NativePostTuneTransaction&)=delete;
    NativePostTuneTransaction& operator=(const NativePostTuneTransaction&)=delete;
    ~NativePostTuneTransaction(){if(!Done())Cancel("transaction_destroyed");}
    bool Done() const {return status!=NativePostTuneTransactionStatus::Running;}
    bool IsCalibrating() const {return bool(calibration);}
    NativePostTuneTransactionStatus Status() const {return status;}
    const std::string& Reason() const {return reason;}
    const QJsonObject& CalibratedPackage() const {return status==NativePostTuneTransactionStatus::Calibrated?finalPackage:emptyPackage;}
    // A fixed-threshold result remains a recognition candidate with its
    // measured selection proof in Report(), never a calibrated-state proof.
    const QJsonObject& FinalPackage() const {return finalPackage;}
    void Advance()
    {
        if(Done())return;
        try
        {
            if(calibration){AdvanceCalibration();return;}
            scope->CheckLifecycle();
            if(waiting)
            {
                const auto& q=current.requests.back();scope->CheckTrialOwnership(q.runId,q.trialId);
                auto o=owner.TakeNativeRecognitionTrialResult();if(!o)return;
                waiting=false;current.observations.push_back(std::move(*o));
                ValidateNativeRecognitionObservation(q,current.observations.back());
                Require(current.observations.back().modelBefore==expected["model_revision"].toString().toStdString(),"post_tune_observation_revision_changed");
                recordedSamples+=current.observations.back().samples.size();
                Require(recordedSamples<=spec.maximumRecordedSamples,"post_tune_recording_budget_exceeded");
            }
            scope->CheckPackage(expected);
            if(installed && current.observations.size()==spec.calibration.trials.size())
            {
                const double gap=MeasureGap(current);records.push_back({expected,std::move(current),gap});current={};
                if(!best || gap>records[*best].gap)best=records.size()-1;
                installed=false;
            }
            if(!installed)
            {
                auto proposal=NextProposal();
                if(!proposal){FinishWeights();return;}
                ++proposals;
                QJsonObject candidate;
                try{auto edit=baseSpec;edit.postTune=*proposal;candidate=ConvertNativeSoloToRecognition(source["source_solo"].toObject(),edit,
                        owner.GetStorage(),owner.GetEnvironment(),implementation);}
                catch(const std::exception& error)
                {rejected.append(QJsonObject{{"proposal",double(proposals-1)},{"reason",QString::fromStdString(error.what())}});return;}
                const auto identity=candidate["model_revision"].toString().toStdString();
                if(!seen.insert(identity).second)
                {rejected.append(QJsonObject{{"proposal",double(proposals-1)},{"reason","duplicate_effective_model"}});return;}
                scope->Install(candidate,expected);installed=true;
            }
            auto q=spec.calibration.trials[current.requests.size()];
            if(ObservationSampleBound(q)>spec.maximumRecordedSamples-recordedSamples)
            {Restore(NativePostTuneTransactionStatus::BudgetExhausted,"weight_observation_budget");return;}
            q.runId=spec.calibration.confirmation.attemptId+"/weights/"+std::to_string(records.size());
            owner.QueueNativeRecognitionTrial(q);current.requests.push_back(std::move(q));waiting=true;
        }
        catch(const std::exception& error){Fail(error.what());}
    }
    void Cancel(const std::string& why)
    {
        if(Done())return;
        try
        {
            if(calibration)
            {
                calibration->Cancel(why);calibrationReport=calibration->Report();
                Require(calibration->Status()!=NativeCalibrationTransactionStatus::RecoveryRequired,"post_tune_calibration_recovery_required");
                expected=records[*best].candidate;scope->CheckPackage(expected);calibration.reset();
            }
            Restore(NativePostTuneTransactionStatus::Cancelled,why.empty()?"external_cancel":why);
        }
        catch(const std::exception& error){status=NativePostTuneTransactionStatus::RecoveryRequired;reason=error.what();}
    }
    QJsonObject Report() const
    {
        QJsonArray rows,trials,bounds,thresholds;
        for(const auto& q:spec.calibration.trials)trials.append(EncodeNativeCalibrationRequest(q));
        for(const auto& b:spec.resistanceBounds)bounds.append(QJsonObject{{"minimum",b.minimum},{"maximum",b.maximum}});
        for(double t:spec.calibration.thresholds)thresholds.append(t);
        for(const auto& r:records)rows.append(QJsonObject{{"candidate",r.candidate},{"matrix",EncodeNativeCalibrationMatrix(r.matrix)},{"measured_gap",r.gap}});
        QJsonObject result{{"kind","native_post_tune_transaction"},{"schema_version",1},
            {"algorithm","bounded-coordinate-full-pattern-native-calibration-v1"},{"source_recognition",source},
            {"mode",ModeName(spec.mode)},{"metric",spec.metric==NativePostTuneMetric::Auto?"auto":spec.metric==NativePostTuneMetric::LTZone?"ltzone":"soma_sum"},
            {"canonical_floor",spec.canonicalFloor},{"canonical_last",spec.canonicalLast},{"resistance_bounds",bounds},
            {"minimum_positive_gap",spec.minimumPositiveGap},{"search_passes",int(spec.searchPasses)},
            {"maximum_candidates",double(spec.maximumCandidates)},{"maximum_recorded_samples",double(spec.maximumRecordedSamples)},
            {"recorded_weight_samples",double(recordedSamples)},{"proposals",double(proposals)},{"search_termination",QString::fromStdString(searchTermination)},
            {"trials",trials},{"candidates",rows},{"rejected_proposals",rejected},{"active_matrix",EncodeNativeCalibrationMatrix(current)},
            {"thresholds",thresholds},{"maximum_calibration_candidates",double(spec.calibration.maximumCandidates)},
            {"confirmation_specification",EncodeNativeThresholdConfirmationSpec(spec.calibration.confirmation)},
            {"selected_index",best?QJsonValue(double(*best)):QJsonValue(QJsonValue::Null)},
            {"calibration",calibration?calibration->Report():calibrationReport},
            {"status",StatusName(status)},{"reason",QString::fromStdString(reason)},{"source_restored",sourceRestored},
            {"calibrated_package",finalPackage},{"dynamic_checkpoint",false},{"statistical_independence_verified",false},
            {"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
        if(spec.finalization==NativePostTuneFinalization::KeepFixedThreshold)
        {
            result["schema_version"]=2;result["algorithm"]="bounded-coordinate-fixed-threshold-native-driver-v1";
            result["finalization"]="keep_fixed_threshold";result["calibration_status"]="fixed";
            result["calibrated_package"]=QJsonObject{};
            result["selected_package"]=status==NativePostTuneTransactionStatus::WeightsSelected?
                QJsonValue(finalPackage):QJsonValue(QJsonValue::Null);
            result["selection_proof_embedded_in_model"]=false;
            result["recognition_success_confirmed"]=false;
        }
        result["sha256"]=NmsdkTools::CanonicalModelDataSha256(result);return result;
    }
private:
    struct Record {QJsonObject candidate;NativeCalibrationMatrix matrix;double gap;};
    Owner& owner;NativePostTuneTransactionSpec spec;std::string implementation,reason,searchTermination;
    std::shared_ptr<NativeRecognitionTransactionScope<Owner>> scope;
    std::unique_ptr<NativeThresholdCalibrationTransaction<Owner>> calibration;
    NativeRecognitionSpec baseSpec;QJsonObject source,expected,finalPackage,emptyPackage,calibrationReport;QJsonArray rejected;
    NativeCalibrationMatrix current;std::vector<Record> records;std::optional<std::size_t> best;
    std::set<std::string> seen;std::size_t inputCount=0,proposals=0,recordedSamples=0,coordinate=0;
    unsigned pass=0,multiplier=0;bool branch=false,installed=false,waiting=false,sourceRestored=false;
    NativePostTuneTransactionStatus status=NativePostTuneTransactionStatus::Running;
    static void Require(bool value,const char* why){if(!value)throw std::logic_error(why);}
    static QJsonValue Threshold(const QJsonObject& p)
    {
        return p.value("neuron").toObject().value("metadata").toObject()
            .value("calibration").toObject().value("threshold");
    }
    static const char* ModeName(NativePostTuneControllerMode mode)
    {
        switch(mode){case NativePostTuneControllerMode::Off:return "off";case NativePostTuneControllerMode::KeepDone:return "keep_done";
        case NativePostTuneControllerMode::CanonRmin:return "canon_rmin";case NativePostTuneControllerMode::FlatLastR:return "flat_last_r";
        case NativePostTuneControllerMode::SearchSynthetic:return "search_synthetic";}
        throw std::logic_error("unknown_post_tune_mode");
    }
    static const char* StatusName(NativePostTuneTransactionStatus s)
    {
        switch(s){case NativePostTuneTransactionStatus::Running:return "running";case NativePostTuneTransactionStatus::Calibrated:return "calibrated";
        case NativePostTuneTransactionStatus::WeightsSelected:return "weights_selected";
        case NativePostTuneTransactionStatus::SearchUnresolved:return "search_unresolved";case NativePostTuneTransactionStatus::BudgetExhausted:return "budget_exhausted";
        case NativePostTuneTransactionStatus::Cancelled:return "cancelled";case NativePostTuneTransactionStatus::Failed:return "failed";
        default:return "recovery_required";}
    }
    bool LtzMetric() const {return spec.metric==NativePostTuneMetric::LTZone || (spec.metric==NativePostTuneMetric::Auto && !branch);}
    void Validate()
    {
        Require(spec.finalization==NativePostTuneFinalization::Calibrate ||
            spec.finalization==NativePostTuneFinalization::KeepFixedThreshold,"unknown_post_tune_finalization");
        ModeName(spec.mode);Require(spec.metric==NativePostTuneMetric::Auto || spec.metric==NativePostTuneMetric::LTZone ||
            spec.metric==NativePostTuneMetric::SomaSum,"unknown_post_tune_metric");
        Require(inputCount>0 && inputCount<=32 && spec.maximumCandidates>0 && spec.maximumCandidates<=4096 &&
            spec.searchPasses>0 && spec.searchPasses<=128 && spec.maximumRecordedSamples>0 && spec.maximumRecordedSamples<=100000000 &&
            Detail::Finite(spec.minimumPositiveGap) && spec.minimumPositiveGap>=0.,"invalid_post_tune_budget_or_gap");
        Require(!spec.calibration.confirmation.attemptId.empty() && spec.calibration.trials.size()>=2 &&
            spec.calibration.trials.size()<=10000,"invalid_post_tune_design");
        DecodeNativeThresholdConfirmationSpec(EncodeNativeThresholdConfirmationSpec(spec.calibration.confirmation));
        if(spec.finalization==NativePostTuneFinalization::Calibrate)
            Require(!spec.calibration.thresholds.empty() && spec.calibration.thresholds.size()<=256 &&
                spec.calibration.maximumCandidates>0 && spec.calibration.maximumCandidates<=256,"invalid_post_tune_calibration_budget");
        else
        {
            Require(spec.calibration.thresholds.empty() && spec.calibration.maximumCandidates==0,
                "fixed_post_tune_has_unused_calibration_grid");
            const auto fixed=Threshold(source);
            Require(fixed.isDouble() && Detail::Finite(fixed.toDouble()),"fixed_post_tune_requires_known_threshold");
        }
        std::set<double> thresholds;for(double t:spec.calibration.thresholds)
            Require(Detail::Finite(t) && thresholds.insert(t).second,"invalid_post_tune_threshold");
        const auto target=source["source_solo"].toObject()["protocol"].toObject()["pattern_hash"].toString().toStdString();
        std::set<std::string> ids;std::map<RecognitionResetPolicy,std::set<std::string>> labels;
        std::map<std::string,std::pair<std::string,std::string>> samples;
        for(const auto& q:spec.calibration.trials)
        {
            DecodeNativeCalibrationRequest(EncodeNativeCalibrationRequest(q));
            Require(q.pattern.GetRows()==int(inputCount) && q.pattern.GetCols()==1 && ids.insert(q.trialId).second &&
                ((q.label=="target")== (q.patternHash==target)) && (q.label=="target" || q.label=="foil") &&
                (!LtzMetric() || q.recordThresholdPotential),"invalid_post_tune_trial_or_metric");
            const auto signature=std::make_pair(q.label,q.patternHash);const auto sample=samples.emplace(q.sampleId,signature);
            Require(sample.second || sample.first->second==signature,"post_tune_sample_collision");
            double span=0.;for(int i=0;i<q.pattern.GetRows();++i)span+=q.pattern(i,0);
            const double ticks=q.duration*owner.GetTimeStep();
            Require(Detail::Finite(span) && q.duration>span && Detail::Finite(ticks) && ticks>=1. &&
                !Detail::After(ticks,std::round(ticks)) && !Detail::After(std::round(ticks),ticks) &&
                ticks+1.<=double(q.maximumSamples),"post_tune_window_or_sample_budget_invalid");
            labels[q.resetPolicy].insert(q.label);
        }
        for(const auto& row:labels)Require(row.second==std::set<std::string>{"target","foil"},"unbalanced_post_tune_design");
        // Pure native preview validates all mode-specific fields/bounds before
        // touching the owner's physical graph, even for the synthetic seed.
        auto s=baseSpec;s.postTune=InitialProposal();
        ConvertNativeSoloToRecognition(source["source_solo"].toObject(),s,owner.GetStorage(),owner.GetEnvironment(),implementation);
        if(spec.finalization==NativePostTuneFinalization::Calibrate)
            CreateNativeThresholdCandidate(source,spec.calibration.thresholds.front(),owner.GetStorage(),owner.GetEnvironment(),implementation);
    }
    NativePostTuneSpec InitialProposal() const
    {
        NativePostTuneSpec p;
        switch(spec.mode){case NativePostTuneControllerMode::Off:p.mode=NativePostTuneMode::Off;break;
        case NativePostTuneControllerMode::KeepDone:p.mode=NativePostTuneMode::KeepDone;break;
        case NativePostTuneControllerMode::FlatLastR:p.mode=NativePostTuneMode::FlatLastR;break;
        default:p.mode=NativePostTuneMode::CanonRmin;break;}
        p.sourceModelRevision=source["model_revision"].toString().toStdString();p.canonicalFloor=spec.canonicalFloor;
        p.canonicalLast=spec.canonicalLast;p.resistanceBounds=spec.resistanceBounds;return p;
    }
    std::optional<NativePostTuneSpec> NextProposal()
    {
        if(proposals>0 && (spec.mode!=NativePostTuneControllerMode::SearchSynthetic || pass>=spec.searchPasses))
        {searchTermination="declared_schedule_complete";return {};}
        if(proposals>=spec.maximumCandidates){searchTermination="candidate_budget_exhausted";return {};}
        if(proposals==0)return InitialProposal();
        NativePostTuneSpec p;p.mode=NativePostTuneMode::SearchSyntheticCandidate;
        p.sourceModelRevision=source["model_revision"].toString().toStdString();p.resistanceBounds=spec.resistanceBounds;
        const auto base=best?records[*best].candidate:source;
        for(const auto& value:base["recognition_inputs"].toArray())p.requestedResistances.push_back(value.toObject()["resistance"].toDouble());
        static const double factors[]={.5,1.,2.};p.requestedResistances[coordinate]*=factors[multiplier];
        if(++multiplier==3){multiplier=0;if(++coordinate==inputCount){coordinate=0;++pass;}}
        return p;
    }
    double MeasureGap(const NativeCalibrationMatrix& matrix) const
    {
        double target=std::numeric_limits<double>::infinity(),foil=-std::numeric_limits<double>::infinity();
        for(std::size_t i=0;i<matrix.observations.size();++i)
        {
            const auto& o=matrix.observations[i];double peak=-std::numeric_limits<double>::infinity();
            for(const auto& sample:o.samples)
            {
                double v=0.;if(LtzMetric())v=sample.thresholdPotential;
                else {Require(!sample.somaPotentials.empty(),"missing_post_tune_soma");for(double s:sample.somaPotentials)v+=s;}
                Require(Detail::Finite(v),"nonfinite_post_tune_metric");peak=std::max(peak,v);
            }
            Require(Detail::Finite(peak),"missing_post_tune_peak");
            if(matrix.requests[i].label=="target")target=std::min(target,peak);else foil=std::max(foil,peak);
        }
        Require(Detail::Finite(target-foil),"invalid_post_tune_gap");return target-foil;
    }
    void FinishWeights()
    {
        if(!best || !(records[*best].gap>spec.minimumPositiveGap))
        {Restore(searchTermination=="candidate_budget_exhausted"?NativePostTuneTransactionStatus::BudgetExhausted:
            NativePostTuneTransactionStatus::SearchUnresolved,"no_positive_measured_gap");return;}
        if(spec.finalization==NativePostTuneFinalization::KeepFixedThreshold)
        {
            // No threshold candidate, threshold search, silent-threshold
            // workaround or fabricated calibration proof. Weight selection
            // alone never asserts target detection or absence of false spikes.
            Require(Threshold(records[*best].candidate)==Threshold(source),"fixed_post_tune_changed_threshold");
            scope->Install(records[*best].candidate,expected);scope->CheckPackage(expected);
            finalPackage=expected;status=NativePostTuneTransactionStatus::WeightsSelected;return;
        }
        std::size_t matrixBudget=0;for(const auto& q:spec.calibration.trials)
        {
            const auto bound=ObservationSampleBound(q);
            if(bound>spec.maximumRecordedSamples-matrixBudget)
            {Restore(NativePostTuneTransactionStatus::BudgetExhausted,"calibration_observation_budget");return;}
            matrixBudget+=bound;
        }
        const auto matrices=std::min(spec.calibration.maximumCandidates,spec.calibration.thresholds.size())+1;
        if(matrixBudget>(spec.maximumRecordedSamples-recordedSamples)/matrices)
        {Restore(NativePostTuneTransactionStatus::BudgetExhausted,"calibration_observation_budget");return;}
        // Start afresh from the immutable selected archive, not its probe dynamics.
        scope->Install(records[*best].candidate,expected);
        calibration=std::make_unique<NativeThresholdCalibrationTransaction<Owner>>(owner,spec.calibration,implementation,scope);
    }
    void AdvanceCalibration()
    {
        calibration->Advance();if(!calibration->Done())return;
        calibrationReport=calibration->Report();
        if(calibration->Status()==NativeCalibrationTransactionStatus::RecoveryRequired)
        {status=NativePostTuneTransactionStatus::RecoveryRequired;reason=calibration->Reason();return;}
        if(calibration->Status()==NativeCalibrationTransactionStatus::Calibrated)
        {
            const auto complete=calibration->CalibratedPackage();scope->CheckPackage(complete);
            finalPackage=complete;expected=complete;
            calibration.reset();status=NativePostTuneTransactionStatus::Calibrated;return;
        }
        const auto childStatus=calibration->Status();const auto why=calibration->Reason();
        expected=records[*best].candidate;scope->CheckPackage(expected);calibration.reset();
        Restore(childStatus==NativeCalibrationTransactionStatus::BudgetExhausted?NativePostTuneTransactionStatus::BudgetExhausted:
            NativePostTuneTransactionStatus::SearchUnresolved,"selected_weights_not_confirmed: "+why);
    }
    void Restore(NativePostTuneTransactionStatus terminal,const std::string& why)
    {
        scope->CheckRollback(expected);
        if(waiting){current.observations.push_back(scope->CancelOwned(current.requests.back(),"post_tune_cancel"));waiting=false;}
        scope->Install(source,expected);sourceRestored=true;status=terminal;reason=why;
    }
    std::size_t ObservationSampleBound(const NativeRecognitionTrialRequest& q) const
    {
        // The validated single-frequency runner samples once per native tick.
        // maximumSamples is a safety cap, not a request to allocate that many
        // frames. Include both boundaries and one rounding/closure guard tick.
        const auto physical=std::size_t(std::ceil(q.duration*owner.GetTimeStep()))+2;
        return std::min(q.maximumSamples,physical);
    }
    void Fail(const std::string& why)
    {
        // A delegated transaction owns its active trial. Never cancel or roll
        // back through the parent while the child still has custody.
        if(calibration){status=NativePostTuneTransactionStatus::RecoveryRequired;reason=why;return;}
        try{Restore(NativePostTuneTransactionStatus::Failed,why);}
        catch(const std::exception& e){status=NativePostTuneTransactionStatus::RecoveryRequired;reason=why+"; rollback: "+e.what();}
    }
};
}
