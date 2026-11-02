# Bounded search termination

Feature implementation, pending server/native/export acceptance.
The measured objective, tolerances, candidate enumeration/ranking and native
lifecycle are unchanged. A fresh final pass satisfying all original tolerances
still ends Exact, including the last allowed sweep. An unsuccessful final pass
with no adjustable input remains SearchUnresolved. Otherwise maximumSweeps
preventing a next required sweep ends BudgetExhausted with
maximum_sweeps_exhausted. Missing/invalid observations retain their existing
bounded recovery and InvalidObservation outcome.

BoundedTrainingSession exposes a terminal Reason. Probe, edit, epoch and
adapter-execution limits have distinct reasons; NativeTrainingService propagates
them to its existing summary/package diagnostic fields. No public Rdk bool
return or component parameter is repurposed. Historical artifacts keep their
recorded source versions and outcomes.

The pure session retains the fresh negative FinalVerification certificate as
measurement evidence. The native service does not label BudgetExhausted as a
completed learned boundary: its existing stopped-state export preserves model,
references, final and observed packets without manufacturing convergence.
An explicit diagnostic evaluation path for stopped models remains a required
part of the full refactor; this feature must not silently route them through
completed-Train recognition. Existing completed-state archives retain their
historical import contract.

Actual ASan baseline on Pulse a18bf543 reproduced the four Classic/Branch N0/N1
status mismatches. A first missing-peak fixture expectation was corrected,
separating InvalidObservation from a genuine no-adjustable-input control.
Corrected seven-case baseline: four expected failures and three controls passed.
Full original stages 5-8, quantized/best-effort semantics and other search limits
are not declared accepted by this correction.
