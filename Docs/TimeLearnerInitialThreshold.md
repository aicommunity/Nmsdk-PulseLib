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
