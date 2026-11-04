#pragma once
#include "ConfiguredNativeTraining.h"
#include "LegacyReadout.h"
#include "NativeLiveFinalization.h"
#include "NativeOwnedPostTunePhase.h"
#include "NativeRecognitionOwner.h"
#include "NativeTrainingBindings.h"
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
// Rdk still invokes the original virtual hooks and schedules all children.
// This dispatch neither calculates components nor changes training decisions.
// Integration failure codes are topology-specific; keep their predicate at
// the call site. Base hooks likewise remain qualified in each concrete class.
struct NativeLearnerLifecycle {
    template<class Owner, class IntegrationFailure>
    static bool BeforeCalculate(Owner& owner, IntegrationFailure integrationFailure)
{
 if(owner.NativeLiveFinalization)
 {
  const auto phase=owner.NativeLiveFinalization;
  TimeLearner::NativeLifecycleScope scope(owner.NativeLiveFinalizationDepth);
  phase->BeforeTick(owner,owner.Environment->GetTime().GetDoubleTime());
 }

 if(owner.NativePostTunePhase && !owner.NativePostTunePhase->Done())
 {
  const auto phase=owner.NativePostTunePhase;
  TimeLearner::NativeLifecycleScope scope(owner.NativePostTunePhaseDepth);
  phase->BeforeTick(owner.Environment->GetTime().GetDoubleTime());
  owner.FinishNativePostTunePhase();
 }
 if(owner.NativeLiveFinalization)
 {
  owner.NativeLiveFinalization->ObservePhase(owner);
  if(owner.NativeLiveFinalization->BlocksCalculation()){owner.ForceSkipComponentCalculation();return true;}
  if(owner.NativeLiveFinalization->CancelledBeforeEntry())return true;
 }

 if(owner.NativeRecognition)
 {
  try
  {
   if(!owner.NativeEditFailure.empty())throw std::runtime_error(owner.NativeEditFailure);
   TimeLearner::CheckNativeRecognitionHold(owner,*owner.NativeRecognition);
   TimeLearner::BeforeNativeRecognitionTrial(owner,*owner.NativeRecognition,owner.NativeProbeModelBindings(),owner.Environment->GetTime().GetDoubleTime());
  }
  catch(const std::exception& error){owner.NativeEditFailure=error.what();owner.NativeRecognition->trials.Cancel(error.what());owner.ForceSkipComponentCalculation();}
  return true;
 }
 if(owner.ControllerVersion.GetData()==2 && owner.NativeEditFailure.empty() && owner.IsNeedToTrain.GetData() &&
    owner.NativeTrainingControl.State()==TimeLearner::NativeTrainingState::Idle &&
    (!owner.NativeProbeControl.OwnsControl() || owner.NativeProbeControl.AtEditBoundary()))
 {
  try
  {
   if(!owner.NativeConfiguredPlan || owner.NativeConfiguredText!=owner.ControllerConfiguration.GetData())
    throw std::invalid_argument("configured_native_model_requires_build");
   auto plan=TimeLearner::MakeConfiguredNativeTrainingPlan(*owner.NativeConfiguredPlan);
   if(plan.recordCapacitanceHistory)owner.BindNativeTrainingCapacitance(owner.NativeConfiguredPlan->capacitanceMaximumRelativeChange);
   plan.probe.onPrepared=owner.NativeConfiguredSinks.onPrepared;plan.probe.onSample=owner.NativeConfiguredSinks.onSample;
   plan.observationSink=owner.NativeConfiguredSinks.onObservation;owner.BeginNativeTraining(std::move(plan));
  }
  catch(const std::exception& error)
  {owner.NativeTrainingOwned=true;owner.NativeEditFailure=error.what();owner.NativeTrainingControl.FailConfiguration(error.what());}
 }

 if(!owner.NativeEditFailure.empty()){owner.ForceSkipComponentCalculation();return true;}
 if(owner.NativeTrainingOwned)
 {
  const auto adapter=TimeLearner::BindNativeTraining(owner,owner.NativeProbeControl,owner.NativeTrainingEditDepth);
  owner.NativeTrainingControl.BeforeTick(adapter,owner.Environment->GetTime().GetDoubleTime(),TimeLearner::NativeTrainingWallTime());
  if(owner.NativeProbeControl.OwnsControl())
   owner.NativeProbeControl.BeforeChildren(owner.NativeProbeModelBindings(),owner.Environment->GetTime().GetDoubleTime());
  if(!owner.NativeEditFailure.empty() || owner.NativeProbeControl.Status()==TimeLearner::NativeProbeStatus::Rejected ||
      owner.NativeProbeControl.Status()==TimeLearner::NativeProbeStatus::Cancelled ||
      (owner.NativeTrainingControl.State()==TimeLearner::NativeTrainingState::Finished &&
       owner.NativeTrainingControl.Outcome()==TimeLearner::TrainingStatus::InvalidModel &&
       !owner.NativeProbeControl.OwnsControl()))owner.ForceSkipComponentCalculation();
  return true;
 }
 if(owner.NativeProbeControl.OwnsControl())
 {
  owner.NativeProbeControl.BeforeChildren(owner.NativeProbeModelBindings(),owner.Environment->GetTime().GetDoubleTime());
  if(integrationFailure(owner.TrainingFailureReason.GetData()) ||
      owner.NativeProbeControl.Status()==TimeLearner::NativeProbeStatus::Rejected ||
      owner.NativeProbeControl.Status()==TimeLearner::NativeProbeStatus::Cancelled)owner.ForceSkipComponentCalculation();
  return true;
 }
 if(!TimeLearner::LegacyReadoutsReady(owner))
 {
  owner.NativeEditFailure=TimeLearner::LegacyReadoutShapeFailure;
  owner.ForceSkipComponentCalculation();return true;
 }
 if(integrationFailure(owner.TrainingFailureReason.GetData()))
  owner.ForceSkipComponentCalculation();
 return true;
}

    template<class Owner, class BaseHook>
    static bool BeforeDefault(Owner& owner, BaseHook baseHook)
{
 owner.InterruptNativePostTunePhase("learner_default");owner.NativePostTunePhase.reset();owner.NativeLiveFinalization.reset();
 owner.NativeRecognition.reset();
 owner.NativeConfiguredPlan.reset();owner.NativeConfiguredSinks={};owner.NativeConfiguredText.clear();
 owner.NativeModelDeclaration.reset();owner.NativeLearnedCapacitanceState={};
 owner.NativeTrainingRequestIntent.Clear();
 owner.NativeTrainingOwned=false;owner.NativeTrainingControl.InvalidateTarget();
 owner.NativeEditFailure.clear();owner.NativeProbeControl.ResetControl();baseHook();return true;
}

    template<class Owner, class BaseHook>
    static bool BeforeReset(Owner& owner, BaseHook baseHook)
{
 owner.InterruptNativePostTunePhase("learner_reset");
 if(!owner.NativeLifecycleDepth)owner.NativeTrainingRequestIntent.Clear();
 if(owner.NativeTrainingOwned)
 {
  if(!owner.NativeLifecycleDepth)owner.NativeTrainingControl.ResetControl();
  else if(TimeLearner::NativeTrainingIsActive(owner.NativeTrainingControl))owner.NativeTrainingControl.FailConfiguration("learner_implicit_reset");
 }
 if(!owner.NativeLifecycleDepth)owner.NativeProbeControl.ResetControl();
 else owner.NativeProbeControl.Cancel("learner_implicit_reset");
 if(!owner.NativeTrainingOwned && !owner.NativeRecognition && owner.ControllerVersion.GetData()!=2 &&
    !owner.ConfiguredModelInitialized.GetData())
 {
  owner.BindLegacyGraphRoles();
  TimeLearner::InitializeLegacyReadouts(owner,owner.NativeEditFailure);
 }
 baseHook();return true;
}

};
}
