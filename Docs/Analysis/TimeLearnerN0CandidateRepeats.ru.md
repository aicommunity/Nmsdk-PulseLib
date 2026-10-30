# Повторы измерения кандидата N0

Подэтап stage07/n0-repeatability, 30.10.2026. Адресные native и Python проверки
завершены на обоих workers; согласование develop выполняется отдельно.
Рекомендация для всей матрицы не объявлена.

BoundedTrainingConfig.n0CandidateRepeats=1 сохраняет прежний протокол. Явный
режим 2..128 выполняет свежие anchor/input пары на неизменном графе и принимает
локальную готовность кандидата только по всем требуемым наблюдениям. Для
ранжирования берётся худший измеренный остаток, а не среднее и не знак ошибки.
CapturedPerInput, опорный N−1, границы Ns и native lifecycle сохраняются.

ConfiguredNativeTraining schema 2 объявляет search.n0_candidate_repeats;
schema 1 остаётся прежней. N1 с повторным N0 фактором отвергается как ошибочная
постановка. Сводка repeated режима имеет schema 2 и candidate_repeat_groups:
все raw potentials, min/max, IDs anchor/input, параметры и revision/epoch.
При invalid trial или budget stop partial history сохраняется как incomplete,
с явной причиной. Повторение детерминированной пробы не является независимым
случайным обучением.

Проверки включают оба адаптера, реальную configured cold загрузку и scheduler,
строгий primary JSON/CSV audit. Полный storage/export/import/continuation matrix
с новой политикой ещё обязателен. Полученный Exact остаётся сертификатом
остатков конкретного final-графа; это не результат разделимости и не noise
robustness guarantee.

Подробная постановка, попытки сборки и оставшиеся gates находятся в корневом
[N0CandidateRepeats.ru.md](../../../../Docs/StructTrain/TimeLearner/N0CandidateRepeats.ru.md).

Проверенный Core commit: `8682435ce8518ef71e55a2fb8ed292f634c256d0`,
root `42d1b58803d6b2060bd8993b60f17c0d74577d5b`. На Elder Release и M40 ASan
по 35/35 native и 38/38 Python/infrastructure tests без skips/errors. На каждом
worker независимо проверены восемь summaries и 132 primary trials, включая
48 repeated-profile trials / десять полных групп. Outcomes native fixtures —
SearchUnresolved, не научный PASS. Отчёты: Elder
`5fb83ecb2f5af3c7adf833ac57aca0b3a64f7edc`, M40
`9045b211253b7ad9b17e2a0049e34faf12d55883`; каждый содержит 686 stored / 684 raw
проверенных hashes. Полные временные графики всех 198 trial–soma пар каждого
worker опубликованы отдельно; [правила и покрытие визуализации](../../../../Docs/StructTrain/TimeLearner/NativeProbeVisualReports.ru.md).
