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
CapturedPerInput. Выбор входа/фазы и automatic dispatcher ещё не реализованы.
Формулы mode/fraction/clamps и реальный выбранный Solo threshold проверены ниже;
подтверждение конечной recognition-модели остаётся открытым.

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
нужен реальный confirmation-прогон. Расчёт и Solo confirmation проверены ниже;
configured dispatcher и final recognition confirmation остаются открыты.

## Target-reference proposal: адресные runtime-проверки

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
calibrated/PASS. Две формулы, оба clamps, stale/ineligible источники и настоящий Solo burst
на выбранном пороге проверены на обоих workers.
Даже успешный Solo burst не подтверждает конечную recognition-модель или foil
responses. Выбор фаз, их automatic dispatcher и отдельная финальная проверка
конечного состояния по-прежнему открыты.

### Проверки target-reference 31.10.2026

Первый gate root 3d6bd14a677d3d28309d5fd9988f573d805120a2 /
PulseLib 273ae17d44b4118689c64b912d0e44595419d12a:
normal 33/33 (26.766 s), ASan 33/33 (170.859 s).
После исключения выдуманных общих defaults API требует все четыре параметра явно:
root b922aaa20119c559367d12b50c6185543890014d /
PulseLib 909647499fc9877fa1ad066d6330cb39326adbd1;
normal 8/8 (6.835 s), ASan 8/8 (44.246 s), без skips, leak checking выключен.
Public defaults не менялись: Classic fraction=0.85/min=0.0115/max=0.05;
Branch fraction=0.99/min=0.0115/max=1.0. Интеграционный fixture явно задаёт
fraction=0.5 и свои bounds; это не новые defaults продукта.

Независимые readers на последнем gate: normal 4/4 (0.704 s), ASan 4/4 (0.571 s),
без skips. На worker: 14 proposals, две actual выбранные Solo-пробы и 28 signed
corruption controls (10 trace + 16 proposal + 2 confirmation). Порог Classic
0.0016869611261023931, Branch 0.0014856928851619267; в каждом полном окне
зарегистрирован один настоящий spike. Это не 8/8 recognition: foil-пробы и
конечная parallel-модель здесь не проверяются. Источник — сохранённый Solo с
исходным Train outcome search_unresolved, не новое холодное обучение.

Latest job повторно использует завершённый runtime только после проверки живых
процессов; snapshots before/after одинаковы внутри задания. Перед повторным Elder
запуском удалены только 8 GNU ar caches завершённого старого Solo runtime,
86079652 logical bytes; top-level executable hashes сохранены. Исторические
логи и unique primary data не удалялись. Failed low-disk попытка сохранена.


Два подтверждающих Solo burst отрисованы тем же production renderer: 5 полных
channel-trace pairs (3 soma, 2 LTZone), без прореживания; source JSON неизменны.
M40 cache — 110 files / 6426888 bytes, rendering/checks 8.010 s. Git proof
s08-native-training-threshold-visual-asan-a1: 8 metadata members / 32452 bytes и
8 selected PNG. Все hashes и связи с двумя настоящими proposal/confirmation
записями проверены. Выбранные soma[0]/LTZone overview/first zoom обеих топологий
просмотрены при 1920×960. Soma/input scale читаем; spike после первого stimulus.
Shared Y LTZone подавляет малое положительное отклонение отрицательным recovery;
для визуального принятия crossing нужен отдельный positive-amplitude detail.
Полная visual acceptance не заявляется.


### Исправление amplitude-detail и legend layout

Недостаток positive crossing view предыдущего proof/a1 исправлен в root
164a8d5dcd575f32a5c99f22534307d0e80fbe75. При отрицательном recovery и настоящем
положительном отклонении добавляются явно помеченные positive-threshold-detail
views с линией фактического scalar threshold. Исходный full-recovery view,
все soma/input samples и actual spike markers сохранены. Отдельный top margin
для четырёх строк legend предотвращает перекрытие поля графика.

Proof s08-native-training-threshold-visual-asan-a3: 2/2 real-data report tests,
2 reports / 5 channel-trace pairs, 8 metadata members / 33844 bytes, 4 selected
LTZ PNG; cache M40 140 files / 8352286 bytes, render/checks 10.332 s. Все hashes,
primary/proposal bindings проверены; первичные записи не менялись. Четыре
1920×960 overview/first zoom positive-detail просмотрены: crossing читаем,
легенда выше осей. Промежуточный a2 прошёл автоматические проверки, но manual QA
выявил legend overlap; он помечен superseded, не принят визуально.

Прохождение threshold после первого stimulus не доказывает правильный момент
срабатывания на target, foil rejection или конечное parallel recognition.
Остальные виды имеют автоматическое покрытие; полной visual acceptance нет.
