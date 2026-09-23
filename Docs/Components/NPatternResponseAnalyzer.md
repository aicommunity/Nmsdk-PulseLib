# NPatternResponseAnalyzer — измерение ответов на паттерны

## RU

`NPatternResponseAnalyzer` регистрирует rising-edge стимулов и выхода нейрона. Wiring: `StimulusInputs←DatasetMatrix.Generator1.Output`, `NeuronOutputs←Neuron.LTZone.Output`, `TargetClassInput←DatasetMatrix.CurrentClass`.

### Фактическая реализация (после A01–A04)

Analyzer берёт expected stim count / class / sample id из soft-linked `DatasetMatrix` (через `LearnerComponentName`). Паттерн полон при `observed >= expected`; без dataset — legacy fallback `1 || >=4`.

In-window `neuron_fired` только после complete и в `PostPatternWindow`. Наблюдение для всех trial длится до `last_stim + LateResponseWindow` (`trial_observe_until_`). Метка класса фиксируется в `BeginTrial` и не перечитывается. Coincident stim/neuron: Close → Begin → attribute once.

`response_class` per_stim — injective (один spike ≤ одно окно). Python metrics v2 считают любой foil-spike как FP.

### CSV и метрики

Поля: `trial,target_class,stim_count,isi0..isi3,neuron_fired,neuron_t_rel,match,late_fired,late_t_rel,error_class,ltz_potential_max,soma_amp_0..3,soma_amp_sum,neuron_spike_count,neuron_spike_times,response_class`.

- `match` учитывает только in-window `neuron_fired`; late в нём не учтён.
- `neuron_spike_count` считает rising-edge, пока trial открыт; список `neuron_spike_times` хранит до 16 времён относительно первого стимула. После закрытия оставшаяся часть late-окна уже не наблюдается для этого trial.
- `response_class`: single/burst/per_stim/multi. Текущий per_stim допускает повторное использование одного спайка для нескольких пересекающихся окон.
- Python `ok_audit` не равен идеальной избирательности. Strict использует fire/late-флаги и может пропустить ранний одиночный ответ foil, хотя spike_count>0.

Нужный критерий эксперимента: один target-спайк после реального последнего стимула, отсутствие любых foil-спайков за одинаковый полный интервал наблюдения. Текущий код этот критерий не гарантирует. [Доказательства и план исправления](../../../../Docs/Audit/TimeLearner-2026-09-22/README.md).

### Свойства

| Имя | Смысл |
|-----|-------|
| PostPatternWindow / LateResponseWindow | Окна в секундах |
| PulseDetectThreshold | Порог rising-edge, default 0 |
| SavePath / FileName | Путь CSV относительно data dir |
| AppendMode | false: пересоздание CSV при Reset |
| Enable | Включение анализа |
| TrialIndex, LastFired, LastMatch, LastLateFired, LastErrorClass | Результат последнего закрытого trial |
| LastNeuronDelay, LastLateNeuronDelay, LastIsi, LastSpikeCount, LastResponseClass | Времена, ISI и морфология последнего trial |

## EN

The current implementation uses observed stimulus count (1 or >=4) as a completion heuristic, closes timely trials before the late window ends, and reads a mutable class label. CSV fire flags and Python audit passes therefore do not guarantee a single response after the actual final stimulus or silence on every foil. See the linked audit before treating these measurements as validated selectivity.
