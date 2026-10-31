# Native PostTune controller

`Core/TimeLearner/NativePostTuneTransaction.h` реализует full-pattern weight
probes, bounded coordinate SearchSynthetic и передачу выбранной immutable модели
в реальный threshold-grid/fresh-confirmation driver. Все варианты Rs используют
native conversion/import с per-input bounds и проверкой физики. Общий scope
сохраняет владение моделью и пробами на обеих фазах; чужое состояние не откатывается.

Новый threshold controller может принять shared scope только того же Owner и
implementation ID. Default собственный scope сохраняется для прежних callers.
Calculate по-прежнему принадлежит штатному scheduler; Build/Init/Reset применяются
только на разрешённой границе импорта, bool не используются как error channel.

Положительный амплитудный gap сам по себе не означает Calibrated. Требуется
отдельная native threshold grid и fresh confirmation. Training certificate
источника не переносится на новый recognition graph. Raw matrices/history
сохраняются в parent report; статистическая независимость не утверждается.

Normal root 44a1598a / Core c8a1995 прошёл 10 native cases и 19 independent
checks без skips; это N1/2000 Гц/два входа, не полная приёмка контроллера.
Исправлен ложный recording-budget stop: учитывается физическое число отсчётов
`min(cap, ceil(duration*frequency)+2)`, а не весь разрешённый safety cap.
Все 24 parent reports/raw matrices и модели сохраняются; положительные пары,
search_unresolved и ошибки/отмены различаются. Source Train не становится Exact.

ASan того же native-контроллера завершён: 10/10 native (3733.602 s), 19/19
original reader (158.845 s), затем 20/20 strict-container reader на тех же данных
(159.205 s), без native rerun. Leak detection выключен; весь профильный matrix
этим не закрыт. Normal production Console root ffd5b15a2 / Python 74f49a5f0
выполнил 18 PostTune jobs, прежние calibration regressions и адресные portable
проверки. Связанные первичные данные: root bundle 3c1751f9. Отдельный ASan Console
тоже завершён: 18 PostTune jobs, 6 calibration regressions и адресные CLI/stop
controls; 4/4 PostTune, 8/8 calibration и 4/4 provenance reader checks без skips.
ASan bundle 88c47ddf: 1089 members / 6054552 bytes. Использовались сохранённые
normal модели; это не независимое переобучение. Калибровка двух паттернов
не является восьмипаттерновой серией. Отдельный N0 normal gate на source root
fe45166a прошёл Classic/Branch, 2/2 native (459.529 s), затем 12/12 evidence
checks (157.913 s), без skips. Bundle 437c92cd: 220 members / 3601844 bytes;
физика и допуски не ослаблялись. N0 ASan выполняется отдельно и ещё не принят.

Native visual bundle d94309d8 покрывает 24 parent reports, 61 completed matrices,
184 trial-soma pairs и 8668 cache files на Data M40. 20 evidence и 9 report/plot
checks прошли; 16 selected PNG просмотрены в исходном размере. Конечное окно
не доказывает settling/отсутствие более поздних событий. Полная visual acceptance
остаётся открытой.

Новые production Console waveforms также построены: renderer root 4c497f41d,
18 run-bound reports, 59 completed matrices, 178 trial-soma pairs, 8437 cache
files. По 8/8 report/plot checks на исходном и последнем reader 7e8d97f3f,
без skips (13.076 / 12.419 s). Compact bundle fd4f9f2b хранит 422 metadata
members / 335060 bytes и 12 selected PNG; полные отчёты остаются кэшем M40.
Выбранные PNG просмотрены в 1920×960, полная visual acceptance/PDF/SVG не приняты.
Нет независимого Test, доказательства вклада структурного обучения или 8/8.

Общий путь controls: `NativePostTuneConfiguration` выделяет строгий codec
девяти controls из Console в PulseLib. Root d48837f1 / PulseLib 30f737c8:
3/3 native cases без skips, все 18 опубликованных job declarations, direct
numeric bindings и отрицательные проверки. Два owned Console A/B (Classic Auto,
Branch SearchSynthetic) сохранили побайтно transaction и конечную модель
исходного bundle 3c1751f9; portable audits прошли. Новый proof bb80e56c:
103 members / 630420 bytes. Алгоритм и физика не менялись. Это prerequisite
configured phase plan, не работающий dispatch: оба optional guards остаются;
Production fixed job и configured phase continuation требуют отдельной реализации.

Полная постановка, budgets, режимы и
остальные обязательства: `Docs/StructTrain/TimeLearner/NativePostTuneController.ru.md`
корня. Public learner property dispatch, continuation и полная mode/profile/frequency
matrix остаются незавершёнными. Legacy ControllerVersion=1 не заменяется этим API.

## Проверенная граница fixed-threshold API (31.10.2026)

PulseLib 03d1ea86 / normal root 3d922ba2 / ASan root 38cd68a0:
12/12 native cases в обеих сборках (67.946 / 401.903 s), включая восемь
fixed controls и четыре codec cases. По два owned schema-1 Console A/B
сохранили исходные transaction/model bytes. Leak detection выключен.
Отдельный N0 ASan также завершён: 2/2 native, 3351.650 s; 12/12 reader,
136.129 s. Эти результаты заменяют прежние отметки pending для данных jobs,
но не принимают полную mode/profile/frequency matrix.

NativePostTuneFinalization::KeepFixedThreshold выбирает реально измеренную
weight-модель, сохраняет исходный порог и выдаёт WeightsSelected. Требует
пустой threshold grid и нулевой calibration budget. FinalPackage возвращает
выбранную модель; CalibratedPackage пуст. Source Solo/outcome не повышаются
до Exact. Selection proof хранится в parent sidecar; silent target остаётся FN.
Проверены native roundtrip и отдельная последующая calibration transaction.
При RecoveryRequired восстановление owner не предполагается; derived audit
считает calibration state unverified даже при raw declared fixed policy.

Python transaction analyze/report поддерживает schema 2. Root 2d6ab1a0
добавляет production Console/job/worker schema 2; scoped normal/ASan проверка
завершена: по шесть owned jobs и четыре malformed Console jobs без native output.
Fixed обоих learner: weights_selected; Classic calibration: calibrated, Branch:
search_unresolved. Legacy и v2 calibrate сохраняют исходные model/transaction bytes.
По 3 preflight и 4 final worker checks (final включает preflight); shared validator
проверен на всех 18 прежних portable runs. Root bundles fixed-console-normal-a1
(317 members) и fixed-console-asan-a1 (322) замыкают по шесть snapshots / 252
artifact entries. Leak detection off; полная матрица не принята.
Public learner properties к fixed API не подключены;
оба optional-mode guards остаются. Не удалять их без отдельного phase plan,
сохранения Solo outcome до import и legal-boundary lifecycle orchestration.
Согласованные root primary bundles: s08-native-posttune-fixed-normal-a2
(134 members, исправление пропущенных input/source в старой упаковке) и
s08-native-posttune-fixed-asan-a1 (117 members). Все bytes/hashes перепроверены.

## Прерывание перед lifecycle teardown: непроверенный подэтап

NativePostTuneTransaction и NativeThresholdCalibrationTransaction получают
InterruptOwnerLifecycle(reason): только terminal RecoveryRequired и диагностика,
без доступа к owner, отмены физической пробы, import, Build/Init/Reset или rollback.
PostTune рекурсивно прерывает принадлежащую ему calibration transaction.
Завершённая транзакция остаётся неизменной. Native lifecycle выполняет физическую
очистку; source_restored не устанавливается. Обычный Cancel сохраняет прежний
legal-boundary rollback. Эта операция нужна будущим configured phase hooks;
сами hooks и phase dispatch пока не подключены.

Подготовлены 16 real-component cases Classic/Branch для Reset, Default,
уничтожения scope и вложенной calibration; server acceptance ещё ожидается.
