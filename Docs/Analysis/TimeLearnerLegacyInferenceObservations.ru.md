# Наблюдения новой legacy PostTune inference-попытки

TL-037 воспроизведён через реальные storage/Environment, DatasetMatrix и typed
soma outputs обоих learner: прежний Result=Success и same-cardinality sample
states сохранялись при старте новой saved-matrix inference-попытки. Непосещённый
foil ошибочно становился измеренным нулём; наблюдались Success и mid=0.25.
На normal/ASan baseline d8e2ad16 / PulseLib ded4b558 получены шесть expected
failures из восьми controls; две существующие NaN controls прошли.

Core 39810a760103d2e5d5a5b9e6ae6575290522b20c сбрасывает attempt-scoped
validity, Result=None, Complete=false и margin diagnostics, создаёт новые
SampleMetricState для фактического числа samples текущей матрицы. Изменение
не сбрасывает весь learner, не меняет физику/storage-прототипы и не добавляет
Calculate из контроллера. Агрегация finite/observed/finished остаётся штатной.

Normal native root e3fe5340 прошёл 12/12: восемь адресных controls и четыре
реальных free-run Classic/Branch при 2000/20000 Гц. Независимый primary reader
прошёл 5/5 без skips; исходные CSV со всеми soma/input отсчётами опубликованы
в root bundle s08-posttune-observation-fixed-normal-a2. Реальные legacy исходы
могут быть BestEffort; это не TrainingCertificate/научный PASS. Полные ASan,
37 отдельных регрессионных cases и визуальные gates отслеживаются отдельно.

Начальный physical fixture ошибочно создавал NativeTrainingOwned граф, который
штатно обходит legacy ACalculate. Он остановлен с сохранением логов и owned-PID
receipt, не принят как memory/algorithm finding. Исправленный physical fixture
использует штатный legacy Build и проверяет отсутствие native ownership.

Ранее исправленные finite/timeout/Branch-metric findings не объявляются заново
открытыми по старому тексту аудита. Исторические измерения не пересматриваются.
Same-matrix/sample-0 legacy policy не заменяет label/ID/hash контракт native V2.
Полный transactional PostTune и матрица моделей этим подэтапом не принимаются.

Подробные статусы, исходники и ограничения:
[корневой контракт](../../../../Docs/StructTrain/TimeLearner/LegacyPostTuneObservation.ru.md).
