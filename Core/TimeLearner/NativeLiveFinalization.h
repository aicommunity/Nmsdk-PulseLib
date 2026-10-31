#pragma once
#include "NativeFinalizationBoundary.h"
#include "NativeFinalizationControlPolicy.h"
#include "NativePostTuneConfiguration.h"
#include <map>
#include <optional>

namespace NMSDK::TimeLearner {
// Explicit runtime declaration, not a dynamic checkpoint. The same immutable
// conversion and measurement design is used after a genuine live Train ends.
// Target-reference calibration remains a distinct phase and is not silently
// substituted with a target/foil grid.
struct NativeLiveFinalizationSpec {
    NativeRecognitionSpec conversion;
    NativePostTuneTransactionSpec postTune;
    std::string implementationId;
};

class NativeLiveFinalization {
public:
    explicit NativeLiveFinalization(NativeLiveFinalizationSpec value):spec(std::move(value)) {
        if(spec.implementationId.empty() || spec.conversion.postTune)
            throw std::invalid_argument("invalid_live_finalization_declaration");
        QJsonArray trials,thresholds;
        for(const auto& q:spec.postTune.calibration.trials)
            trials.append(EncodeNativeCalibrationRequest(DecodeNativeCalibrationRequest(EncodeNativeCalibrationRequest(q))));
        for(double t:spec.postTune.calibration.thresholds) {
            if(!Detail::Finite(t))throw std::invalid_argument("invalid_live_finalization_threshold");
            thresholds.append(t);
        }
        if(trials.size()<2 || trials.size()>10000)
            throw std::invalid_argument("invalid_live_finalization_design");
        declaration={{"kind","native_live_finalization_plan"},{"schema_version",1},
            {"implementation_id",QString::fromStdString(spec.implementationId)},
            {"conversion",EncodeNativeRecognitionSpec(spec.conversion)},
            {"post_tune",EncodeNativePostTuneControlsV3(spec.postTune)},
            {"trials",trials},{"thresholds",thresholds},
            {"maximum_calibration_candidates",double(spec.postTune.calibration.maximumCandidates)},
            {"confirmation",EncodeNativeThresholdConfirmationSpec(spec.postTune.calibration.confirmation)}};
        DecodeNativeThresholdConfirmationSpec(declaration["confirmation"].toObject());
        declaration["sha256"]=NmsdkTools::CanonicalModelDataSha256(declaration);
    }
    template<class Owner> void CheckControls(const Owner& owner) const {
        Require(owner.ControllerVersion.GetData()==2,"live_finalization_requires_controller_v2");
        Require(owner.SyncCapacitanceTrim.GetData()==0,"live_finalization_c_trim_not_yet_supported");
        Require(!owner.AutoCalibrateFixedLTZThreshold.GetData(),"live_finalization_target_reference_not_yet_supported");
        Require(owner.EnablePostTrainTuning.GetData(),"live_finalization_post_tune_request_missing");
        Require(NativePostTuneModeFromPublic(owner.PostTrainTipResistanceMode.GetData())==spec.postTune.mode &&
            NativePostTuneMetricFromPublic(owner.PostTrainMidMetric.GetData())==spec.postTune.metric,
            "live_finalization_public_mode_mismatch");
        Require(owner.EnablePostTrainMidThreshold.GetData()==
            (spec.postTune.finalization==NativePostTuneFinalization::Calibrate),"live_finalization_threshold_policy_mismatch");
        Require(!owner.UseCandidateMidOnNonSeparableTest.GetData(),"live_finalization_nonseparable_fallback_not_yet_supported");
        if(spec.postTune.weightProbeThreshold)
            Require(owner.PostTrainSilentThreshold.GetData()==*spec.postTune.weightProbeThreshold,
                "live_finalization_weight_threshold_mismatch");
        if(spec.postTune.mode==NativePostTuneControllerMode::CanonRmin ||
           spec.postTune.mode==NativePostTuneControllerMode::SearchSynthetic)
            Require(owner.TipResistanceCanonFloor.GetData()==spec.postTune.canonicalFloor,
                "live_finalization_canonical_floor_mismatch");
        if(spec.postTune.mode==NativePostTuneControllerMode::FlatLastR)
            Require(owner.TipResistanceCanonLast.GetData()==spec.postTune.canonicalLast,
                "live_finalization_canonical_last_mismatch");
        if(spec.postTune.mode==NativePostTuneControllerMode::SearchSynthetic)
            Require(owner.PostTrainTipSearchIters.GetData()==int(spec.postTune.searchPasses),
                "live_finalization_search_passes_mismatch");
    }
    template<class Owner> void Started(const Owner& owner) {
        Require(status=="armed","live_finalization_requires_new_declaration");
        CheckControls(owner);request=owner.NativeTrainingRequest();
        Require(!request.isEmpty(),"live_finalization_training_request_missing");status="training";
    }
    template<class Owner> void CheckTrainingDesign(const Owner& owner,bool branch) const {
        QJsonArray pattern;const auto& target=owner.InputPattern.GetData();
        Require(target.GetRows()>0 && target.GetCols()==1,"live_finalization_target_dimension_invalid");
        for(int i=0;i<target.GetRows();++i)pattern.append(target(i,0));
        const auto targetHash=NmsdkTools::CanonicalModelDataSha256(pattern).toStdString();
        std::set<std::string> ids;
        std::map<RecognitionResetPolicy,std::set<std::string>> labels;
        const bool ltz=spec.postTune.metric==NativePostTuneMetric::LTZone ||
            (spec.postTune.metric==NativePostTuneMetric::Auto && !branch);
        for(const auto& q:spec.postTune.calibration.trials) {
            Require(q.pattern.GetRows()==target.GetRows() && q.pattern.GetCols()==1 && ids.insert(q.trialId).second &&
                ((q.label=="target")== (q.patternHash==targetHash)) && (q.label=="target" || q.label=="foil") &&
                (!ltz || q.recordThresholdPotential),"invalid_live_finalization_trial_or_metric");
            double span=0.;for(int i=0;i<q.pattern.GetRows();++i)span+=q.pattern(i,0);
            const double ticks=q.duration*owner.GetTimeStep();
            Require(Detail::Finite(span) && q.duration>span && Detail::Finite(ticks) && ticks>=1. &&
                !Detail::After(ticks,std::round(ticks)) && !Detail::After(std::round(ticks),ticks) &&
                ticks+1.<=double(q.maximumSamples),"live_finalization_window_or_sample_budget_invalid");
            labels[q.resetPolicy].insert(q.label);
        }
        for(const auto& row:labels)
            Require(row.second==std::set<std::string>{"target","foil"},"unbalanced_live_finalization_design");
        if(spec.postTune.finalization==NativePostTuneFinalization::Calibrate)
            Require(!spec.postTune.calibration.thresholds.empty() && spec.postTune.calibration.thresholds.size()<=256 &&
                spec.postTune.calibration.maximumCandidates>0 && spec.postTune.calibration.maximumCandidates<=256,
                "invalid_live_finalization_calibration_budget");
        else Require(spec.postTune.calibration.thresholds.empty() && spec.postTune.calibration.maximumCandidates==0,
                "live_fixed_finalization_has_unused_calibration_grid");
    }
    // Called after the native observation is consumed, before mode0 closes the
    // request. Read-only capture here; structural edits are deferred to BeforeTick.
    template<class Owner> void TrainingFinished(Owner& owner,double time) {
        if(status!="training")return;
        try {
            Require(owner.NativeTrainingRequest()==request,"live_finalization_training_request_changed");
            boundary.emplace(CaptureNativeFinalizationBoundary(owner,spec.implementationId,true));
            capturedTime=time;
            if(boundary->DiagnosticOnly()) {status="stopped_diagnostic";reason="training_has_no_completed_certificate";}
            else {CheckControls(owner);status="queued";}
        } catch(const std::exception& error) {Fail(error.what());}
    }
    template<class Owner> void BeforeTick(Owner& owner,double time) {
        if(status=="queued") {
            bool edited=false;
            try {
                Require(boundary.has_value() && capturedTime && Detail::Finite(time) && time>*capturedTime,
                    "live_finalization_requires_next_native_tick");
                CheckControls(owner);
                ValidateNativeFinalizationEntry(owner,*boundary,spec.implementationId);
                const auto converted=ConvertNativeSoloToRecognition(boundary->Solo(),spec.conversion,
                    owner.GetStorage(),owner.GetEnvironment(),spec.implementationId);
                // Conversion is a private preview. Revalidate the live source
                // before import; neither preview nor this dispatcher calculates children.
                const auto result=owner.ImportNativeFinalizationState(converted,spec.implementationId,*boundary);
                edited=result.modelMayHaveChanged || result.status==NativeModelBuildStatus::Adopted;
                Require(result.status==NativeModelBuildStatus::Adopted,result.reason.c_str());
                owner.Build();owner.Init();
                Require(owner.ExportNativeRecognitionState(spec.implementationId)==converted,
                    "live_finalization_import_readback_changed");
                entryTime=time;status="post_tune";
                owner.BeginNativePostTunePhase(spec.postTune,spec.implementationId);
            } catch(const std::exception& error) {
                status=edited?"recovery_required":"failed";reason=error.what();
            }
        }
        ObservePhase(owner);
    }
    template<class Owner> void ObservePhase(Owner& owner) {
        if(status=="post_tune" && owner.NativePostTunePhaseDone()) {
            postTuneReport=owner.NativePostTunePhaseReport();postTuneExecution=owner.NativePostTunePhaseExecution();
            const auto terminal=postTuneReport["status"].toString();
            status=terminal=="recovery_required"?"recovery_required":"completed";
            reason=terminal.toStdString();
        }
    }
    void Interrupt(const std::string& why) {
        if(status=="training" || status=="queued" || status=="post_tune") {status="interrupted";reason=why;}
    }
    bool Pending() const {return status=="queued" || status=="post_tune";}
    bool BlocksCalculation() const {return status=="failed" || status=="recovery_required";}
    bool Done() const {return status!="armed" && status!="training" && !Pending();}
    QJsonObject Report() const {
        QJsonObject value{{"kind","native_live_finalization"},{"schema_version",1},{"plan",declaration},
            {"training_request",request},{"status",QString::fromStdString(status)},{"reason",QString::fromStdString(reason)},
            {"boundary",boundary?QJsonValue(boundary->Document()):QJsonValue(QJsonValue::Null)},
            {"captured_model_time",capturedTime?QJsonValue(*capturedTime):QJsonValue(QJsonValue::Null)},
            {"entry_model_time",entryTime?QJsonValue(*entryTime):QJsonValue(QJsonValue::Null)},
            {"post_tune",postTuneReport},{"post_tune_execution",postTuneExecution},
            {"dynamic_checkpoint",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
        value["sha256"]=NmsdkTools::CanonicalModelDataSha256(value);return value;
    }
private:
    NativeLiveFinalizationSpec spec;
    QJsonObject declaration,request,postTuneReport,postTuneExecution;
    std::optional<NativeFinalizationBoundary> boundary;
    std::optional<double> capturedTime,entryTime;
    std::string status="armed",reason;
    static void Require(bool value,const char* why) {if(!value)throw std::logic_error(why);}
    void Fail(const std::string& why) {status="failed";reason=why;}
};
}
