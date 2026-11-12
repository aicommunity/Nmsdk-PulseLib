# NPatternResponseAnalyzer — избирательность паттерна

## RU

### Назначение

**Класс**: `NPatternResponseAnalyzer` — сопоставляет стимульный паттерн (rising-edge на входах генератора) с ответом нейрона (LTZone) и меткой класса сэмпла. Записывает trial-строки в CSV.

### Wiring (TimeNeuronTimeLearnerTest)

- `StimulusInputs` ← `DatasetMatrix.Generator1.Output`
- `NeuronOutputs` ← `Neuron.LTZone.Output`
- `TargetClassInput` ← `DatasetMatrix.CurrentClass`

### CSV

```text
trial,target_class,stim_count,isi0,isi1,isi2,isi3,neuron_fired,neuron_t_rel,match
```

- `target_class=1` — ожидается спайк (`match=1` если `neuron_fired=1`).
- `target_class=0` — ожидается тишина (`match=1` если `neuron_fired=0`).

### Свойства

| Имя | Смысл |
|-----|--------|
| `PostPatternWindow` | Окно (сек) после последнего стимульного спайка для детекта ответа и закрытия trial |
| `PulseDetectThreshold` | Порог rising-edge (default 0) |
| `SavePath` / `FileName` | Каталог и файл CSV относительно data dir проекта |
| `AppendMode` | false — пересоздать CSV на Reset |
| `LastFired`, `LastMatch`, `LastNeuronDelay`, `LastIsi` | Состояние последнего закрытого trial |

### Favorites

`StimulusInputs`, `NeuronOutputs`, `TargetClassInput`, `PostPatternWindow`, `SavePath`, `FileName`, `Enable`, `TrialIndex`, `LastFired`, `LastMatch`.
