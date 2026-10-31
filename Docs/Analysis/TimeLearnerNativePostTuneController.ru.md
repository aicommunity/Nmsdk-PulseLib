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

Это рабочий, ещё не принятый контроллер. Полная постановка, budgets, режимы и
остальные обязательства: `Docs/StructTrain/TimeLearner/NativePostTuneController.ru.md`
корня. Public learner/Console dispatch, continuation и полная mode/profile/frequency
matrix остаются незавершёнными. Legacy ControllerVersion=1 не заменяется этим API.
