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

Адресная runtime проверка исправления Core
`7b5939c5744877ba1a762393d78f7183853fd5c0` завершена: baseline дал ровно
двадцать ожидаемых assertion failures на каждом worker, fixed A3 прошёл
51/51 native tests на Elder и M40 ASan. Независимый corrected readback прошёл
77/77, затем объединённый Python контур — 92/92 на каждом worker, без skips/
errors. M40 producer 8d73c0f и reader 02cf154 различались только анализом;
эквивалентность C++/gitlinks проверена отдельно, native job не перезапускался.

Все новые 338 probes / 507 trial–soma pairs каждого producer имеют полные
native graphics в M40 cache. По выбранной пользователем компактной политике
полные первичные данные, модели и runtime/source provenance опубликованы
lossless archive с шестью source-bound контрольными PNG. Pack/verify/extract
прошли 10/10 checks и 5/5 real boundary evidence checks после распаковки на
каждом worker. Raw bytes и исходная CapturedPerInput цель сохраняются.

Это приёмка компонентного исправления сводки и адресного native transfer,
а не полных этапов export/import, PostTune/calibration или всей mode/physics
матрицы. Новая full-cache regeneration и согласованный root merge описываются
в корневом отчёте отдельно; здесь не заявляется общий scientific PASS.
