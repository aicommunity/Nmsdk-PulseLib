# NPatternResponseAnalyzer — измерение ответов на паттерны

Сверено с PulseLib d4149190, 2026-09-24. [Повторный аудит и ограничения](../../../../Docs/Audit/TimeLearner-2026-09-24-review/README.md).

## RU

Analyzer регистрирует rising-edge стимулов и выхода нейрона. Wiring: StimulusInputs←DatasetMatrix.Generator1.Output, NeuronOutputs←Neuron.LTZone.Output, TargetClassInput←DatasetMatrix.CurrentClass.

### Полнота паттерна и границы trial

При найденном DatasetMatrix через learner (включая Branch) expected stim count, sample id и класс берутся из dataset. Паттерн полон при observed>=expected. Без доступного expected count legacy fallback — **observed>=4**; одиночный стимул больше не объявляется полным. Это ограничение fallback, а не общая поддержка произвольного N.

neuron_fired выставляется после complete в PostPatternWindow относительно последнего стимула. Метка класса фиксируется в BeginTrial. После timely fire наблюдение продолжается до trial_observe_until_=last_stim+EffectiveLateWindow, **если раньше не закрыт trial при переходе к следующему sample**.

Текущие ограничения (R01/R02 аудита): sample advance может сократить окно без явного censored-статуса; между advance и первым новым стимулом output edge может остаться незаписанным. При совпадении sample advance и нового stimulus edge неполный старый trial может поглотить стимул нового sample. Этот последний переход воспроизведён изолированно; его достижимость в полном scheduler требует проверки.

### CSV и Python-метрики

Поля: trial,target_class,stim_count,isi0..isi3,neuron_fired,neuron_t_rel,match,late_fired,late_t_rel,error_class,ltz_potential_max,soma_amp_0..3,soma_amp_sum,neuron_spike_count,neuron_spike_times,response_class.

- При MatchMode=0 (default) match учитывает in-window neuron_fired; late в нём не учтён. MatchMode=1 использует IsIsiTemplateMatch независимо от fired.
- neuron_spike_count и neuron_spike_times сохраняют не более 16 rising-edge открытого trial (count — размер ограниченного списка); overflow отдельно не отмечается. Времена заданы относительно первого стимула. Закрытое/обрезанное окно не восстанавливается последующим пересчётом CSV.
- per_stim использует injective matching: один spike не может покрывать несколько stimulus windows.
- Python metrics_version=2 учитывает любой записанный foil-spike как FP; last_pulse_ok использует сумму ISI. Но schema_ok не проверяет все противоречия/count/NaN; classify отдельно не гарантирует last_pulse_ok.
- Четыре ISI-поля не описывают произвольный N>5; N=1 требует согласования single/per_stim и last_pulse. Поле ok_audit_legacy не воспроизводит исторический scorer на всех входах.

Критерий «один target-spike после последнего стимула, тишина всех foils за полное одинаковое окно» требует полной записи наблюдений и проверки валидности. Текущие исправления сами по себе ещё не обеспечивают его во всех переходах.

### Свойства

| Имя | Смысл |
|---|---|
| PostPatternWindow / LateResponseWindow | Окна в секундах |
| PulseDetectThreshold | Порог rising-edge, default 0 |
| SavePath / FileName | Путь CSV относительно data dir |
| AppendMode | false: пересоздание CSV при Reset |
| Enable | Включение анализа |
| TrialIndex, LastFired, LastMatch, LastLateFired, LastErrorClass | Последний закрытый trial |
| LastNeuronDelay, LastLateNeuronDelay, LastIsi, LastSpikeCount, LastResponseClass | Времена, ISI и морфология |

## EN

The dataset supplies expected stimulus count, sample identity and the immutable trial label. Completion uses observed>=expected; without an expected count, the legacy fallback is observed>=4. A timely fire no longer immediately ends observation. However, dataset advance can still close the trial before its observation deadline, and an output edge before the next first stimulus can be lost. The incomplete-trial/coincident-sample transition also needs an explicit boundary rule.

CSV spike count and timestamps are both capped at 16; overflow is not flagged. Default MatchMode=0 uses in-window fire, while MatchMode=1 matches the ISI template independently of fire. Per-stim matching is injective. Python metrics v2 counts all recorded foil spikes and last_pulse_ok sums ISIs. Full schema validity, N=1, the fixed four-ISI CSV format for larger N, and exact legacy-scoring compatibility remain limited. A passing CSV score does not certify unobserved portions of a truncated window. See the linked review for executable counterexamples and the planned contract.
