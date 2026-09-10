# NPatternResponseAnalyzer — избирательность паттерна

## RU

### Назначение

**Класс**: `NPatternResponseAnalyzer` — сопоставляет стимульный паттерн (rising-edge на входах генератора) с ответом нейрона (LTZone) и меткой класса сэмпла. Записывает trial-строки в CSV.

### Wiring (`TimeNeuronTimeLearner/Test`)

- `StimulusInputs` ← `DatasetMatrix.Generator1.Output`
- `NeuronOutputs` ← `Neuron.LTZone.Output`
- `TargetClassInput` ← `DatasetMatrix.CurrentClass`

### Окна детекта

1. **`PostPatternWindow`** (default 0.5 с после последнего стимула) — «вовремя». Первый rising-edge в этом окне задаёт `neuron_fired` / `match`. Trial **не** закрывается на первом спайке: окно додерживается до конца Post, чтобы посчитать все спайки.
2. **`LateResponseWindow`** (default 1.5 с после последнего стимула, ≥ PostPatternWindow) — если in-window тишина, наблюдаем дальше до конца Late (или старта следующего паттерна). Первый late edge → `late_fired=1`; последующие late edge тоже учитываются в `neuron_spike_count`.
3. Доп. стимулы **внутри** открытого PostPatternWindow дописываются в тот же trial (мульти-импульсный паттерн).

### CSV

```text
trial,target_class,stim_count,isi0,isi1,isi2,isi3,
neuron_fired,neuron_t_rel,match,
late_fired,late_t_rel,error_class,
ltz_potential_max,soma_amp_0,soma_amp_1,soma_amp_2,soma_amp_3,soma_amp_sum,
neuron_spike_count,neuron_spike_times,response_class
```

- `match` — только по in-window (`neuron_fired`): target→нужен fire, nontarget→тишина. **Не** учитывает `late_*` (legacy).
- `error_class`: `ok` / `fn` / `fp` / `late_fn` / `late_fp`
- `neuron_spike_count` / `neuron_spike_times` — все rising-edge за trial (относительные к первому стимулу, `;`-разделитель, до 16)
- `response_class`: `single` | `burst` | `per_stim` | `multi`

**Audit gate:** success требует single-spike (`response_class=single` при fire) и strict-учёт late как FA/FN. Пачка / ответ на каждый стимул паттерна = брак параметров модели или гиперпараметров (L, синапсы), не «частичный успех».

### Свойства

| Имя | Смысл |
|-----|--------|
| `PostPatternWindow` | Окно (сек) после последнего стимула для `neuron_fired` / `match` |
| `LateResponseWindow` | Окно (сек) после последнего стимула для late (≥ Post) |
| `PulseDetectThreshold` | Порог rising-edge (default 0) |
| `SavePath` / `FileName` | Каталог и файл CSV относительно data dir проекта |
| `AppendMode` | false — пересоздать CSV на Reset |
| `LastFired`, `LastMatch`, `LastLateFired`, `LastErrorClass`, `LastNeuronDelay`, `LastLateNeuronDelay`, `LastIsi`, `LastSpikeCount`, `LastResponseClass` | Состояние последнего закрытого trial |

### Favorites

`StimulusInputs`, `NeuronOutputs`, `TargetClassInput`, `PostPatternWindow`, `LateResponseWindow`, `SavePath`, `FileName`, `Enable`, `TrialIndex`, `LastFired`, `LastMatch`, `LastLateFired`, `LastErrorClass`, `LastSpikeCount`, `LastResponseClass`.
