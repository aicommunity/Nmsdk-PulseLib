# Повторы измерения кандидата N0

Рабочий подэтап stage07/n0-repeatability, 30.10.2026. Приёмка ещё выполняется;
develop merge и рекомендация для всей матрицы не объявлены.

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
