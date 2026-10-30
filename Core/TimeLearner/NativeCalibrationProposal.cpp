#include "NativeCalibrationProposal.h"
#include "ModelIdentity.h"
#include "NumericChecks.h"
#include "NativeTrainingBindings.h"
#include "NativeRecognitionObservationValidation.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
bool EqualTime(double a,double b){return !Detail::After(a,b) && !Detail::After(b,a);}
QString Text(const std::string& text){return QString::fromStdString(text);}
QJsonValue Optional(std::optional<double> value){return value?QJsonValue(*value):QJsonValue(QJsonValue::Null);}
void Maximum(std::optional<double>& value,double sample){if(!value || sample>*value)value=sample;}
}
NativeThresholdProposal ProposeNativeThreshold(const std::vector<NativeRecognitionTrialRequest>& expected,
    const std::vector<NativeRecognitionTrialObservation>& observations,const std::string& trainingPatternHash,
    const CalibrationResponseWindow& window,double minimumThreshold,double maximumThreshold)
{
    ValidateCalibrationResponseWindow(window);
    Require(Detail::Finite(minimumThreshold) && Detail::Finite(maximumThreshold) && minimumThreshold>0. &&
        minimumThreshold<=maximumThreshold,"invalid_calibration_threshold_bounds");
    Require(!trainingPatternHash.empty() && expected.size()>=2 && observations.size()==expected.size(),"incomplete_calibration_characterization");
    using Key=std::pair<std::string,std::string>;
    std::map<Key,const NativeRecognitionTrialRequest*> wanted;
    std::map<RecognitionResetPolicy,std::set<std::string>> labels;
    for(const auto& request:expected)
    {
        Require(!request.runId.empty() && !request.trialId.empty() && !request.sampleId.empty() &&
            (request.label=="target" || request.label=="foil") && request.recordThresholdPotential &&
            (request.resetPolicy==RecognitionResetPolicy::PerTrial || request.resetPolicy==RecognitionResetPolicy::PreserveNeuronDynamics) &&
            Detail::Finite(request.duration) && request.duration>0. &&
            ((request.label=="target")== (request.patternHash==trainingPatternHash)),"invalid_calibration_characterization_design");
        Require(request.pattern.GetCols()==1 && request.pattern.GetRows()>0,"invalid_calibration_characterization_pattern");
        QJsonArray sequence;
        for(int i=0;i<request.pattern.GetRows();++i)
        {
            const double value=request.pattern(i,0);
            Require(Detail::Finite(value) && value>=0.,"invalid_calibration_characterization_pattern");sequence.append(value);
        }
        Require(NmsdkTools::CanonicalModelDataSha256(sequence).toStdString()==request.patternHash,"calibration_characterization_pattern_hash_mismatch");
        Require(wanted.emplace(Key{request.runId,request.trialId},&request).second,"duplicate_calibration_characterization_id");
        labels[request.resetPolicy].insert(request.label);
    }
    for(const auto& [policy,present]:labels)Require(present==std::set<std::string>{"foil","target"},"unbalanced_calibration_characterization_policy");
    std::set<Key> seen;std::optional<double> targetMinimum,foilMaximum,targetOutside;
    std::string revision,path,zoneClass;std::map<Key,QJsonObject> records;
    for(const auto& o:observations)
    {
        const Key key{o.request.runId,o.request.trialId};const auto found=wanted.find(key);
        Require(found!=wanted.end() && seen.insert(key).second,"unexpected_calibration_characterization_id");
        const auto& q=*found->second;
        ValidateNativeRecognitionObservation(q,o);
        Require(o.request.sampleId==q.sampleId && o.request.label==q.label && o.request.patternHash==q.patternHash &&
            o.request.resetPolicy==q.resetPolicy && o.request.duration==q.duration && NativeTrainingPatternEqual(o.request.pattern,q.pattern),
            "calibration_characterization_request_changed");
        Require(o.request.recordThresholdPotential && o.armed && o.complete && o.valid && o.reason=="window_closed" &&
            o.modelBefore==o.modelAfter && !o.modelBefore.empty() && !o.thresholdPotentialPath.empty() && !o.thresholdPotentialClass.empty(),
            "invalid_calibration_characterization");
        Require(o.spikeTimes.empty(),"calibration_characterization_has_spikes");
        if(revision.empty()){revision=o.modelBefore;path=o.thresholdPotentialPath;zoneClass=o.thresholdPotentialClass;}
        Require(o.modelBefore==revision && o.thresholdPotentialPath==path && o.thresholdPotentialClass==zoneClass,
            "calibration_characterization_model_or_source_changed");
        Require(Detail::Finite(o.begin) && Detail::Finite(o.end) && o.end>o.begin && Detail::Finite(o.frequencyHz) && o.frequencyHz>0. &&
            EqualTime(o.end,o.begin+q.duration) && EqualTime(o.closedAt,o.end) && !o.samples.empty() &&
            EqualTime(o.samples.front().time,o.begin) && EqualTime(o.samples.back().time,o.end) &&
            !o.stimulusTimes.empty(),"incomplete_calibration_characterization_window");
        const auto [begin,end]=CalibrationResponseBounds(window,o.begin,o.end,o.stimulusTimes.front(),o.stimulusTimes.back());
        std::optional<double> inside,outside,full;std::optional<double> previous;
        for(const auto& sample:o.samples)
        {
            Require(Detail::Finite(sample.time) && Detail::Finite(sample.thresholdPotential) &&
                !Detail::After(o.begin,sample.time) && !Detail::After(sample.time,o.end) &&
                (!previous || (sample.time>*previous && !Detail::IntervalAfter(sample.time,*previous,1./o.frequencyHz))),
                "invalid_calibration_characterization_sample");
            previous=sample.time;Maximum(full,sample.thresholdPotential);
            if(!Detail::After(begin,sample.time) && !Detail::After(sample.time,end))Maximum(inside,sample.thresholdPotential);
            else Maximum(outside,sample.thresholdPotential);
        }
        Require(bool(inside),"missing_calibration_response_samples");
        if(q.label=="target")
        {
            if(!targetMinimum || *inside<*targetMinimum)targetMinimum=inside;
            if(outside)Maximum(targetOutside,*outside);
        }
        else Maximum(foilMaximum,*full);
        records.emplace(key,QJsonObject{{"run_id",Text(q.runId)},{"trial_id",Text(q.trialId)},{"sample_id",Text(q.sampleId)},
            {"label",Text(q.label)},{"pattern_hash",Text(q.patternHash)},{"begin",o.begin},{"end",o.end},
            {"response_begin",begin},{"response_end",end},{"maximum_inside",Optional(inside)},
            {"maximum_outside",Optional(outside)},{"maximum_full",Optional(full)},
            {"observation_sha256",NmsdkTools::CanonicalModelDataSha256(NativeRecognitionTrialJson(o))}});
    }
    Require(seen.size()==wanted.size() && targetMinimum && foilMaximum,"incomplete_calibration_characterization_set");
    const double lower=std::max({minimumThreshold,*foilMaximum,
        window.rejectTargetOutsideWindow && targetOutside?*targetOutside:minimumThreshold});
    const double upper=std::min(maximumThreshold,*targetMinimum);
    const double middle=std::midpoint(lower,upper);
    NativeThresholdProposal result;
    const char* status="no_measured_gap";
    const auto permitted=[&](double value) {
        return Detail::Finite(value) && value>=minimumThreshold && value<=maximumThreshold && value>*foilMaximum &&
            (!window.rejectTargetOutsideWindow || !targetOutside || value>*targetOutside) && value<*targetMinimum;
    };
    if(lower<upper)status="no_representable_threshold";
    if(permitted(middle))result.threshold=middle;
    else if(permitted(minimumThreshold))result.threshold=minimumThreshold;
    else if(permitted(maximumThreshold))result.threshold=maximumThreshold;
    if(result.threshold)status="proposed";
    QJsonArray orderedRecords;for(const auto& [key,record]:records)orderedRecords.append(record);
    result.evidence={{"kind","timelearner_threshold_proposal"},{"schema_version",1},
        {"algorithm","nonspiking-native-ltz-bracket-v1"},{"model_revision",Text(revision)},
        {"training_pattern_hash",Text(trainingPatternHash)},{"threshold_potential_path",Text(path)},
        {"threshold_potential_class",Text(zoneClass)},{"minimum_threshold",minimumThreshold},{"maximum_threshold",maximumThreshold},
        {"anchor",window.anchor==CalibrationAnchor::Onset?"onset":"last_stimulus"},{"begin_offset",window.beginOffset},
        {"end_offset",Optional(window.endOffset)},{"reject_target_outside_window",window.rejectTargetOutsideWindow},
        {"target_minimum_inside",*targetMinimum},{"foil_maximum_full",*foilMaximum},{"target_maximum_outside",Optional(targetOutside)},
        {"lower_bound",lower},{"upper_bound",upper},{"threshold",Optional(result.threshold)},{"status",status},
        {"trials",orderedRecords},{"calibrated",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
    result.evidence["sha256"]=NmsdkTools::CanonicalModelDataSha256(result.evidence);
    return result;
}
}
