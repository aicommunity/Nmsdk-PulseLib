# Native API: диагностические подмножества импульсов

Новый schema6 находится в feature branch и ещё не принят native/runtime
проверками. Он не заменяет алгоритм назначения пиков штатной пачки.

`ProbeSpec::stimulusSubset` хранит hash исходного training pattern,
`physicalInputCount` и возрастающий список исходных `sourceOrdinals`.
`NativeProbeRequest::expectedInputIsi` всегда содержит полный исходный
паттерн; `patternHash` — hash временно предъявляемого подмножества.
Номера событий сохраняются как `pulse<sourceOrdinal>`.

`PlanStimulusSubset` создаёт новые ISI и запрошенные source offsets.
Его результат не доказывает совпадение фактических emission ticks: после
перегруппировки double-сложений требуется native readback. Пустое множество
не поддерживается. Неиспользуемые исходные интервалы также валидируются.

Executor временно меняет только собственный Dataset на штатной границе
подготовки, вызывает его обычный Build и восстанавливает полный Dataset
после закрытия/отмены. Размерность и параметры нейрона, training target,
storage-образы и refs не меняются. Никакого вложенного Calculate нет.
Существующий reset физического поддерева выполняется штатным executor.

Schema6 наблюдает полное объявленное окно и реальные события, но не создаёт
peak windows, assignment, references или TrainingCertificate. Запись spikes
не поддерживается: отсутствие списка не означает нулевой спайковый ответ.
`FinalVerification` отвергает metadata такого опыта даже после подмены его
номера схемы на старый. Codecs сохраняют purpose и source ordinals строго.

Обязателен `onSample`: без получателя waveform-потока запрос отвергается
до transient presentation. Владелец sink отвечает за сохранение каждого
sample; отдельный artifact reader должен проверять его полноту. Metadata
сами по себе не доказывают наличие waveform. Native fixture wrapper2
дополнительно сохраняет фактические training/Dataset patterns после пробы,
а не только boolean восстановления. Эти дополнения ещё не проверены.

Schemas1–5 сохраняют прежний смысл. Console/job parser ещё не предоставляет
этот режим; утверждать поддержку его через CLI нельзя. Состояние проверок
и последующие научные опыты ведутся в корневом репозитории:
`Docs/StructTrain/TimeLearner/StimulusSubsetDiagnostics.ru.md`.
