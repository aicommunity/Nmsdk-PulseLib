#pragma once
#include "NativeTrainingIntent.h"

namespace NMSDK::TimeLearner {
inline void CheckNativeTrainingIntent(const QJsonObject& request,const QJsonObject& controls,
    const NativeTrainingService& service)
{
    if(request.isEmpty())return; // Imported historical Solo has no invented request.
    auto unsignedRequest=request;unsignedRequest.remove("sha256");
    if(request["kind"]!=QJsonValue("native_training_request_intent") || request["schema_version"]!=QJsonValue(1) ||
        request["sha256"]!=QJsonValue(NmsdkTools::CanonicalModelDataSha256(unsignedRequest)))
        throw std::logic_error("native_training_request_invalid");
    const auto& plan=service.Plan();
    if(request["session_attempt"]!=QJsonValue(double(service.SessionAttempt())) ||
        request["run_id"].toString()!=QString::fromStdString(plan.search.runId) ||
        request["protocol_revision"].toString()!=QString::fromStdString(plan.protocol.revision) ||
        request["pattern_hash"].toString()!=QString::fromStdString(plan.protocol.patternHash))
        throw std::logic_error("native_training_request_session_changed");
    auto expected=request["controls"].toObject()["parameters"].toObject();
    auto actual=controls["parameters"].toObject();
    // Need is a live control signal: automatic local completion and external
    // stop may lower it. Their outcome/entry rules remain separately enforced.
    // Its source bindings, and every other parameter, must remain unchanged.
    expected.remove("IsNeedToTrain");actual.remove("IsNeedToTrain");
    if(expected!=actual || request["controls"].toObject()["bindings"]!=controls["bindings"])
        throw std::logic_error("native_training_request_controls_changed");
}
}
