#pragma once
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include <stdexcept>

namespace NmsdkTools {
// Rdk lifecycle bools are not error channels. Caught component exceptions are
// exposed by the operation ledger, including failures forwarded from children.
inline void RequireNativeOperationsUnchanged(const RDK::UModule& module,std::uint64_t marker)
{
    if(!module.HasOperationFailureSince(marker))return;
    const auto failure=module.LastOperationFailure();
    throw std::runtime_error("native_operation_failure:"+failure.Operation+":"+failure.Reason);
}
inline void InitializeNative(RDK::UEPtr<RDK::UContainer> node)
{
    if(!node)throw std::runtime_error("missing_native_operation_target");
    const auto life=node->ObserveLifecycle();const auto marker=node->OperationFailureSequence();
    node->Init();
    if(!node || !life.IsAlive())throw std::runtime_error("native_init_destroyed_operation_target");
    RequireNativeOperationsUnchanged(*node,marker);
}
inline void InitializeAndResetNative(RDK::UEPtr<RDK::UContainer> node)
{
    InitializeNative(node);
    const auto life=node->ObserveLifecycle();const auto marker=node->OperationFailureSequence();
    node->Reset();
    if(!node || !life.IsAlive())throw std::runtime_error("native_reset_destroyed_operation_target");
    RequireNativeOperationsUnchanged(*node,marker);
}
}
