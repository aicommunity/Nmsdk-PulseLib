#pragma once
#include "TrainingParameters.h"
#include "TrainingCapacitance.h"
#include <QJsonArray>
#include <QJsonObject>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace NMSDK::TimeLearner::EvidenceJsonDetail {
inline QString Text(const std::string& value){return QString::fromStdString(value);}
inline QJsonValue Number(double value)
{if(!std::isfinite(value))throw std::invalid_argument("nonfinite_native_probe_evidence_number");return value;}
inline QJsonValue Count(std::size_t value)
{if(value>9007199254740991ULL)throw std::invalid_argument("inexact_native_probe_evidence_integer");return double(value);}
inline QJsonArray Parameters(const std::vector<InputParameters>& values)
{
    QJsonArray result;for(const auto& p:values)result.append(QJsonObject{
        {"length",p.length},{"synapses",p.synapses},{"resistance",Number(p.resistance)}});return result;
}
inline QJsonObject CapacitanceParameters(const TrainingCapacitance& value)
{
    ValidateTrainingCapacitance(value);QJsonObject scales;
    for(const auto& [path,scale]:value.membraneScales)scales[Text(path)]=Number(scale);
    return {{"origin",Text(value.origin)},{"maximum_relative_change",Number(value.maximumRelativeChange)},
        {"membrane_scales",scales}};
}
}
