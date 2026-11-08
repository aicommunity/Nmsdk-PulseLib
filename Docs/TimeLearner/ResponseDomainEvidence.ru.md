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
Normal-проверка связки выполнена отдельно от прежних unit26 и первоначального
native capture gate:10 integrity/codec checks,4 настоящие Classic/Branch пробы
на2000/20000 Гц,12 reader checks на каждом сервере и независимый пересчёт4
пакетов C++ по CSV. Compiled PulseLib:
`706813805ac54e21dc1bed46a68c62f358792394`; producer root:
`0e3d5bca3d89746ac6a14d2fef952083f2cdb968`; reader root:
`b22e3e443753bca3aa856b19a0e6948d28823b50`.
Root evidence `b10750b5cde0c9977b0d1861f3c205a4ef3fdd07` и receipts
`Docs/StructTrain/TimeLearner/ResponseDomainEvidenceNormalVerification.json`,
`ResponseDomainReaderM40Verification.json`. Дополнительный shared-output gate
старого/нового case2/2: `411a04b2ecb1798ea926543d5a7f64c5ed26e95e`.
Полная ASan-проверка связки на M40 выполняется; расширенная fault/N матрица,
Console wiring и causal attribution ещё не приняты. В Train этот формат не
включён. Общая готовность этапа7 или полного рефакторинга не заявляется.
