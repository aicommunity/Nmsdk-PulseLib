#pragma once
#include "NativeTrainingService.h"
#include "ModelIdentity.h"
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
// Declaration of a live training attempt, separate from a later physical
// finalization boundary. No component references, model snapshot or callbacks.
class NativeTrainingIntent {
public:
    void Clear() { document={}; }
    QJsonObject Read() const {return document;}
    void Capture(const QJsonObject& controls,const NativeTrainingService& service,double modelTime)
    {
        if(!document.isEmpty() || service.State()==NativeTrainingState::Idle ||
            !std::isfinite(modelTime) || controls.isEmpty())
            throw std::logic_error("invalid_native_training_intent_capture");
        const auto& plan=service.Plan();
        document={{"kind","native_training_request_intent"},{"schema_version",1},
            {"controls",controls},{"run_id",QString::fromStdString(plan.search.runId)},
            {"session_attempt",double(service.SessionAttempt())},{"model_epoch",double(service.ModelEpoch())},
            {"initial_model_revision",QString::fromStdString(plan.search.initialModelRevision)},
            {"protocol_revision",QString::fromStdString(plan.protocol.revision)},
            {"pattern_hash",QString::fromStdString(plan.protocol.patternHash)},
            {"started_model_time",modelTime},{"dynamic_checkpoint",false},
            {"calibrated",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
        document["sha256"]=NmsdkTools::CanonicalModelDataSha256(document);
    }
private:
    QJsonObject document;
};
}
