# Общий matched N+2 context: mechanical candidate

Отдельный mechanical commit от Pulse fce3cdf переносит проверки до выбора
максимума из SelectMatchedOmissionMaximum в private implementation helper
`Core/TimeLearner/MatchedOmissionValidation.h/.cpp`. Контракт/порядок проверок,
Validity и reason strings сохраняются. Сами FullMaxima, support, choose и
repeated-winner checks остаются побайтно прежними после LF normalization.

Проверки охватывают policy, размер/masks/порядок controls, model/source/reset/
instance/epoch/routing/baseline, actual events/frames, форму source, coverage,
chronology, budgets и pointwise full repeat. CheckedContext создаётся только
factory после этих проверок, не копируется и не имеет public constructor/bool.
Это private borrowed context на время вызова selector, не persistent proof и не
независимая аттестация live native reset/ownership произвольных входных structs.

[Static receipt](MatchedOmissionContextExtractionVerification.json) фиксирует
точное равенство перенесённых блоков и неизменённых алгоритмических блоков.
Он не является runtime A/B. Root standalone v1 target добавляет новый production
source. Native execution/portable/reader gates и сравнение v1 outputs между
исходным и mechanical source обязательны отдельно. Локально C++ не собирается
и численные tests не исполняются.
