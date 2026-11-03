#pragma once
#include "NativeCapacitanceEdit.h"
#include "NativeModelBuilder.h"
#include <QJsonDocument>
#include <optional>
#include <stdexcept>

namespace NMSDK::TimeLearner {
// Resolve only at an explicit edit/export boundary. The owner keeps one full
// immutable-origin state; callers may request a subset of absolute scales.
inline std::optional<NativeCapacitanceCorrection> ResolveOwnedNativeCapacitance(
    const NativeProbeBindings& bindings,const std::optional<NativeModelSpec>& declaration,
    const QJsonObject& owned,const NativeCapacitanceCorrection* requested)
{
    if(owned.isEmpty())
        return requested?std::optional<NativeCapacitanceCorrection>(*requested):std::nullopt;
    if(!declaration)throw std::logic_error("owned_capacitance_requires_model_declaration");
    ValidateAppliedNativeCapacitanceState(bindings,*declaration,owned);
    std::string revision;
    auto result=ParseNativeCapacitanceState(QJsonDocument(owned).toJson(QJsonDocument::Compact).toStdString(),revision);
    if(requested)
    {
        if(requested->baseline!=result.baseline)
            throw std::invalid_argument("owned_capacitance_baseline_cannot_be_rebased");
        for(const auto& [name,scale]:requested->membraneScales)
        {
            if(requested->dormantMembraneScales.count(name))
                throw std::invalid_argument("overlapping_active_dormant_capacitance_scales");
            result.dormantMembraneScales.erase(name);result.membraneScales[name]=scale;
        }
        for(const auto& [name,scale]:requested->dormantMembraneScales)
        {result.membraneScales.erase(name);result.dormantMembraneScales[name]=scale;}
        result.maximumRelativeChange=requested->maximumRelativeChange;
    }
    return result;
}
}
