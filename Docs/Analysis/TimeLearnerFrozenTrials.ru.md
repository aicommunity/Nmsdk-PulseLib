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
