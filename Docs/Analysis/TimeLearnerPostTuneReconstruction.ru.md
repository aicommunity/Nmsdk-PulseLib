# Реконструкция физического графа PostTune

Branch использует общий `ReconstructNativeLearnerNeuron`, основанный на native
Save/Load/Build/Init фактического графа. Parent C=0 не означает 1 nF: параметры
дочерних каналов, LTMembrane, индивидуальные Rs/Ns и реальные связи сохраняются.
Нет ApplyElementDefaults, uniform parent overlays или повторного wake-pass Build.

Подготовка выполняется до удаления старого neuron. Attachment и semantic
identity проверяются до освобождения исходного объекта; порядок owner
восстанавливается. На ошибке attachment возвращается прежний объект. Внешние
listeners запрещают замену до detach. Caller обновляет alias Neuron после
обоих исходов. Физическая динамика новая, это не mid-probe checkpoint.

Branch не продолжает PostTune после неудачной реконструкции: PostTuneResult
SetupFailure, PostTrainTuneComplete=false, failed phase. Bool lifecycle методов
не служат протоколом ошибок. Вложенный Calculate не добавляется.

Native root e7ee0104 / PulseLib d862127: на Elder normal и M40 ASan прошли
по 10 C++ и 7 independent Python tests, 10090 пар parallel stimulus/soma samples.
Есть реальные N0 Ns=3 при 2/20 кГц, heterogeneous C при parent C=0, injected
attachment failure и external listener. Исходная implementation 593b69c с
теми же fixtures дала по восемь воспроизведённых отказов на обоих workers.

Это проверка реконструкции, а не всей транзакции выбора PostTune candidates.
Mode preparation не заменяет полный SearchSynthetic, реальную калибровку и
независимую оценку. Current source/run scopes, сохранённые неудачные fixtures
и Git artifacts описаны в корневом
`Docs/StructTrain/TimeLearner/PostTuneNativeRebuild.ru.md`. Подэтап ещё не merge
в develop и не закрывает этап 8 всего плана.
