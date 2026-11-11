# Native → declared-window basin: diagnostic bridge candidate

Bridge `NativeDeclaredWindowBasinAssignment.h/.cpp` — отдельный следующий commit
после mechanical extraction общего matched context. Ни v1 selector, ни его
portable codec1/исторические outcomes не заменяются. Train/CapturedReferences/
сертификаты здесь не подключаются; eligibility остаётся false.

NativeDeclaredWindowBasinSpecification явно связывает method ID, group/input/
assigned ordinal/channel и offsets окна. Окно фиксировано для данной N+2 группы
и repeat. Оно не подбирается по её peaks; внешний controller может заранее
объявить новое окно новой группы с новой specification под отдельной policy.

Порядок admission:

1. Проверить method/window binding и вызвать **тот же** private
   CheckMatchedOmissionContext, который теперь использует v1. В нём прежние
   model/reset/source/actual event/grid/coverage/chronology/budget/repeat checks.
   Factory-owned CheckedContext не конструируется через bool и не экспортируется
   как самостоятельное training proof.
2. Проверить closed group inventory и исходные planned/applied ISI, для каждой
   маски actual applied readback, canonical hash, native subset/profile/budgets.
   Каждый packet/trace проходит существующий production
   EncodeMatchedOmissionPrimaryTrace: strict native decode и Arm/Append/CloseNative
   replay. Это вход codec без v1 assignment/validity whitelist. Независимая live
   execution/reset attestation из такого replay не выводится.
3. Спроецировать actual выбранный канал на общий frame index после проверки
   actual grids/events. Общая ось — сохранённые absolute timestamps fullBefore;
   interpolated/one-tick-shifted данные не создаются. Вызвать pure geometry method
   с явно отдельной absolute repeatability policy.
4. Если выбранный actual full максимум предшествует actual assigned event по
   frame index **или** raw timestamp, вернуть CausalityRejected без geometry/
   measured result. Context/native rejection также не возвращает geometry.
   Completed означает выполнение диагностического анализа; geometry outcome
   может быть Missing/Ambiguous/Clipped/NonRepeatable и не является Train success.

Bridge не меняет модель, не вызывает Calculate/Build/Reset, не чинит параметры
на tick и не восстанавливает presentation — этим по-прежнему владеет native
runner. Source ISI/readbacks не интерпретируются как доказательство равенства
intended cumulative time и actual generator event time. Model/wall budgets
остаются declarations; из recorded packet span проверяется лишь model lower
bound, wall elapsed не выдумывается.

Root scoped filter `NativeDeclaredWindowBasinExecution.*` содержит14 real
storage/scheduler cases: Classic/Branch × assigned0/1 ×2000/20000 (8), merged
negative при2000 (2), stop/edit between controls (4). Valid complete cases
дополнительно проверяют13 отдельных context/native/source/readback/method
tamper refusals без изменения живого графа. Отдельный adversarial **packet**
control намеренно заменяет потенциалы копии и заново закрывает production
ProbeRecorder: геометрически необходимый максимум до assigned event должен
дать CausalityRejected без измерения. Это synthetic packet replay boundary,
не измеренный физический ответ; его samples не выдаются за native primary.
Их настоящие raw full/omit CSV и прежний v1
portable envelope сохраняются существующим writer; новый sidecar явно
diagnostic-only. Повторный анализ с удалённым optional v1 assignment даёт тот же
геометрический outcome: решение старого метода не является входным proof.
Имеющиеся10 v1 native execution controls и pure v1 cases не переписаны.

До normal/ASan A/B для mechanical pin и отдельного native bridge gate runtime
приёмка не заявляется. Synthetic causality-rejection control не заменяет
проверки native scheduler, source physics и reset ownership. Локальные сборки
и численные tests не запускались.
