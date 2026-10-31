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
не является восьмипаттерновой серией. N0 gate добавлен отдельно и ещё не принят.

Native visual bundle d94309d8 покрывает 24 parent reports, 61 completed matrices,
184 trial-soma pairs и 8668 cache files на Data M40. 20 evidence и 9 report/plot
checks прошли; 16 selected PNG просмотрены в исходном размере. Конечное окно
не доказывает settling/отсутствие более поздних событий. Полная visual acceptance
и новые production Console waveforms остаются открытыми.

Полная постановка, budgets, режимы и
остальные обязательства: `Docs/StructTrain/TimeLearner/NativePostTuneController.ru.md`
корня. Public learner property dispatch, continuation и полная mode/profile/frequency
matrix остаются незавершёнными. Legacy ControllerVersion=1 не заменяется этим API.
