#pragma once
#include "NativeRecognitionOwner.h"
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace NMSDK::TimeLearner {
// Shared recognition-control boundary for Classic and Branch. Physical graph
// construction, probe scheduling and topology-specific bindings stay with their
// existing owners. No state, nested Calculate or lifecycle call is added here.
struct NativeRecognitionControl {
    template<class Owner>
    static std::weak_ptr<const void> ObserveNativeRecognitionContext(const Owner& owner)
{
 if(owner.NativeRecognition)return owner.NativeRecognition->contextLifetime;
 return {};
}

    template<class Owner>
    static bool HasNativeRecognitionTrialIdentity(const Owner& owner,const std::string& runId,const std::string& trialId)
{
 return owner.NativeRecognition && owner.NativeRecognition->trials.HasIdentity(runId,trialId);
}

    template<class Owner>
    static bool IsNativeRecognitionTrialIdle(const Owner& owner)
{
 return owner.NativeRecognition && !owner.NativeRecognition->trials.Busy() && !owner.NativeRecognition->trials.HasResult();
}

    template<class Owner>
    static void QueueNativeRecognitionTrial(Owner& owner,NativeRecognitionTrialRequest request)
{
 if(owner.NativePostTunePhase && !owner.NativePostTunePhase->Done() && !owner.NativePostTunePhaseDepth)
  throw std::logic_error("native_owned_post_tune_owns_trials");

 if(!owner.NativeRecognition || !owner.NativeEditFailure.empty())throw std::logic_error("recognition_model_not_available");
 TimeLearner::CheckNativeRecognitionHold(owner,*owner.NativeRecognition,true);
 owner.NativeRecognition->trials.Queue(std::move(request));
}

    template<class Owner>
    static std::optional<NativeRecognitionTrialObservation> TakeNativeRecognitionTrialResult(Owner& owner)
{
 if(owner.NativePostTunePhase && !owner.NativePostTunePhase->Done() && !owner.NativePostTunePhaseDepth)
  throw std::logic_error("native_owned_post_tune_owns_trials");

 if(!owner.NativeRecognition)return {};
 return owner.NativeRecognition->trials.TakeResult();
}

    template<class Owner>
    static void CancelNativeRecognitionTrial(Owner& owner,const std::string& reason)
{
 if(owner.NativePostTunePhase && !owner.NativePostTunePhase->Done() && !owner.NativePostTunePhaseDepth)
  throw std::logic_error("native_owned_post_tune_owns_trials");

 if(owner.NativeRecognition)owner.NativeRecognition->trials.Cancel(reason);
}

    template<class Owner>
    static QJsonObject ExportNativeRecognitionState(Owner& owner,const std::string& implementationId)
{
 if(!owner.NativeRecognition)throw std::logic_error("native_recognition_state_not_installed");
 if(!owner.NativeEditFailure.empty())throw std::logic_error(owner.NativeEditFailure);
 if(owner.NativeRecognition->package["implementation_id"].toString().toStdString()!=implementationId)
  throw std::invalid_argument("recognition_implementation_mismatch");
 TimeLearner::CheckNativeRecognitionHold(owner,*owner.NativeRecognition,true);return owner.NativeRecognition->package;
}
};
}
