# Native execution scheduling capability

The verified native protocol requires uniform single-rate physical/stimulus
subtrees and no per-component wall-driven child skipping. Nonnegative inherited
MaxCalculationDuration is therefore rejected with
`unsupported_native_wall_limited_schedule` on the owner/ancestors and required
Neuron/DatasetMatrix subtrees. Every negative value remains disabled. This is
a capability policy even if RDK_ENABLE_CALC_TIME_CHECKS was compiled out; a
non-acting wall limit must not masquerade as an effective experimental factor.

The shared read-only NativeCalculationPolicy helper never changes properties,
calculates a component, resets an owner, or repairs a profile with default -1.
Rdk behavior and lifecycle bools are unchanged. Legacy calculation remains
available; explicit native API operations follow this verified protocol even
when invoked on a class loaded with its legacy default ControllerVersion.

Checks run at native probe/recognition preparation and live boundaries. Probe
callbacks are checked again before successful closure, including the final
sample. An explicit cancellation/reset from the callback retains its outcome.
A partial or complete invalid observation remains diagnostic evidence.

NativeTrainingAdapter.validateExecution is populated by BindNativeTraining;
the service invokes it before Start/actions. Pure controller adapters may omit
it and do not attest any Rdk graph. Revision reads deliberately remain usable
under an invalid execution policy so failed observations can close and physical
checkpoints can be restored. Generic Build/import/export/standalone model edits
and rollback do not gain this execution restriction. Owned finalization and
the production Console preflight validate scheduling before execution.

Implementation/62 declared native fixture cases are pending server acceptance.
The two-case Elder baseline accepted both large positive limits without policy
diagnostics; this proves a capability-validation gap, not actual wall skips.
Full lifecycle/storage/physics matrices remain open; root documentation contains
exact revisions, worker manifests, baseline outcomes and acceptance reports.
