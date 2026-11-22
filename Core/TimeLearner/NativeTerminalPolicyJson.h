#pragma once
#include "NativeTerminalSelection.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
// One wire representation for configured policy and terminal DATA. Neither
// decoding a declaration nor importing DATA creates a live restore permit.
inline QJsonObject EncodeNativeTerminalSelectionPolicy(const NativeTerminalSelectionPolicy& policy)
{
    ValidateNativeTerminalSelectionPolicy(policy);
    return {{"schema_version",1},{"maximum_wall_seconds",policy.maximumWallSeconds}};
}
inline NativeTerminalSelectionPolicy DecodeNativeTerminalSelectionPolicy(const QJsonObject& value)
{
    if(value.size()!=2 || !value.contains("schema_version") || !value.contains("maximum_wall_seconds"))
        throw std::invalid_argument("native_terminal_policy_fields");
    const auto version=value.value("schema_version"),seconds=value.value("maximum_wall_seconds");
    if(!version.isDouble() || version.toDouble()!=1. || !seconds.isDouble())
        throw std::invalid_argument("native_terminal_policy_schema_or_number");
    NativeTerminalSelectionPolicy result{seconds.toDouble()};ValidateNativeTerminalSelectionPolicy(result);return result;
}
}
