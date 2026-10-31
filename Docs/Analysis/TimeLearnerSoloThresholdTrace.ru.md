# Свежая LTZ-трасса штатной Solo-пробы: адресные runtime-проверки

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
measurement, не новое независимое обучение. Source Train outcome не меняется.

## Проверки 31.10.2026

Native root a87bb8fc16d5ad0a48b5b337efb11c72c6d26724 / PulseLib
cd3e7d0b6f5b68b18a4f61c3e87468eb9012b419: normal 33/33 (27.605 s),
ASan 33/33 (161.506 s), без skips; ASan leak checking выключен.
Включены восемь новых сценариев и 25 lifecycle/boundary/codec regressions.
Независимый production reader root fe7adfd36dbb8f6826052186504fa5bd593e61e2:
normal 2/2 (0.192 s), ASan 2/2 (0.156 s). На каждом worker проверены восемь
реальных записей и десять повторно подписанных повреждений extrema, spikes,
validity, sample coverage и frequency. Valid записи дополнительно сверяются
с штатным peak audit по lossless CSV-проекции настоящих samples.

Компактные proofs опубликованы в корневом Reports/StructTrain/TimeLearner:
s08-native-solo-threshold-trace-normal-a1 и s08-native-solo-threshold-trace-asan-a1.
Каждый хранит 133 файла в сжатом архиве, в том числе два полных original portable
Train runs / 84 artifacts. Временные графики подготовлены отдельно; этот подэтап не закрывает полную
visual acceptance, configured dispatcher, калибровку или весь этап 8.

## Временные отчёты

Root 2ced776d00f8677cc61b595deedf4ceea09142a9: восемь отчётов, 12 soma и
8 LTZone traces с настоящим общим входом. Overview и native-tick zooms без
прореживания; actual spike events. Прерванные пробы показывают только записанный
участок; foil-кривые не выдумываются. Это не all-pattern recognition.
M40 report gate: 2/2, 19.414 s вместе с rendering, без skips. Восемь source JSON
не изменились. Полный кэш: 284 файла / 15624091 bytes на M40 Data. Компактный
корневой proof s08-native-solo-threshold-visual-asan-a1 хранит 44 metadata members /
73124 bytes и 8 PNG. Все hashes и восемь source bindings проверены.
Выбранные 1920×960 soma[0] nonspiking / LTZone spiking overview и первый zoom
просмотрены для обеих топологий. Остальные виды имеют автоматическое покрытие.
При threshold=0 фактический spike возникает в time zero, до первого stimulus;
это положительный контроль регистратора, а не успешная классификация.

При переносе target-reference формулы сохранить явный выбор samples: legacy
UpdateIterLTZPotential учитывает только ltz>0, тогда как raw recorder сохраняет
все конечные значения, включая нулевой baseline и отрицательный post-spike
recovery. Подставлять raw minimum в legacy gap_fraction без отдельного решения
нельзя. Свежие nonspiking extrema не подтверждают выбранный spiking threshold;
нужен реальный confirmation-прогон. Сам расчёт и dispatcher остаются открыты.

## Target-reference proposal: реализация, runtime ожидается

NativeTrainingThresholdReference использует только ClosedReport реального recorder,
не принимает произвольный импортированный JSON. Копия evidence не изменяет recorder.
Требуются завершённая valid nonspiking проба и совпадение current/base model revision.
Обе legacy-формулы сохранены: gap = positiveMin + fraction*(positiveMax-positiveMin),
peak = fraction*positiveMax; далее явные min/max clamps. Positive samples — строго
ltz>0 после native children; baseline исключён. Legacy span guard 1e-9 сохранён.
Fraction допускает любое конечное значение, как исходная формула; не вводится
скрытое ограничение [0,1]. Неконечный результат не публикуется как число.

Proposal хранит источник, границы, extrema/count, unclamped/clipped значения и
requires_spiking_confirmation=true. Он не пишет в нейрон и не утверждает
calibrated/PASS. Runtime-проверка двух формул, обоих clamps, stale/ineligible
источников и настоящего Solo burst на выбранном пороге пока ожидается.
Даже успешный Solo burst не подтверждает конечную recognition-модель или foil
responses. Выбор фаз, их automatic dispatcher и отдельная финальная проверка
конечного состояния по-прежнему открыты.
