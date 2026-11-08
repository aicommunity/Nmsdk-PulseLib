# Доставка фактических raw results при отмене composite Train

Статус: исправление в исходниках, до серверной сборки и runtime-приёмки.
Исходная PulseLib: `f925e37247bb987980fce80da47831a1c31d04c9`.
Отрицательный результат Elder сохранён в корне:
`90af73022c3c0955eaafa368ef5a7e00702147b1`.

`ServicePreparedStop` обнаружил реальный дефект: подготовлена одна проба,
её actual Cancelled packet сохранён группой, но observationSink вызван ноль раз.
В `InterruptComposite` временный adapter не содержал обратного вызова, поэтому
пакет обходил actor rawTrials и service observedPackets. Сходная потеря была
возможна после TakeResult, если последующая проверка trace завершалась ошибкой.
Требование closed=1 сохранено.

Группа теперь сначала сохраняет фактический packet и вызывает внутренний
capture-only обработчик, затем выполняет validation. Actor синхронизирует
rawTrials.packet и отдаёт только новые записи журнала текущей группы. Service
сохраняет их по уникальной паре run/trial текущей попытки и копирует sink.
Application callbacks не вызываются из mutable group dispatch и destructor.

Отмена захватывает фактический executor result до очистки Reset. Если group
frame ещё активен, обновление его журнала завершается при выходе из dispatch.
Cancel/TakeResult разрешены только для совпадающего собственного run/trial и
executor controlGeneration. Обычный путь/destructor требует current lifecycle.
Rdk увеличивает lifecycle до ABeforeReset/ABeforeDefault: фактический owner hook
получает узкую cleanup authority для alive owner и той же creation identity,
но не новую training lease. Поэтому даже новый request с повторёнными текстовыми
IDs после Reset не может быть отменён старым actor.

Stop завершает archive/control/revocation и terminal outcome до уведомления.
Перед пользовательским вызовом запись удаляется из очереди доставки; повторная
остановка не повторяет callback. После вызова проверяются actor, generation,
attempt/run и lifecycle: Reset/Default/retarget не получают старого Finish или
Archive. В lifecycle hook diagnostic delivery связана с его свежим lifecycle
token; этот token перепроверяется после callback и не разрешает обучение.
Ошибка нормального sink даёт InvalidObservation до consume; ошибка sink
отменённой пробы сохраняет ExternalStop с отдельной диагностикой без повторения.

NativeProbe копирует callable и prepared context перед вызовом; только его
исполняемая копия callbacks удерживает shared actor. Шаблон группы не образует
self-cycle. Generation executor защищает от Reset/Queue того же ID во время
callback и от перезаписи уже закрытого cancellation ошибкой старого frame.
Штатные Calculate/Reset/Default и порядок дочерних расчётов не заменяются.

Diagnostic cancellation сохраняет фактический packet и prequeue C0 witness,
даже если текущая C0-проверка не прошла или getter недоступен после lifecycle.
Причина отдельно записывается в diagnostic reason, terminalization не прерывается;
normal-current C0 validation остаётся строгой, certification отсутствует.

beforeCompositeConsume/compositeSink и их record arguments копируются до вызова.
Search SyncOutcome/публикация service certificate отложены до успешного
post-consume notification и свежей проверки обоих wall budgets. Сам факт consume
не отменяется в историческом record, если последующий callback завершился ошибкой;
итог остаётся InvalidObservation без certificate.

После callback используется свежий NativeTrainingWallTime перед consume и
certificate. Новые real-clock controls обоих типов ждут 16 секунд в sink
последнего raw packet и последнего final composite record при объявленном
group wall budget 15 секунд и большем attempt budget 60 секунд; ожидается
BudgetExhausted без references/certificate. Это отдельные реальные серверные
проверки, не подстановка времени.

Добавлены 36 Classic/Branch controls: throw/reset/default/retarget/recursive stop
на cancellation, warm без повторения старой доставки, throw/reset нормального
sink, actual physics corruption с доставкой невалидного packet, slow sink,
устаревший actor против новой пробы/ABA IDs и destructor без application callback,
изменённая C0 при отмене, stable record после Default из before/post-consume
callbacks, их throw и slow last-composite notification.
Старые 206 controls должны быть сохранены и повторно проверены вместе с новыми.
Математика выбора окна/максимума, tolerances и научные критерии не изменены.
