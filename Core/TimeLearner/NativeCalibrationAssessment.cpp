#include "NativeCalibrationAssessment.h"
#include "NativeRecognitionObservationValidation.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <map>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
QString Text(const std::string& value){return QString::fromStdString(value);}
QString Policy(RecognitionResetPolicy policy)
{return policy==RecognitionResetPolicy::PerTrial?"reset_per_trial":"preserve_neuron_dynamics";}
QJsonArray Numbers(const std::vector<double>& values)
{QJsonArray result;for(double value:values)result.append(value);return result;}
struct Counts {
    std::size_t tp=0,fp=0,fn=0,tn=0,outside=0;
    void Add(bool target,bool hit,bool targetOutside)
    {if(target){if(hit)++tp;else ++fn;if(targetOutside)++outside;}else{if(hit)++fp;else ++tn;}}
    QJsonObject Encode() const
    {
        const auto targets=tp+fn,foils=fp+tn,total=targets+foils;
        return {{"TP",double(tp)},{"FP",double(fp)},{"FN",double(fn)},{"TN",double(tn)},
            {"targets",double(targets)},{"foils",double(foils)},{"target_trials_with_outside_spikes",double(outside)},
            {"accuracy",total?QJsonValue(double(tp+tn)/double(total)):QJsonValue(QJsonValue::Null)},
            {"target_hit_rate",targets?QJsonValue(double(tp)/double(targets)):QJsonValue(QJsonValue::Null)},
            {"foil_response_rate",foils?QJsonValue(double(fp)/double(foils)):QJsonValue(QJsonValue::Null)}};
    }
};
}
NativeCalibrationAssessment AssessNativeThresholdResponses(
    const std::vector<NativeRecognitionTrialRequest>& expected,
    const std::vector<NativeRecognitionTrialObservation>& observations,
    const std::string& trainingPatternHash,const CalibrationResponseWindow& window)
{
    ValidateCalibrationResponseWindow(window);
    Require(!trainingPatternHash.empty() && expected.size()>=2 && expected.size()==observations.size(),
        "incomplete_native_calibration_assessment");
    using Key=std::pair<std::string,std::string>;
    std::map<Key,const NativeRecognitionTrialRequest*> wanted;
    std::map<Key,const NativeRecognitionTrialObservation*> actual;
    std::map<RecognitionResetPolicy,std::set<std::string>> labels;
    std::map<std::string,std::pair<std::string,std::string>> samples;
    const auto runId=expected.front().runId;
    for(const auto& q:expected)
    {
        Require(!runId.empty() && q.runId==runId && (q.label=="target" || q.label=="foil") &&
            ((q.label=="target")==(q.patternHash==trainingPatternHash)),"invalid_native_calibration_assessment_design");
        Require(wanted.emplace(Key{q.runId,q.trialId},&q).second,"duplicate_native_calibration_assessment_trial");
        labels[q.resetPolicy].insert(q.label);
        const auto signature=std::make_pair(q.label,q.patternHash);
        const auto [it,added]=samples.emplace(q.sampleId,signature);
        Require(added || it->second==signature,"native_calibration_assessment_sample_collision");
    }
    for(const auto& [policy,present]:labels)Require(present==std::set<std::string>{"foil","target"},
        "unbalanced_native_calibration_assessment_policy");
    std::string revision;std::optional<double> frequency;std::optional<std::size_t> channels;
    for(const auto& o:observations)
    {
        const Key key{o.request.runId,o.request.trialId};const auto found=wanted.find(key);
        Require(found!=wanted.end() && actual.emplace(key,&o).second,"unexpected_native_calibration_assessment_trial");
        ValidateNativeRecognitionObservation(*found->second,o);
        if(revision.empty()){revision=o.modelBefore;frequency=o.frequencyHz;channels=o.samples.front().somaPotentials.size();}
        Require(o.modelBefore==revision && o.frequencyHz==*frequency && o.samples.front().somaPotentials.size()==*channels,
            "native_calibration_assessment_model_changed");
    }
    NativeCalibrationAssessment result;Counts pooled;std::map<RecognitionResetPolicy,Counts> byPolicy;
    std::map<Key,QJsonObject> records;std::optional<double> previousEnd;
    for(const auto& q:expected)
    {
        const Key key{q.runId,q.trialId};const auto& o=*actual.at(key);
        Require(!previousEnd || Detail::After(o.begin,*previousEnd),"native_calibration_assessment_presentation_order_changed");
        previousEnd=o.end;
        const auto [begin,end]=CalibrationResponseBounds(window,o.begin,o.end,o.stimulusTimes.front(),o.stimulusTimes.back());
        std::vector<double> early,inside,late;
        for(double spike:o.spikeTimes)
        {
            if(Detail::After(begin,spike))early.push_back(spike);
            else if(Detail::After(spike,end))late.push_back(spike);
            else inside.push_back(spike);
        }
        const bool target=q.label=="target",hit=target?!inside.empty():!o.spikeTimes.empty();
        const bool outside=target && (!early.empty() || !late.empty());
        const bool targetViolation=target && (!hit || (window.rejectTargetOutsideWindow && outside));
        if(targetViolation)++result.targetViolations;if(!target && hit)++result.foilResponses;
        pooled.Add(target,hit,outside);byPolicy[q.resetPolicy].Add(target,hit,outside);
        records.emplace(key,QJsonObject{{"run_id",Text(q.runId)},{"trial_id",Text(q.trialId)},{"sample_id",Text(q.sampleId)},
            {"label",Text(q.label)},{"pattern_hash",Text(q.patternHash)},{"reset_policy",Policy(q.resetPolicy)},
            {"begin",o.begin},{"end",o.end},{"response_begin",begin},{"response_end",end},
            {"spike_count",double(o.spikeTimes.size())},{"inside_spikes",Numbers(inside)},
            {"early_spikes",Numbers(early)},{"late_spikes",Numbers(late)},
            {"classification",target?(hit?"TP":"FN"):(hit?"FP":"TN")},{"target_violation",targetViolation},
            {"observation_sha256",NmsdkTools::CanonicalModelDataSha256(NativeRecognitionTrialJson(o))}});
    }
    result.criterionMet=result.targetViolations==0 && result.foilResponses==0;
    QJsonArray trials;for(const auto& [key,row]:records)trials.append(row);
    QJsonObject policies;for(const auto& [policy,counts]:byPolicy)policies[Policy(policy)]=counts.Encode();
    result.evidence={{"kind","native_threshold_response_assessment"},{"schema_version",1},
        {"algorithm","post-anchor-target-whole-window-foil-v1"},{"run_id",Text(runId)},
        {"model_revision",Text(revision)},{"training_pattern_hash",Text(trainingPatternHash)},
        {"criterion",QJsonObject{{"id","post_anchor_target_whole_window_foil_v1"},
            {"anchor",window.anchor==CalibrationAnchor::Onset?"onset":"last_stimulus"},{"begin_offset",window.beginOffset},
            {"end_offset",window.endOffset?QJsonValue(*window.endOffset):QJsonValue(QJsonValue::Null)},
            {"reject_target_outside_window",window.rejectTargetOutsideWindow}}},
        {"validity","valid"},{"complete_observation_set",true},{"criterion_met",result.criterionMet},
        {"target_violations",double(result.targetViolations)},{"foil_responses",double(result.foilResponses)},
        {"pooled_counts",pooled.Encode()},{"by_reset_policy",policies},{"trials",trials},
        {"calibrated",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
    result.evidence["sha256"]=NmsdkTools::CanonicalModelDataSha256(result.evidence);
    return result;
}
}
