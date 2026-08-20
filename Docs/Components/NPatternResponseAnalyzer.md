# NPatternResponseAnalyzer — избирательность паттерна

## RU

### Назначение

**Класс**: `NPatternResponseAnalyzer` — сопоставляет стимульный паттерн (rising-edge на входах генератора) с ответом нейрона (LTZone) и меткой класса сэмпла. Записывает trial-строки в CSV.

### Wiring (TimeNeuronTimeLearnerTest)

- `StimulusInputs` ← `DatasetMatrix.Generator1.Output`
- `NeuronOutputs` ← `Neuron.LTZone.Output`
- `TargetClassInput` ← `DatasetMatrix.CurrentClass`

### Окна детекта

1. **`PostPatternWindow`** (default 0.5 с после последнего стимула) — «вовремя». Только эти спайки идут в `neuron_fired` / `match`. Если в окне был fire — trial закрывается сразу (late не смотрим).
2. **`LateResponseWindow`** (default 1.5 с после последнего стимула, ≥ PostPatternWindow) — если в окне тишина, продолжаем наблюдение. Первый late rising edge → `late_fired=1` и закрытие; иначе закрытие по концу LateResponseWindow или при старте следующего паттерна.
3. Доп. стимулы **внутри** открытого PostPatternWindow дописываются в тот же trial (мульти-импульсный паттерн).

### CSV

```text
trial,target_class,stim_count,isi0,isi1,isi2,isi3,
neuron_fired,neuron_t_rel,match,
late_fired,late_t_rel,error_class,
ltz_potential_max,soma_amp_0,soma_amp_1,soma_amp_2,soma_amp_3,soma_amp_sum
```

- `match` — только по in-window (`neuron_fired`): target→нужен fire, nontarget→тишина.
- `error_class`:
  - `ok` — верно по in-window
  - `fn` / `fp` — промах / ложное в окне
  - `late_fn` — target: в окне тишина, но был поздний спайк
  - `late_fp` — nontarget: в окне тишина, но был поздний спайк

### Свойства

| Имя | Смысл |
|-----|--------|
| `PostPatternWindow` | Окно (сек) после последнего стимула для `neuron_fired` / `match` |
| `LateResponseWindow` | Окно (сек) после последнего стимула для `late_fired` (≥ Post) |
| `PulseDetectThreshold` | Порог rising-edge (default 0) |
| `SavePath` / `FileName` | Каталог и файл CSV относительно data dir проекта |
| `AppendMode` | false — пересоздать CSV на Reset |
| `LastFired`, `LastMatch`, `LastLateFired`, `LastErrorClass`, `LastNeuronDelay`, `LastLateNeuronDelay`, `LastIsi` | Состояние последнего закрытого trial |

### Favorites

`StimulusInputs`, `NeuronOutputs`, `TargetClassInput`, `PostPatternWindow`, `LateResponseWindow`, `SavePath`, `FileName`, `Enable`, `TrialIndex`, `LastFired`, `LastMatch`, `LastLateFired`, `LastErrorClass`.
