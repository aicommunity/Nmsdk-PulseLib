# Финальная верификация ограничений TimeLearner

`Core/TimeLearner/FinalVerification.h/.cpp` реализует отдельный stateful
контроллер финального прохода и неизменяемые CapturedReferences/TrainingCertificate.
Это часть будущего общего контроллера. Legacy EndOfLearning и публичные режимы
двух learner пока не переключены на него; ControllerVersion=2 не объявлена
production-ready. Поиск L/Ns/Rs, conversion, PostTune и калибровка здесь не
реализованы и не имитируются успешными stub.

## References и протокол

CapturedReferences создаётся только из завершённых валидных ProbeObservation
для всех входов одной исходной модели/epoch. Сохраняются собственные измеренные
rawPotential, provenance run/trial/effective revision и порядок входов.
Последний вход N−1 остаётся опорным. Изменять goals у существующего набора
нельзя; retarget требует нового capture и инвалидирования прежней сессии.

Цель CapturedPerInput сравнивает rawPotential с исходным rawPotential того же
входа. Baseline-deviation не подменяет цель. Измеренный raw=0 или отрицательный
raw не означает missing: наличие положительного отклонения от объявленного
baseline и валидного пика проверяет recorder. Это поддержка измерителя/верификатора,
а не заявление о готовности всех нормализаторов для такой физики.

Протокол явно задаёт topology routing, sample/pattern identity, origin stimulus,
соответствие input↔stimulus IDs, полную пачку N импульсов для N входов, верхнюю
границу sample gap и minimum measurable amplitude. В Branch каждая проба должна
быть объявлена как solo выбранного входа и иметь одно соответствующее peak window.
Classic допускает измерение нескольких сом в одной пачке. Эталонные observations
могут разделять один Classic burst; Branch reference evidence не может повторно
использовать одну solo-пробу как измерение разных входов.

## Управление и границы Rdk

```text
request = NextRequest()
adapter prepares requested routing and native reset at a legal boundary
adapter validates base model key, actual effective graph and protocol
Arm(prepared_context)
native scheduler calculates children once; recorder collects raw physical data
Consume(closed_observation, current_base_model_revision)
repeat until every required input is freshly verified or a declared stop occurs
```

Порядок прохода Classic — 0…N−1; Branch — N−1…0. Готовность одного активного
входа не завершает сессию. Пустой input ID, неправильная маршрутизация и отсутствие
reference запрещают принятие. N=1 не является пустой проверкой: требуется
валидная свежая физическая проба и амплитудный остаток.

Пока проба armed, NextRequest не выдаёт повторный action. Сессия не вызывает
Build/Reset/Calculate и не владеет Rdk-компонентами. Её нельзя копировать как
checkpoint; адаптер отвечает за scheduler, gate/mute, reset и все model edits.
Runtime ошибки данных становятся InvalidObservation; неверная конфигурация
или нарушение программного порядка Arm остаются ошибками preflight/API.

## Revision, epoch и свежесть

Различаются baseModelRevision обученной solo-модели и effectiveModelRevision
конкретной пробы с её transient routing. Оба ключа производит адаптер.
Изменение выбранного Branch входа в рамках объявленного probe protocol не
переписывает learned state; его фактические связи всё равно входят в effective
probe revision. Изменение самой физики, topology или learned/logical bindings
меняет base key и modelEpoch, инвалидируя все ранее принятые входы.

Invalidate очищает весь проход, включая уже проверенный опорный вход, даже
если восстановлен прежний semantic hash. Epoch не сбрасывается. Бюджет уже
выданных проб тоже не сбрасывается. Если recorder сообщил mid-probe revision
change, либо текущий base key неизвестен, вся частичная верификация аннулируется.

RunId обязан быть уникальным ID verification attempt, а не ConditionSpec hash.
Внутри сессии requests получают уникальные trial IDs с epoch/serial; replay
старой либо unrequested пробы отвергается. Уникальность run IDs между отдельными
объектами/процессами обязан обеспечивать runner. Модуль сам не обнаруживает
неучтённую запись в Rdk, о которой адаптер не обновил key/epoch.

## Остатки и допуски

По завершении свежего прохода anchor timing берётся из **текущей** пробы входа
N−1, а не из замороженного prediction/captured timing. Время каждого пика
приводится к фактическому origin stimulus его пробы. Для отдельного входа
хранится signed интервал ошибки:

```text
low  = input_peak_start - anchor_peak_end
high = input_peak_end   - anchor_peak_start
timing_ok = max(abs(low), abs(high)) <= SyncTolerance
```

Plateau uncertainty не скрывается выбором удобного первого sample. Вход anchor
сравнивается с собой, поэтому его timing residual равен нулю; его реальный пик
и амплитуда всё равно обязательны.

Амплитудный остаток равен measuredRaw−capturedRaw. Допуск явно равен
absoluteTolerance + relativeTolerance·abs(capturedRaw). Defaults: absolute=1e-5,
relative=0, SyncTolerance=0.02. Их нельзя заменять на 1%, timestep, Rmin slack
или oscillation band. N0/N1 и физические границы не создают исключений из Exact.
Все численные fields, schema/context, множества stimulus/window/peak IDs и
соответствие raw/baseline/deviation/delay повторно проверяются; valid flag сам
по себе недостаточен. Это structural validation результата доверенного recorder,
не независимое восстановление всей waveform из DTO; traces сохраняются отдельно.

## Результат и сертификат

Exact выдаётся только после всех входов в одном base key/epoch/protocol.
OutsideTolerance сохраняет валидный **отрицательный** constraint certificate;
он не является Exact. InvalidObservation, BudgetExhausted и ExternalStop не
создают certificate. Bounded retry допускается только в заданном количестве,
с новым trial ID и сохранением rejected evidence. Исчерпание бюджета и локальный
stall не называются PhysicalInfeasible.

TrainingCertificate.Matches требует текущие model key/epoch, полный набор
references с provenance/protocol, явно совпадающие tolerances и LearnedSolo
state kind. В Recognition он не переносится автоматически, даже при одинаковом
численном hash (например, N=1 conversion). Matches подтверждает привязку;
потребитель обязан отдельно проверить Exact. Selectivity/calibration/recognition
качество не является результатом этого контроллера.

## Проверки и оставшаяся интеграция

Сигнальные fixtures используют production ProbeRecorder для N=1/2/4/6 и обеих
схем routing. Native N=1 fixtures строят реальные Classic/Branch, независимо
capturing reference и выполняя вторую физическую пробу при 2000/20000. Arm
происходит после подготовки реального графа, до первого стимула. Initial L=1
может удовлетворять единственному ограничению; изменение шаблона не обязательно.
Эти проверки не являются multi-input training или 8/8 recognition.

Остаются полноценные production adapters для обоих learner, base/effective
revision producers, versioned serialization/import certificate/references,
normalization/search outcomes и автоматический lifecycle общего контроллера.
Полная Branch mute/growth матрица и все PostTune/conversion сценарии требуют
отдельных native проверок. Исходные наблюдения августа этим не пересматриваются.
