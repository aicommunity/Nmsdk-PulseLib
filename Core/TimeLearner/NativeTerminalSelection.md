# Native terminal measured-state selection — implementation boundary

This is an explicit `NativeTrainingPlan::terminalSelection` capability, currently
for native validation. Existing cold JSON configurations do not enable it.
It is not yet a complete experimental release: versioned portable export,
import and replay of the transition remain required.

After a budget stop with a retained controller-selected measured boundary, the
service enters `Finalizing`. The old actor's control lifetime is revoked. The
next ordinary before-children call grants one short-lived permit, validates the
actual terminal model against the stopped search and reserves two transactions
from the remaining declared edit allowance: one commit and one possible rollback.
No probes, iterations, model time or measurement budgets are replenished.

The adapter verifies the original owner lifetime, target/configuration, image
closure and quiet physical boundary. It builds private children through native
snapshot import, checks the permit and finalization wall allowance immediately
before attachment, then restores graph, Dataset, computing order, owner mirrors
and owned C-state. It reads back complete semantic identities. No nested
Calculate, shared prototype mutation, parent-C fallback or mid-probe checkpoint
is used. A bounded rollback retains the pre-transaction model on a rejected
candidate; failure to verify rollback remains an invalid model.

External stop or explicit Reset before the transaction does not trigger a
restore. A successful restore preserves BudgetExhausted and has no certificate.
Search history keeps its original epoch; the physical epoch advances by the
charged native transactions. Old observations are never rebound to the new
epoch. The event records source/selected/actual identities, charges and outcome.

Until the portable transition format is implemented, old learned-state,
summary and boundary exporters reject this explicit capability. Reset/warm
control alone cannot remove that requirement. Explicit cold retarget starts a
new evidence scope. Returning an old-format package describing the restored
graph as an ordinary terminal search state would be incorrect.

Native integration cases cover both topologies and N0/N1 with a real wall
deadline after a real unmeasured edit, response replay, owned C-state, external
stop, Reset, insufficient edit reservation and failed attachment. These cases
are pending server validation. Passing them will not finish the portable
export/replay requirement or the full TimeLearner refactoring plan.
