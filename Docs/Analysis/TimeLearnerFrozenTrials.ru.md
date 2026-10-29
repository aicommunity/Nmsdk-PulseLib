# Frozen recognition trial API

Оба TimeLearner предоставляют QueueNativeRecognitionTrial, TakeNativeRecognitionTrialResult
и CancelNativeRecognitionTrial после versioned ImportNativeRecognitionState.
Общий executor работает до/после штатного расчёта детей; вложенный Calculate
не используется. Transient Dataset pattern отделён от training target и
не меняет экспортируемый immutable recognition package.

ResetPerTrial и PreserveNeuronDynamics задают разные протоколы физической
динамики; source подаёт одну штатную пачку, фактические stimulus/spike histories
читаются каждый native tick. Сохраняются все soma potentials, LT output,
абсолютные/относительные времена, model hashes, complete/valid и причина
завершения. Silence допустим; отсутствие данных не становится нулевым ответом.

Первый проверяемый контур требует uniform rate, штатный deterministic generator,
один sample/feature, неперекрывающиеся импульсы, frozen threshold и storage physics.
Проба с пропуском кадров, отменой или Reset не получает valid result. Импорт
другой модели требует idle boundary и чтения предыдущего результата.

Это не калибровка, не controller checkpoint и не доказательство selectivity.
Исходная схема и состояние серверной приёмки: корневой документ
Docs/StructTrain/TimeLearner/FrozenRecognitionTrials.ru.md.

Scoped acceptance: root 127806a34 / Core 81ee217, Elder-1 normal и M40 ASan:
по 6/6 native, 12/12 Python/CLI, ноль skips/errors. Четыре fresh-owner модели
Classic N0 / Branch N1 при 2 kHz и двух storage thresholds (1 и 0.0001)
дают 16 полных target/foil проб, 8 invalid controls, 3864 primary frames и
4 actual spikes. Source proof проверен по 56 training trials. Пакеты доступны
в Git; по 414 stored / 412 raw hashes сверены локально. Порог не меняется
после Train; automatic calibration, все физические профили, дополнительные
Reset/recovery controls и общий experiment job остаются открытыми.
