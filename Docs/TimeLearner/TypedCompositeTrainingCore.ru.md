# Typed composite Train: live core, первый блок

Статус: реализация для отдельного server gate; runtime acceptance и научный результат не заявлены. Этот блок следует за механическим выделением scalar acceptance (`474df05b714415057bbcb4692ce224621ae55ac0`). Он не меняет defaults, старые эксперименты, старый метод influence-exclusive и схемы ProbeRecorder 1–6.

## Контракт

Новый opt-in `VerificationProtocol.schemaVersion=4` требует `CompositeMeasurementContract` schema1 с методом `declared-window-basin-necessity-v1`, полной context policy, geometry policy и явными относительными окнами каждого input. Старые wave/selection flags остаются неиспользуемыми default-полями. Старые protocol1–3 с непустым composite contract отклоняются. N=1 сохраняет существующий single-impulse путь; новый N+2 метод рассчитан на N>=2 и не ограничен N=2.

Окно фиксируется до первой raw-пробы группы. Эта версия не выбирает его по результату, не использует EstDelay и не утверждает физическую декомпозицию нелинейного ответа. Раздельная ранняя вершина допустима при хвосте, усиливающем позднюю. Локальный неразрешённый merge/Unknown не превращается в scalar; поздняя посторонняя вершина вне окна не становится глобальным veto. Геометрия и ограничения её причинной интерпретации наследуются от `DeclaredWindowBasinAssignment`.

`PreparedCompositeMeasurement` и `ValidatedCompositeMeasurement` имеют закрытые конструкторы. Ни public boolean, ни diagnostic validity/reasons, ни raw `ProbeObservation`, ни вызывающий код с произвольными double не создают такой тип. Factory владеет native группой и живой попыткой. Diagnostic bridge сохраняет `TrainingCertificateEligible=false`; это поле не меняется.

## Native authority

`NativeCompositeTrainingAttempt` принимает реального `NNeuronTimeLearner` или `NNeuronTimeLearnerBranch`. Он получает фактические private bindings и тот же `NativeProbeControl`; адаптер с произвольными callbacks не является публичным входом factory. Компонент должен уже принадлежать native model declaration; старый legacy/native service не должен одновременно собирать другую попытку.

В начале read-only захватываются:

- полный фактический initial neuron identity с внешним burst port;
- исходный model specification и полная immutable storage-image closure, включая declared growth/source roots;
- применённые source ISI, их canonical hash, фактический профиль генератора;
- initial model epoch, process-local owner/attempt generation;
- существующий C0 witness, если он уже принадлежит owner.

Это retained `SourceBinding`, а не один hash без данных. `SourceOrigin` — canonical length-framed UTF8/binary64 fingerprint этого binding. C-trim Off не требует C property, C0 capture или C-trim capability. Factory не создаёт фиктивные единичные C0, не recapture-ит baseline и не меняет физику. Если реальный C0 присутствует, каждый закрытый boundary проходит существующий `ReadNativeTrainingCapacitanceWitness` и binding origin; рост/active/dormant state остаются ответственностью существующего native editor. Неизменяемый исходный SourceBinding отличается от текущего candidate witness.

`StartMeasurement` принимает logical `VerificationRequest`, purpose Reference/Search/Final и обычный native burst prototype. Он создаёт одну собственную N+2 группу: fullBefore, omit0..omitN−1, fullAfter. Только первая реальная onPrepared выдаёт один typed logical Arm; omit/fullAfter не являются самостоятельными search measurements. Все raw callbacks остаются доступны artifact sink. Каждый child рассчитывает исходный Rdk scheduler; factory не вызывает Calculate/Reset/Build и не исправляет модель каждый tick.

API работает в одном scheduler thread. Перед actual model-time/data/declaration reads и cancel сначала проверяется Rdk owner lifecycle. Actual `IsNeedToTrain.GetData()` должен быть true: setter, connected source и setterless direct-data false наблюдаются на hooks, перед выдачей и перед consume. Отзыв запроса после close до consume бросает отдельный control-revocation `CompositeTrainingStopped`; typed consumers переходят в ExternalStop без нового accepted input/certificate и без invalid-observation charge. Уже завершённый certificate остаётся историческим завершённым результатом; снятие запроса не является новой Exact попыткой. Fixtures ставят реальный request=true после native construction, не используют постоянный callback true.

Per-input окна этого первого live core заданы immutable configuration protocol. Это scope первой реализации, не запрет на будущую controller policy: она может предложить другое declared окно до новой независимой группы, с новым measurement specification/proof и полностью свежими observations. В текущей версии такой proposal policy ещё нет; переинтерпретировать уже записанную группу удобным окном запрещено.

Native group сохраняет исходную context validation: фактические source/grid/event indices, physical channels, model/reset/epoch/instance bindings, closure, chronological absolute clocks, source readback и pointwise full repeat. Новый bridge вызывает тот же validator и strict native packet replay, затем новый geometry selector. Старый v1 assignment и его reasons не являются доказательством.

После закрытия требуется Valid geometry и post-assigned-event causality. Scalar берётся из реального fullBefore peak. Arrival и plateauEnd вычисляются относительно **ACTUAL события `protocol.originStimulusId` этой fullBefore группы**. Это не абсолютный native time и не delay от assigned input. Разные input groups начинаются в разное время, что не должно менять прежние sync residuals.

Перед mint proof и при typed consume private lease независимо перечитывает actual physical model, source/storage/C0 family и candidate witness. Передаваемый consumer-у cached revision не может скрыть реальный edit. Prepared admission возможен только в текущей ещё работающей группе, после реальной native preparation. Закрытый packet нельзя позднее Arm-ить как новую живую группу. После Cancel/destruction/lifecycle change lease недействительна. При устаревшем lifecycle factory не отменяет чужой новый executor job.

Текущий candidate binding хранит полный independently captured physical identity и actual optional C0 witness внутри closed measurement, отдельно от immutable initial SourceBinding. Это позволяет последующему codec использовать сохранённые boundary данные; их нельзя подменять поздним recapture. Rdk `UModule::BeforeDefault/BeforeBuild/BeforeReset/BeforeInit/BeforeUnInit` вызывают `AdvanceLifecycleObservation`; owner Reset/Default и pool reuse инвалидируют lease даже после реконструкции identical semantic graph. Обычный per-probe neuron/Dataset reset меняет child lifecycle, не owner lifecycle. Отдельный SourceBirthGeneration поверх существующего Rdk observation не введён.

## Capture/search/final

`CapturedReferences::CaptureComposite` принимает ровно N разных закрытых Reference groups на исходной revision/epoch/source family и одном candidate witness. TargetRawPotential каждого входа — его actual fullBefore selected raw potential. Group/proof IDs сохраняются в reference records. Raw Capture для protocol4 запрещён.

`BoundedTrainingSession::ArmComposite/ConsumeComposite` и `FinalVerificationSession::ArmComposite/ConsumeComposite` выполняют typed admission и затем вызывают **тот же private scalar acceptance path**, что и старый raw-путь. Candidate order, ранжирование, N0 repeats, L/Ns/logRs/C cursors и числовые tolerances не переписаны. Default amplitude absolute=1e−5, minimum amplitude=1e−6; sync задаёт прежний config. Reference — последний input N−1. Каждый edit по-прежнему инвалидирует все текущие измерения, включая Branch.

Anchor/input одной точки требуют одинаковый actual candidate witness. Итоговая верификация собирает свежие Final groups всех N входов на окончательной модели; Reference/Search purpose нельзя выдать за Final. Logical trial, group и semantic proof IDs защищают от replay. `TrainingCertificate.SchemaVersion=2` только для protocol4; внутренний certificate удерживает полный protocol/reference proof IDs/final proof IDs и прежние residuals. `Matches(..., Recognition, ...)` остаётся false.

Semantic proof hash включает declared method/policy/window/source origin/current witness/live attempt/logical group/purpose и фактические native packets+canonical primary CSV hashes. Относительные пути artifact files не входят в него. Group-specific native modelInstanceBinding не сравнивается между независимыми группами как owner identity.

## Бюджеты и границы этого commit

Logical groups, actual native probes и фактические sampled frames имеют отдельные счётчики/limits. Failed/cancelled current raw frames учитываются через реальный sample callback, даже если trace не попал в completed witness. Перед новой группой резервируется её declared sample upper bound. После исчерпания group/native/sample budget новые raw trials не ставятся в очередь. Модельное время берётся из реального owner Environment. Перед выдачей и при приёме proof проверяется actual steady wall clock, включая работу replay/selection/hash. Hook wall times должны принадлежать тому же clock, что `NativeTrainingWallTime`.

Это live-core API, а не завершённый controller product path. Пока **явно отклоняются**:

- serializing/decoding protocol4 через старый protocol codec;
- exporting certificate2 старым certificate1 codec;
- запуск protocol4 старым `NativeTrainingService`/cold config/package/CLI;
- old `AdvanceUnassignedBurst` recovery для нового метода.

Эти отказы не заменяют будущий функционал: следующий отдельный блок должен соединить authority с NativeTrainingService lifecycle, сохранить все reference/search/final/partial группы и primary blobs, добавить protocol4/config6/package5/certificate2 replay, warm restart/retarget и runner CLI. Текущий authority хранит исходный binding и последнюю группу; owning service обязан собирать каждую группу до следующего Start. Новый authority generation намеренно не импортируется из произвольного JSON. Перенос retained target через новый owner/service restart ещё не реализован. Legacy warm starts не изменены.

Не изменены Train/Recognition conversion, PostTune и OriginalExact propagation. Новый метод не включён в общие experiments/defaults. Ни certificate object в подготовленном test, ни положительный source review не объявляют научную сходимость или принятую реализацию до отдельных build/runtime/independent replay gates.
