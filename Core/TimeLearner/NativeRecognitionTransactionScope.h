#pragma once
#include "NativeRecognitionState.h"
#include "NativeModelBuilder.h"
#include "NativeRecognitionTrial.h"
#include "../../../../Rdk/Core/Engine/UEnvironment.h"
#include <memory>
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
// Shared ownership/lifecycle guard for outer recognition controllers. No
// scheduling or scientific policy: children still run only in native ticks.
// There is deliberately no automatic rollback in this object's destructor.
template<class Owner> class NativeRecognitionTransactionScope {
public:
    NativeRecognitionTransactionScope(Owner& owner,std::string implementation,std::string diagnosticPrefix)
        :owner(owner),implementation(std::move(implementation)),prefix(std::move(diagnosticPrefix)),
         lifecycle(owner.ObserveLifecycle()),recognitionContext(owner.ObserveNativeRecognitionContext()),
         storage(owner.GetStorage().Get()),environment(owner.GetEnvironment().Get()),frequency(owner.GetTimeStep())
    {
        Require(environment!=nullptr,"context_changed");lastClock=environment->GetTime().GetDoubleTime();
    }
    NativeRecognitionTransactionScope(const NativeRecognitionTransactionScope&)=delete;
    NativeRecognitionTransactionScope& operator=(const NativeRecognitionTransactionScope&)=delete;
    void CheckLifecycle()
    {
        Require(lifecycle.IsCurrent(),"owner_lifecycle_changed");
        const auto context=owner.ObserveNativeRecognitionContext();
        Require(!recognitionContext.expired() && !recognitionContext.owner_before(context) &&
            !context.owner_before(recognitionContext),"recognition_context_replaced");
        Require(owner.GetStorage().Get()==storage && owner.GetEnvironment().Get()==environment &&
            owner.GetTimeStep()==frequency,"context_changed");
        const double now=environment->GetTime().GetDoubleTime();
        Require(Detail::Finite(now) && !Detail::After(lastClock,now),"clock_moved_backwards");lastClock=now;
    }
    void CheckPackage(const QJsonObject& expected)
    {
        CheckLifecycle();
        Require(owner.ExportNativeRecognitionState(implementation)==expected,"owner_changed");
        Require(owner.IsNativeRecognitionTrialIdle(),"foreign_or_unconsumed_trial");
    }
    void CheckTrialOwnership(const std::string& runId,const std::string& trialId) const
    {Require(owner.HasNativeRecognitionTrialIdentity(runId,trialId),"trial_ownership_changed");}
    void Install(const QJsonObject& package,QJsonObject& expected)
    {
        CheckPackage(expected);
        const auto result=owner.ImportNativeRecognitionState(package,implementation);
        Require(result.status==NativeModelBuildStatus::Adopted,"native_import_failed");
        // bool returns are not error channels. Read back the whole package.
        owner.Build();owner.Init();owner.Reset();lifecycle=owner.ObserveLifecycle();
        recognitionContext=owner.ObserveNativeRecognitionContext();expected=package;CheckPackage(expected);
    }
    void CheckRollback(const QJsonObject& expected)
    {
        CheckLifecycle();
        Require(owner.ExportNativeRecognitionState(implementation)==expected,"refuses_foreign_rollback");
    }
    NativeRecognitionTrialObservation CancelOwned(const NativeRecognitionTrialRequest& request,const std::string& reason)
    {
        CheckTrialOwnership(request.runId,request.trialId);owner.CancelNativeRecognitionTrial(reason);
        auto result=owner.TakeNativeRecognitionTrialResult();
        Require(result && result->request.runId==request.runId && result->request.trialId==request.trialId,"cancelled_trial_changed");
        return std::move(*result);
    }
private:
    Owner& owner;std::string implementation,prefix;
    RDK::UModuleLifecycleObservation lifecycle;
    std::weak_ptr<const void> recognitionContext;
    RDK::UStorage* storage=nullptr;RDK::UEnvironment* environment=nullptr;
    unsigned frequency=0;double lastClock=0.;
    void Require(bool value,const char* why) const
    {if(!value)throw std::logic_error(prefix+"_"+why);}
};
}
