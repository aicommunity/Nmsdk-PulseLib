# NCGenerator — NConstGenerator

## RU

`NCGenerator` — storage-образ C++ класса `NConstGenerator`, наследника `NSource`.
Это постоянный источник: не отдельный C++ alias и не ограничение классическими
нейронными сетями. CreateClassSamples регистрирует образ с именем CGenerator.
`NPNeuronPosCGenerator` и `NPNeuronNegCGenerator` — его штатные образы с Amplitude
соответственно +1 и −1; они используются в PulseLib-моделях мембран.

Публичный параметр `Amplitude` имеет default 100 в NConstGenerator::ADefault.
Фактический default конкретного storage-образа может отличаться. Output — матрица
1×1. Setter изменившейся Amplitude отмечает необходимость обновления Output.
ACalculate при этом публикует Amplitude и снимает update flag.

Native AReset сразу присваивает Output текущую Amplitude и устанавливает update
flag. Состояние до первого тика потому соответствует постоянному источнику,
даже если потребитель находится раньше источника в computing order. До этого
исправления Reset оставлял прежний Output, что давало разные первые отсчёты
у свежего и уже рассчитанного нейрона. Не требуется перестановка общего scheduler
или дополнительный Calculate из learner.

Uniform значение задают новым storage-образом до построения нейрона. Не менять
общий prototype при работающих экземплярах и не возвращать параметры к образу
через повторный Default после TakeObject.

Проверки: корневые Test_PulseLib_LifecycleContracts.cpp и
Test_PulseLib_ModelRoundtrip.cpp; отчёт
[Lifecycle contracts](../Analysis/TimeLearnerLifecycleContracts.ru.md).

## EN

`NCGenerator` is the storage image of C++ `NConstGenerator : NSource`. Its default
Amplitude is 100; specific images may differ. Pulse-neuron images provide +1/−1.
Reset publishes the current Amplitude immediately, before any consumer's first
tick; Calculate refreshes the flagged output. This preserves the native schedule
and removes dependence on the source's previous calculated state. Use immutable
storage images for uniform physical settings.
