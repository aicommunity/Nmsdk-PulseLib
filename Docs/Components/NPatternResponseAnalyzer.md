# NPatternResponseAnalyzer — измерение ответов на паттерны

## RU

Analyzer регистрирует rising-edge стимулов и выхода нейрона. StimulusInputs
подключаются к DatasetMatrix.Generator1.Output, NeuronOutputs — к LTZone.Output,
TargetClassInput — к классу сэмпла. Метка фиксируется в BeginTrial. При доступном
dataset ожидаемое число стимулов и sample id берутся из него. Legacy fallback без
dataset требует минимум четыре observed стимула и не подтверждает произвольный N.

Ответ классифицируется относительно последнего стимула. Наблюдение продолжается
после timely fire до last_stim + EffectiveLateWindow. Досрочное закрытие при
sample advance помечает censored. Неполный паттерн помечает incomplete. Такие
наблюдения не подтверждают тишину фойлов.

### CSV

ObservationSchemaVersion=3 сохраняет прежний набор колонок. Версия 4 выбирается
явно и добавляет run_id, trial_id, sample_id, pattern_hash, model_revision,
onset_abs, required_observe_end, measurement_valid и soma_amplitudes. RunId и
ModelRevision задаются launcher из immutable manifest. Hash описывает наблюдённые
stimulus offsets (fnv1a64); он не заменяет hash конфигурации/исходных данных.

Soma peaks хранятся для всех входных строк. В schema 4 полный массив записывается
через ";", а optional trace добавляет soma_potentials. Старые soma_amp_0..3
сохраняются для чтения legacy tools. CSV пишется с precision 17 и classic locale.

NaN/Inf или изменение размерности во время пробы инвалидируют измерение даже
при сохранённом прежнем конечном максимуме. Ограничение spike timestamps — 16;
превышение выдаёт error_class=overflow. Невалидное измерение выдаёт
invalid_measurement. Match такого наблюдения равен 0. Rest-state и физическая
динамика модели не изменяются analyzer.

При AppendMode заголовок существующего CSV должен совпадать с выбранной схемой.
Смена схемы не дописывает строки к несовместимому заголовку. Неизвестные версии
не записываются. Ошибки записи обнаруживаются pipeline как отсутствие полных
наблюдений; bool lifecycle остаются true.

### Scorer

Metrics version 4 использует метки target/foil и не зависит от позиции target.
Проверяются уникальные trial IDs, ожидаемое множество проб, schema version,
complete/censored, числа, stimulus/spike timestamps и их согласованность.
ok_audit и ok_audit_v2 — один verdict, включающий last-pulse. Для нового run
schema 4 дополнительно требует provenance и полное окно наблюдения.

Старые CSV без доказательств полноты сохраняют историческую точность и target hit,
но не получают новый strict PASS. Августовские визуально подтверждённые 4/8–6/8
не отменяются более строгим gate. Same-Matrix calibration и независимый frozen
Test должны иметь разные protocol IDs.

### Свойства

| Имя | Смысл |
|---|---|
| PostPatternWindow / LateResponseWindow | Окна в секундах |
| PulseDetectThreshold | Порог rising-edge; default 0 |
| SavePath / FileName | CSV относительно data dir |
| AppendMode | false пересоздаёт CSV при Reset; true требует совместимый header |
| ObservationSchemaVersion | 3 — совместимость; 4 — новый observation contract |
| RunId / ModelRevision | Идентификаторы immutable запуска и измеряемой модели |
| Enable | Включает сбор наблюдений |
| TraceLogEnabled / TraceLogFileName | Optional trace, без изменения обучения |
| TrialIndex, LastFired, LastMatch, LastLateFired, LastErrorClass | Последний trial |
| LastNeuronDelay, LastLateNeuronDelay, LastIsi, LastSpikeCount, LastResponseClass | Времена, ISI, morphology |

Ограничения остаются явными: fallback без dataset, привязка к фактическому
вычислительному порядку, разрешение источников после перестройки модели и
полноценная end-to-end проверка границ trial входят в lifecycle/integration audit.

## EN

Schema 3 remains the default for old configurations. Explicit schema 4 adds run,
trial and sample identity, observed-pattern hash, model revision, absolute onset,
required observation deadline, validity and a variable-size soma array. Numeric
corruption, a dimension change or spike-list overflow invalidates evidence.
Early sample advance censors the observation. Append requires a matching header.

Metrics v4 identifies targets by labels, validates the expected trial set and
completeness, and uses one audit verdict including last-pulse timing. Historical
partial recognition is retained separately; missing historical metadata cannot
be reconstructed into proof of a complete modern trial. Model state, lifecycle
ordering, and independent frozen evaluation require their own integration checks.
