# Сводка импортированной границы модели

Рабочий подэтап `codex/timelearner-repeat-boundary`. Completed/stopped model
archive не является controller checkpoint посреди пробы. Native import
восстанавливает physical graph, goal и конечный proof либо диагностический
stop; Search при этом отсутствует. Сводка должна сообщать это явно.

Новые `native_training_state_summary` schema 1/2 содержат
`search_history_retained`: наличие текущего Search, без выдумывания прежних
кандидатов. При false отсутствуют search counters и accepted_parameters;
history arrays пусты. Schema 2 всегда содержит candidate_repeat_groups,
включая пустой массив для restored boundary. Certificate относится к
конкретному графу и независимым final measurements, а не к наличию Search.

Старые сводки без marker читаются как unknown. Внешний audit продолжает
проверять reference/final primary data и не превращает unknown в успешную
историю повторов. Политика repeats сверяется с full model package отдельно.

CapturedPerInput, допуски, native Build/Reset/Calculate и storage ownership
не меняются. Частичная группа исходного Search остаётся incomplete; после
warm restart выполняется новая попытка на сохранённой цели, не продолжение
интегратора посреди пробы.

Реальные адресные baseline/fixed проверки и ограничения:
[RepeatBoundaryState](../../../../Docs/StructTrain/TimeLearner/RepeatBoundaryState.ru.md).
До их завершения develop merge и полная матрица не объявляются.
