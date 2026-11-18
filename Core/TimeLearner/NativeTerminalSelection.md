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
passed14/14 independently on Elder and as part of44/44 on M40 at
Root3c5bfe01609a6e6bd290aa9697938e885d5904b6 /
Pulse1d8fc24cdb19b4b0e399483d198314b84548f9ea. Workers and whole child groups
are closed; both primary archives are externally published on M40. This does
not finish portable export/replay or the full TimeLearner refactoring plan.
Later source-retention/assertion and historical-export changes are not covered
by those accepted gates.

The earlier snapshot-only M40 gate ran all30 cases:26 passed and four ordinary
empty-C snapshots failed their independent C-state equality. Non-const JSON
lookup had inserted a null baseline while collecting image dependencies.
Capture now uses value() lookup, preserving absent C-state. The original
failures remain evidence; all six snapshot cases passed in the new M4044 gate.

Terminal finalization also retains the physical SOURCE immediately before the
transaction, separately from the selected measured destination. Full native
neuron/Dataset snapshots and owned C-state are retained as immutable DATA;
the public source view exposes no Restore method or permit. Capture is bounded
to64MiB, uses the already declared finalization wall allowance, and precedes
edit-budget admission. Retention exhaustion stops without a physical edit and
keeps BudgetExhausted. Source identity/C-state are linked into the event.
Reset and a new control session release this transient source boundary.

This is the foundation for exporting historical search against its original
graph while separately exporting the restored actual graph. The portable
factory and cold/runtime schema are still required; the existing exporters
continue rejecting the explicit terminal scope. Additional integration
assertions compare the source snapshot with independent pre-transition
readbacks, including rollback and insufficient-edit cases. Those assertions
are not present in the already running Root3c5/Pulse1d8 jobs.

Historical export plumbing now has a read-only, factory-created context. The
existing protocol3, composite4 and candidate-window5 exporters can validate a
private historical graph and use the stopped transcript epoch. They continue
their original native/proof/C0/image checks; primary records are referenced
from the original service rather than copying the whole controller or its raw
sample journal. Ordinary exports still reject terminal scopes. The versioned
factory, import/replay, cold/runtime declarations and experimental pipeline are
not implemented by this plumbing; no public configuration enables it yet.
