# Измеряемые предложения C-кандидатов

Core/TimeLearner/MeasuredCapacitanceSearch содержит production kernel конечной
сетки абсолютных scales C/C₀ и предложения по signed timing intervals.
Refinement требует минимум три различных реально измеренных scale, одинаковый
comparisonScope остальных физических условий и согласованный измеренный trend.
Повторы объединяются полным интервалом; NaN, mixed scopes, nonmonotonicity и
неразрешённая неоднозначность не создают вымышленную сходимость. Прогноз — только
кандидат для последующей native проверки, не свидетельство пика или интегратора.

Pulse functional commit `0832d3ea17fa0a224a332d802f0cee00514c7cec` проверен
unit gates на обоих workers:12 новых cases +11 прежних length/resistance
regressions,23/23 normal и23/23 ASan. Компилировались полные production modules
и штатные test files root `b951fb3d44eb713a21d5aefd69b13f0e9440ac77`.
Root proofs `e311c97eeb0eb44270a027b9e2aec68ca16fdeb6` и
`6da55382f2600db42ed69073da281cb67e5626a5` содержат compiler/source/dependency
hashes, stdout/JUnit и loader diagnostics. Все members и9 source hashes
проверены. Это изолированные numerical unit tests, не полный SDK target.

Модуль пока не вызывается NativeTrainingService и не принят в develop.
ControllerVersion2 с SyncCapacitanceTrim On по-прежнему явно отклоняется.
Следующий обязательный шаг — полное C-состояние в action/candidate/evidence,
native comparisonScope provider, joint L/Ns/Rs/C preview/domain и bounded
поиск с исходными CapturedPerInput references и свежей all-input проверкой.
Полные contracts/tests/remaining steps находятся в корневом
Docs/StructTrain/TimeLearner/MeasuredCapacitanceSearch.ru.md. Принятие automatic
C₀ owner идёт отдельно; данный kernel не подтверждает его матрицу или physics.

## Подготовленный native bridge полного C-состояния

После unit130/130 normal и ASan (root proofs8214f206/88e96182) pure candidate
state уже переносит полный absolute scale map. Подготовлены новые методы обоих
learners ReadNativeTrainingCapacitance и ApplyNativeTrainingParameters,
делегирующие общей реализации NativeTrainingCapacitance. Отсутствие bound C₀
возвращает nullopt, а не выдуманную unit baseline. Владелец проверяет signed state,
реальные C/physics и applied model revision. Origin вычисляется из immutable
capture model revision, ModelSpec и storage-images fingerprint; расширение C₀
witnesses после роста не должно менять origin.

Полная controller map объединяет active/dormant scales и доказанные unit paths.
Применение требует неизменных origin/политики и сохранения всей известной истории;
новые paths и их C₀/active-dormant partition проверяет существующий совместный
native L/Ns/Rs/C preview. Смена origin, потеря path, NaN и unbound baseline
отклоняются до live mutation. Методы не вызывают Calculate/Default и не меняют
prototypes. Четыре реальные SDK проверки Classic/Branch × N0/N1 при2000Hz
подготовлены в root NativeTrainingCapacitanceBridge; runtime результат pending.

Это пока механический API, не dispatch NativeTrainingService, не schema3 summary
и не C-trim On. Следом обязательны подключение service/adapters, joint N1 domain
с тем же C, versioned codecs/readers, физические witnesses истории, C policy и
ограниченный измеряемый поиск. CapturedPerInput, anchor/default tolerances и
legacy absolute-F DendriteCapacitance не меняются. В develop bridge не принят.


## Typed N1 domain и schema3: подготовлено, не принято

Добавлен AssessNativeTrainingResistanceDomain в оба learner. Он строго
разрешает complete requested map относительно действующего C₀ owner и передаёт
тот же correction в существующий native preview L/Ns/Rs/C. Старые domain calls
сохраняют schema2 и прежнее поведение; новая capability запрашивается явно.

Новый результат различает requested и effective map: preview вправе добавить
доказанные новые unit paths, но не менять requested значения/origin/policy.
Сохраняются signed native state с applied preview revision и отдельно прочитанные
storage images с dormant roots. Witnesses публикуются в result только после
полной подготовки; ошибка подготовки не оставляет частично заполненного proof.

EncodeResistanceDomainAssessment выбирает schema3 для typed request. Она хранит
requested_capacitance, effective_capacitance, capacitance_state и
capacitance_storage_images. Available без полного witness запрещён. Unsupported
может иметь null effective proof, что не подтверждает физическую допустимость.
Нельзя молча сериализовать новый C-контекст в schema2. Pure controller не владеет
Qt/физикой: witnesses представлены непрозрачными JSON strings.

Корневые native проверки связывают N1 preview с последующим фактическим joint
commit (полный model hash), проверяют неизменность live model/owner и отказ при
смене policy. Independent schema3 reader обязан проверить C₀/registry/C и заново
вычислить Rs bounds по тому же preview. Эти изменения ещё не собирались и не
исполнялись; C-trim On и service dispatch остаются выключенными. Нужны отдельный
SDK gate после bridge H3, readers и ASan; до этого merge в develop запрещён.


## Explicit C₀ binding до первого search action (подготовлено)

BindNativeTrainingCapacitance(maximumRelativeChange) — отдельная metadata-only
операция setup, не физический edit. При первом вызове требуется Idle controller
без references/search, декларация модели и legal native probe boundary.
CaptureBoundNativeCapacitanceBaseline читает фактические C/physics дочерних
каналов, graph и storage definitions; строится unit map всех известных путей.
Encode/ValidateApplied и полный typed codec завершаются до присваивания owner.
Не вызываются Build/Reset/Calculate/Default, не меняются модель, scheduler,
prototypes, динамика или model epoch. Во время recognition и чужого training
управления bind запрещён. Ошибки не передаются через lifecycle bool.

Повторный вызов валидирует и возвращает существующий owner; он не recapture
текущие уже trimmed C и не меняет policy. Для импорта/retarget нельзя сбрасывать
owner и этим выдумывать утраченную историческую C₀. First bind описывает явно
объявленную текущую исходную модель; он не доказывает её исторический cold start.

Root NativeTrainingCapacitanceBinding содержит4 реальные Classic/Branch ×N0/N1
fixtures с heterogeneous C. Проверяются initial Idle/no tick/no edit/lifecycle
change, full graph identity при unit bind, повторный bind и отказ NaN/policy
без изменения модели, а также retained original C₀ после реального trim и
повторного bind. Сохраняются полные snapshots и actual probe/input/soma traces.
Сборка/проверки pending; до их приёмки setup не подключён к NativeTrainingService.


Дополнительно подготовлены2 native guards Classic/Branch N1: реально измеряются
reference probes, выполняются external stop и штатный Reset learner с сохранением
references; последующий first bind должен отвергаться с
training_capacitance_binding_requires_initial_model_boundary. Owner остаётся
unbound, граф и lifecycle неизменны. Это защищает от rebase через Reset и
перетолкования ранее измеренной модели как новой исходной C₀. Для этого выпуска
root gate содержит140 SDK cases и23 structured Python checks (bridge8+domain8+
binding7). Checks pending; C-trim On и native service integration не включены.


## Полная registry closure в witnesses (исправление по фактическому H4 snapshot)

C₀ baseline связывает не только neuron/channel images, но и dataset/generator
из ModelSpec. CaptureModelImageDefinitions(neuron, ports, channel_classes) не
включает их автоматически: Classes(identity) обходит только узлы и ClassName
selectors самого neuron. На actual H4 Classic N0 captured frame подтверждено
отсутствие NDatasetMatrix и NPulseGeneratorTransit относительно baseline.

Typed domain теперь перечитывает storage по ВСЕМ именам baseline.storage_images,
включая dormant, growth-only и stimulus/dataset definitions. Root bridge/binding
snapshots используют тот же способ. Это actual registry readback, не подстановка
сохранённых baseline definitions. Strict independent auditor не ослаблен.
Добавлены semantic negative controls: удалить NDatasetMatrix definition, которая
не имеет live consumer внутри neuron, всё равно означает profile mismatch.
Код и snapshots требуют отдельного SDK/runtime gate; научный/algorithm verdict
из отсутствовавших witnesses не выводится. C-trim/service остаются выключенными.

## Per-revision witness API (подготовлено, runtime pending)

`ReadNativeTrainingCapacitanceWitness` обоих learner использует общий read-only
producer. Unbound owner возвращает None. На legal boundary валидируются полный
native parameter state, actual C каждого канала и applied model revision.
Затем снимаются полная semantic identity и actual storage closure по всем именам
baseline (включая stimulus/dataset, dormant и growth-only images). Проверяется
совпадение storage semantics с исходным профилем, повторно валидируется native
state и сверяется identity до/после capture. Никаких Default/Build/Reset/Calculate,
замен C₀, изменений controller epoch или физических параметров не выполняется.

`TrainingCapacitanceWitness` содержит typed full map, model revision и opaque
JSON identity/native state/storage definitions. Это boundary parameter evidence,
не dynamics checkpoint и не certificate сходимости. Pure controller не разбирает
native JSON. API пока не подключён к NativeTrainingService или summary schema3:
нужны registry per-revision witnesses и отдельная независимая проверка истории.

Root bridge tests дополняются сопоставлением каждого witness с независимым
readback unbound/captured/growth/shrink/rejection/future-growth frames; lifecycle,
epoch и граф должны сохраняться. Fault injection меняет actual child C при
неизменном owner: witness обязан отказать без ремонта модели. Во время настоящей
scheduled probe чтение также обязано отказать. Эти новые assertions ещё не
выполнялись на сервере; текущий immutable H5 проверяет предыдущий snapshot.

Следующее дополнение: `EncodeNativeTrainingCapacitanceWitness` сериализует
version1 envelope с model revision, typed controller map, полной model identity,
signed native state и actual storage closure. Encoder проверяет согласованность
JSON/digests/revision/map/profile; физическое равенство C проверяется отдельно
native producer и независимым Python auditor. Root bridge emits пять bound
witness artifacts на case. Это подготовка таблицы истории для summary schema3,
не смена действующего summary1/2 и не включение C-search/service.

Root auditor дополнительно проверяет каждое effective child C по исходному C₀,
связывает registry keys с revisions, требует witnesses всех запрошенных revisions,
единство origin/policy и неизменность исходных полей/общих channel baselines
между расширяемыми inventories. Reject-контроли включают boolean scale, удалённый
dataset image, пропавший dormant correction, изменённый revision и физический C
даже после пересчёта всех digests. Все эти новые codec assertions runtime pending;
H6 проверяет только предыдущий read-only producer без нового encoder.
