# Свежая LTZ-трасса штатной Solo-пробы: runtime-приёмка ожидается

NativeSoloThresholdTrace дополняет NativeProbeRequest через штатные onPrepared /
onSample callbacks. Native executor по-прежнему один владеет routing, физическим
Reset и вычислением окна. Recorder не вызывает Calculate/Build/Init/Reset и не
пишет в компоненты или storage. Предыдущие artifact callbacks сохраняются.

В onPrepared после штатного physical reset фиксируются настоящий источник
Neuron.LTZone.Potential, его C++-поддержка scalar Threshold, frequency, baseline,
порог, lifecycle и base/effective model revisions. Нулевой baseline не требуется.
В onSample после native children записываются LTZ potential, реальные soma/input
samples, новые stimuli и реальные новые spike timestamps. Zero-time spikes не
теряются. Числа, размеры, времена, частота, число samples/spikes, источник и model
revision проверяются. Данные ограничены maximumSamples исходной пробы.

Finish принимает настоящий NativeProbeResult и связывает запись с полным ProbeSpec
через общий production serializer, routing, input selector, epoch и protocol.
Complete, valid и nonspiking различаются. ProposalEligible требует полного valid
окна без спайков; пик spiking-динамики не считается nonspiking reference.
Никакого подобранного порога или scientific verdict запись не утверждает.

Explicit learner Reset штатно отбрасывает executor result. Для такого прерывания
Interrupt сохраняет уже снятые реальные samples, native_probe=null и complete /
valid / proposal_eligible=false; cancelled result не выдумывается. Оба вида
закрытия однократны и не читают уничтоженные Rdk-компоненты.

Classic сохраняет полный поток на всех кончиках. Branch сохраняет штатное mute
активного подключения. Recorder не превращает Branch solo в parallel recognition,
не подменяет burst single-pulse стимулом и не изменяет математическую цель
CapturedPerInput. Выбор входа/фазы для target-reference, формулы mode/fraction/
clamps, реальная spiking confirmation и automatic dispatcher ещё не реализованы.

Адресные интеграционные сценарии: Classic/Branch × обычный burst, реальный
spiking threshold, Reset, изменение физического порога между native ticks.
Используются настоящие storage/Environment и сохранённый Source Solo; это replay
measurement, не новое независимое обучение. До завершения серверных проверок
runtime-приёмка не заявляется. Source Train outcome не меняется.
