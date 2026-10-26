# Нативный исполнитель проб TimeLearner

`Core/TimeLearner/NativeProbe.h/.cpp` подключён к обоим реальным learner.
Это внутренний C++ API измерения, а не готовый ControllerVersion=2 или новый
Train CLI. Legacy поиск L/Ns/Rs и его EndOfLearning остаются отдельным путём.
Актуальные commits, серверные gates и ограничения приведены в
[корневом журнале подэтапа](../../../../Docs/StructTrain/TimeLearner/NativeExecution.ru.md).

## Вызов и владение

После native Init/Build готового learner вызывающий код передаёт
`NativeProbeRequest` в `QueueNativeProbe`. Контур вызывает BeforeChildren и
AfterChildren из штатных learner hooks; сам исполнитель не вызывает Calculate.
Дочерние компоненты рассчитывает общий scheduler.

`NativeProbeState`, `NativeProbeOwnsControl`, `TakeNativeProbeResult` позволяют
получить статус и завершённое наблюдение. До получения результата новую пробу
не ставят; повторные run/trial ID запрещены. Между пробами исполнитель сохраняет
управление и подавляет legacy edits. Неявного продолжения legacy Train нет.
Явный learner Reset/Default освобождает управление; neuron Reset для подготовки
пробы не сбрасывает learner. External stop отменяет пробу без выдачи Exact.

Запрос объявляет длительность, ожидаемый паттерн/ISI, sample/hash/IDs, окна,
activeInput, topology routing и readModelRevision. Callback ревизии обязан читать
реальную физику, частоту и связи, а не возвращать неизменную строку по обещанию.
onPrepared вызывается после native подготовки до первого стимула, onSample —
после физического измерения. Оба callback предназначены только для чтения;
их ошибки инвалидируют пробу.

## Физическая подготовка

Classic предъявляет одну пачку на кончики и читает физические сомы. Branch
временно оставляет активной выбранную точку подключения, сохраняя пачку целиком;
после пробы восстанавливает исходные typed Input bindings. Такая маршрутизация
входит в effective revision, тогда как base revision сохраняет solo-модель.
Persistent UNet связи не используются как замена фактически действующим pointers.

Подготовка выполняется между измерениями: native Neuron/Dataset Reset, измерение
модельного baseline, arm recorder, запуск источника. Rest не обязан быть нулём.
Actual stimuli читаются из OutputPulseTimes; PulseCounter и фронты не заменяют
историю реальных emissions. Сохраняется также typed source profile schema 2:
класс, amplitude, pulse length, frequency/deviation, transit/pattern modes.
Schema 1 остаётся отдельным историческим/контролируемым протоколом.

Явная [schema 3](TimeLearnerBurstAssignment.ru.md) дополнительно проверяет полную
последовательность и physical input ownership; default request остаётся schema 2.

Первый контур требует единой частоты всего дерева, штатного Dataset playback и
поддерживаемых source/routing dimensions. При незавершённом legacy physics edit,
PostTune или недопустимом графе preflight отклоняет пробу. Multirate legacy
конфигурации не становятся поддерживаемыми новым API автоматически.

## Свежесть и ограничения

Помимо semantic revision проверяются process-local lifecycle observations и
принадлежность слабых указателей текущему живому дереву. Reset с тем же model
hash меняет физическую историю и потому инвалидирует пробу. После удаления узла
нельзя читать или восстанавливать его через старый UEPtr, даже если pooled объект
ещё жив. Build/Init/UnInit сохраняют штатные guard и порядок Rdk; их bool не
превращаются в канал ошибок обучения.

Completed описывает выполнение окна, а качество задают validity/reasons
наблюдения. Native result сам не является сертификатом сходимости. Отдельный
[FinalVerificationSession](TimeLearnerFinalVerification.ru.md) проверяет каждую
свежую пробу и ограничения относительно собственных raw references.

Основной bounded search, глобальное сопоставление волн импульсам при изменяемой
морфологии Classic, полный режимный охват, versioned learned-state export,
conversion/PostTune/calibration и configured dispatch версии 2 ещё требуются.
Завершённые implementation fixtures не подменяют исторический replay или
научный результат распознавания.
