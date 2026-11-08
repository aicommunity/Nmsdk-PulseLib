# Portable evidence диагностического анализа областей

`ResponseDomainEvidenceJson.h/.cpp` кодирует только успешно закрытый
`NativeResponseDomainCapture`. Закрытый native result хранится внутри capture;
caller не может подставить результат другой пробы или готовый analysis.
Domain graph повторно вычисляется production analyzer из той же записи.

Primary CSV содержит `time,stimulus,soma0,...` с locale classic и precision17.
Encoder проверяет точное совпадение переданных байтов с канонической записью
capture, безопасный относительный путь, finite числа и точность JSON counts.
Packet содержит SHA256/размер/header CSV, полный штатный native evidence,
declared domains, mapping физических каналов, все features и associations,
включая исходные индексы, плато, signs и причины отказа. Raw samples в JSON
не дублируются. Caller отвечает за NewOnly, flush/readback и публикацию файлов.

Kind `timelearner_native_response_domain_evidence`, schema1, method
`declared-response-domains-diagnostic-v1`; diagnostic_only=true,
training_certificate_eligible=false, scientific_verdict=null.
Mapping путей сам по себе не является независимым readback графа.

Codec не разрешает происхождение пика, не изменяет модель или Train.
Независимый reader находится в корневом пакете Scripts/StructTrain/TimeLearner.
Новая серверная проверка codec/reader требуется отдельно от прежних unit26
и первоначального native capture gate. Текущая реализация ещё не принята.
