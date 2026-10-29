# PostTune: цель и временный stimulus

Production `0e1b01ae91addfe27c3a4731596f82d27321d01b` сохраняет InputPattern,
Neuron.TrainingPattern и PostTuneTargetIsi при временных предъявлениях.
PushPostTunePattern изменяет Dataset.MatrixData; BeginTrainingIteration в
single-presentation PostTune использует эту матрицу для ожидаемых arrivals.
Обычное обучение использует исходную цель. Native Reset сохраняется, новых
вложенных Calculate/Build, parent physics overlays и изменений образов нет.

На Elder normal и M40 ASan прошли по 4 native cases (Classic/Branch, 2/20 кГц),
8 полных presentations / 19368 samples. По 8 independent Python tests,
включая скрытую подмену посреди raw window. Общие waveform projections byte-exact
к корректному pre-fix baseline, где 4/4 случая меняли цель. Classic PostTune=3,
Branch PostTune=4; ранняя fixture с общей фазой 4 сохранена как ошибка постановки.

Это проверка production Push/Begin на native graph, не весь SearchSynthetic
и не сертификат обучения или научная selectivity. Полная candidate transaction,
V2 wiring, произвольный retarget, calibration и independent Test остаются открыты.

Подробный отчёт, полные source SHA и Git-артефакты:
`Docs/StructTrain/TimeLearner/PostTuneTransientStimulus.ru.md` корневого репозитория.
