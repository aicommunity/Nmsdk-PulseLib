# Explicit construction threshold

NativeModelSpec schema 1 retains the effective storage image threshold.
Schema 2 requires finite `initial_threshold`, including an explicitly chosen
zero or negative scalar threshold. Boolean, missing, unknown or nonfinite
fields are rejected by the strict native codec.

The constructor applies this value once to the private neuron's actual LTZone,
after structural construction and before Init, model identification, attachment
and any reference measurement. Only supported scalar spike-threshold LTZone
implementations are accepted. Readback after Init must match. No shared image
is changed and no nested Calculate or tick-time repair is introduced.

This is a construction parameter, not a permanent constraint. Inspection,
Reset, growth and model/archive import do not reapply it. Actual recognition
thresholds and calibration provenance remain in the exported graph and the
separate calibration state. A training certificate still belongs to its exact
measured model revision and cannot certify a changed recognition graph.

The immediate use is the original no-LT NSPNeuronGen training protocol with
threshold 100. Its storage body physics remains unchanged; using the stock
recognition threshold 0.0117 during native Train is a different protocol.
The implementation requires server native/codec/ownership/transport checks
before acceptance. No successful training or selectivity is asserted here.


Acceptance, 2026-11-04: code541bf89b9966f63ca5131fa57a31a9ae3f5820ea passed
18 native/codec/storage/archive controls on Elder and M40 ASan. Root source
63d861e823aea132625b22f3702ac11c3d1e7320, evidence
60f5905c7b04ac41f16d405d2fe715f1e3eed174 and
cd3a477ee03f5f75b2d71af83772ec6dce5dda1e in the root repository. Four physical
roundtrips cover Classic/Branch at2000/20000Hz, stock physics without
LTMembrane, construction threshold100 and a later effective instance value0.005.
Actual input/soma/spike samples are equal after transfer; Reset and import do
not reapply the construction threshold. Shared prototype0.0117 is preserved.

The associated four positive calibration fixtures formerly failed before
calibration under an insufficient search budget. They now explicitly select
the existing engineering LT storage profile and production calibration seed;
four separate legacy negative controls retain their original stopped status.
Rootb82f76e2fe543acf25d6c3076abbd864f965b3a8 passed8/8 native cases on both
workers, with7/7 primary readers each. Evidence3824e7eb2068812c94af33ef2775a7c25f8556f9
and ec7934bfd4a838b397298f9cf8ad18774b28e0f9. Four sources obtain real Exact
certificates before calibration, and two schema2 cases import into a distinct
owner. No certificate is fabricated and no tolerance is relaxed. ASan mappings
are captured with detect_leaks=0; this does not attest absence of leaks.

This accepts the optional construction-threshold contract and those addressed
fixtures. It does not establish successful classification of the stock
four-input model, support for every LTZone implementation, the entire mode/
physics matrix, or integration with other pending algorithm branches. Complete
provenance and limitations are in root Docs/StructTrain/TimeLearner/
InitialThresholdConstruction.ru.md and CalibrationFixturePreconditions.ru.md.
