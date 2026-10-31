#pragma once
#include "NumericChecks.h"
#include <optional>
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
enum class CalibrationAnchor { Onset,LastStimulus };
struct CalibrationResponseWindow {
    CalibrationAnchor anchor=CalibrationAnchor::LastStimulus;
    double beginOffset=0.;
    std::optional<double> endOffset;
    bool rejectTargetOutsideWindow=true;
};
inline void ValidateCalibrationResponseWindow(const CalibrationResponseWindow& window)
{
    if(window.anchor!=CalibrationAnchor::Onset && window.anchor!=CalibrationAnchor::LastStimulus)
        throw std::invalid_argument("unknown_calibration_anchor");
    if(!Detail::Finite(window.beginOffset) || window.beginOffset<0. ||
        (window.endOffset && (!Detail::Finite(*window.endOffset) || *window.endOffset<window.beginOffset)))
        throw std::invalid_argument("invalid_calibration_response_window");
}
inline std::pair<double,double> CalibrationResponseBounds(const CalibrationResponseWindow& window,
    double observationBegin,double observationEnd,double firstStimulus,double lastStimulus)
{
    ValidateCalibrationResponseWindow(window);
    const double anchor=window.anchor==CalibrationAnchor::Onset?firstStimulus:lastStimulus;
    const double begin=anchor+window.beginOffset,end=window.endOffset?anchor+*window.endOffset:observationEnd;
    if(!Detail::Finite(begin) || !Detail::Finite(end) || Detail::After(observationBegin,begin) ||
        Detail::After(begin,end) || Detail::After(end,observationEnd))
        throw std::invalid_argument("calibration_response_window_outside_observation");
    return {begin,end};
}
}
