# NClassifierDynamicsRecorder

`NClassifierDynamicsRecorder` is an optional passive probe for classifier experiments. It samples connected scalar outputs and writes signed values to CSV with model time. It does not alter neuron state, classifiers, or their response semantics.

## Configuration

Add the component to the model and connect each source output to its `Signals` input. The order of links defines the order of CSV columns.

| Property | Meaning |
|---|---|
| `Enable` | Turns recording on; default is `false`. |
| `SignalNamesCsv` | Comma-separated column labels aligned with `Signals`; omitted labels become `signal_1`, etc. |
| `SavePath` | Directory relative to the project data directory; default `ClassifierDynamics`. |
| `FileName` | CSV file name; default `signals.csv`. |
| `SampleInterval` | Minimum interval between samples in model seconds; default `0.0005`. |
| `FlushInterval` | Model-time interval between explicit file flushes; default `0.01`. A non-positive value flushes each sample. |
| `AppendMode` | Appends to an existing file and preserves its header when `true`; otherwise reset starts a fresh file. |
| `LastError` | Last file-creation/open error, if any. |
| `SamplesWritten` | Number of rows written since the latest reset. |

The CSV begins with `model_time` followed by the configured alias and the **actual connected source path** in brackets, for example `class1_ltz [Model.Classifier.NeuronTrainer1.Neuron.LTZone.Output]`. The path comes from the live `Signals` connection and follows the same connected-output order used to read values, so it can be compared with the serialized links rather than inferred from hand-maintained labels. Each signal is read from matrix element `(0,0)` and is recorded without thresholding or taking its absolute value. Connect LTZone outputs for spike edges, membrane `SumPotential` outputs for signed potentials, and inhibitory synapse/channel outputs to inspect the full causal chain. `NPulseMembraneCommon::SumPotential` is exposed as an output for this diagnostic purpose; its calculation is unchanged.

Analyzer scripts should use the text before ` [` as the semantic alias and retain the bracketed path in their summary. A multi-signal CSV produced before source paths were added has no verified alias-to-source map; it must be rerun or checked from its links before interpreting individual columns.

## Example links

```xml
<elem Type="ULink">
  <Item Type="ULinkSide" Index="-1" Name="Output">Classifier.NeuronTrainer1.Neuron.LTZone</Item>
  <Connector Type="ULinkSide" Index="-1" Name="Signals">ClassifierDynamicsRecorder</Connector>
</elem>
<elem Type="ULink">
  <Item Type="ULinkSide" Index="-1" Name="Output">Classifier.NeuronTrainer1.Neuron.Soma1.SumPotential</Item>
  <Connector Type="ULinkSide" Index="-1" Name="Signals">ClassifierDynamicsRecorder</Connector>
</elem>
```

For a controlled classifier comparison, `NSpikeClassifier` and `NClassifier` expose `UseLateralInhibition`. It defaults to `true`; set it to `false` only in the no-inhibition control clone. This switch controls only inter-class inhibitory links and does not change the response bit contract.
