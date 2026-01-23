# Научный фон и связанные публикации Nmsdk-PulseLib

## RU

### Назначение

Этот документ описывает научные концепции и публикации, лежащие в основе компонентов библиотеки Nmsdk-PulseLib. Библиотека реализует различные модели импульсных нейронных сетей (Spiking Neural Networks, SNN) и механизмы обучения, описанные в научной литературе.

### Основные тематики

#### 1. Модели импульсных нейронов

##### Модель Ижикевича (Izhikevich Model)

**Компоненты PulseLib:** `NPulseNeuronIzhikevich`, `NPulseChannelIzhikevich`, `NPulseMembraneIzhikevich`, `NPulseLTZoneIzhikevich`

**Научная основа:**

Модель Ижикевича — эффективная модель импульсного нейрона, способная имитировать различные типы нейронов с минимальными вычислительными затратами.

**Ключевая публикация:**
- Izhikevich, E. M. (2003). "Simple model of spiking neurons." IEEE Transactions on Neural Networks, 14(6), 1569-1572.
- Izhikevich, E. M. (2004). "Which model to use for cortical spiking neurons?" IEEE Transactions on Neural Networks, 15(5), 1063-1070.

**Связь с компонентами:**
- Параметры `A`, `B`, `C`, `D` в `NPulseNeuronIzhikevich` соответствуют параметрам модели из публикаций
- Модель позволяет имитировать регулярно спайкующие, быстро спайкующие, медленно спайкующие и другие типы нейронов

**Использование в конфигурациях:** `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`, `Bin/Configs/!OldConfigs/STDP-Simple-01/`

##### Модель Integrate-and-Fire

**Компоненты PulseLib:** `NIntegrateAndFireNeuron`, `NPulseChannelIaF`, `NPulseMembraneIaF`

**Научная основа:**

Классическая модель импульсного нейрона, основанная на интегрировании входных токов до достижения порога срабатывания.

**Ключевые публикации:**
- Lapicque, L. (1907). "Recherches quantitatives sur l'excitation électrique des nerfs traitée comme une polarisation." Journal de Physiologie et de Pathologie Générale, 9, 620-635.
- Gerstner, W., & Kistler, W. M. (2002). "Spiking neuron models: Single neurons, populations, plasticity." Cambridge University Press.

**Связь с компонентами:**
- `NIntegrateAndFireNeuron` реализует базовую модель IaF
- `NPulseChannelIaF` и `NPulseMembraneIaF` обеспечивают каналы и мембраны для IaF-модели

#### 2. Синаптическая пластичность и обучение

##### STDP (Spike-Timing Dependent Plasticity)

**Компоненты PulseLib:** `NSynapseStdp`, `NPulseSynapseStdp`, `NSynapseTrainerStdp*` (различные варианты)

**Научная основа:**

STDP — механизм синаптической пластичности, при котором изменение силы синапса зависит от временной корреляции между пресинаптическими и постсинаптическими спайками.

**Ключевые публикации:**
- Bi, G. Q., & Poo, M. M. (1998). "Synaptic modifications in cultured hippocampal neurons: Dependence on spike timing, synaptic strength, and postsynaptic cell type." Journal of Neuroscience, 18(24), 10464-10472.
- Song, S., Miller, K. D., & Abbott, L. F. (2000). "Competitive Hebbian learning through spike-timing-dependent synaptic plasticity." Nature Neuroscience, 3(9), 919-926.
- Markram, H., Lübke, J., Frotscher, M., & Sakmann, B. (1997). "Regulation of synaptic efficacy by coincidence of postsynaptic APs and EPSPs." Science, 275(5297), 213-215.

**Связь с компонентами:**
- `NSynapseStdp` реализует базовый механизм STDP с параметрами `TauPlus` и `TauMinus`
- Различные варианты `NSynapseTrainerStdp*` реализуют модификации STDP:
  - `NSynapseTrainerStdpLobov` — вариант STDP по Лобову
  - `NSynapseTrainerStdpTriplet` — тройной STDP
  - `NSynapseTrainerStdpStable` — стабильный вариант STDP

**Использование в конфигурациях:** `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`

##### Правило Хебба

**Компоненты PulseLib:** `NPulseHebbSynapse`, `NPHebbNeuron`, `NPulseHebbLifeSynapse`

**Научная основа:**

Классическое правило обучения: "нейроны, которые срабатывают вместе, связываются вместе".

**Ключевая публикация:**
- Hebb, D. O. (1949). "The Organization of Behavior: A Neuropsychological Theory." Wiley.

**Связь с компонентами:**
- `NPulseHebbSynapse` реализует синапс с обучением по правилу Хебба
- `NPHebbNeuron` — нейрон с встроенным механизмом Хеббовой пластичности

**Использование в конфигурациях:** `Bin/Configs/!OldConfigs/NM-Neurons/NM-PN-01-HebbNeuron-1M1St1In1/`

#### 3. Биоинспирированные модели

**Компоненты PulseLib:** `NPMembraneBio`, `NPExcChannelBio`, `NPInhChannelBio`, `NPSynapseBio`, `NSPNeuronGen`

**Научная основа:**

Биоинспирированные компоненты моделируют более детальные биологические механизмы нейронов, включая:
- Динамику ионных каналов
- Секрецию и диссоциацию нейромедиаторов
- Структуру нейрона (сома, дендриты)

**Ключевые концепции:**
- Hodgkin-Huxley модель ионных каналов
- Диффузия нейромедиаторов в синаптической щели
- Компартментальная модель нейрона

**Связь с компонентами:**
- `NPMembraneBio` моделирует мембрану с возбуждающими и тормозными каналами
- `NPExcChannelBio` / `NPInhChannelBio` моделируют ионные каналы с сопротивлением и емкостью
- `NPSynapseBio` моделирует синапс с временными константами секреции и диссоциации
- `NSPNeuronGen` позволяет создавать нейроны с настраиваемой структурой (сома, дендриты)

**Использование в конфигурациях:** `Bin/Configs/User/CognitiveNavigation/`

#### 4. Долгосрочная пластичность (LT-зоны)

**Компоненты PulseLib:** `NPulseLTZoneThreshold`, `NPLTZone`, `NPulseLTZoneIzhikevich`

**Научная основа:**

LT-зоны моделируют механизмы долгосрочной потенциации (LTP) и долгосрочной депрессии (LTD), которые лежат в основе формирования памяти.

**Ключевые публикации:**
- Bliss, T. V., & Lomo, T. (1973). "Long-lasting potentiation of synaptic transmission in the dentate area of the anaesthetized rabbit following stimulation of the perforant path." Journal of Physiology, 232(2), 331-356.
- Malenka, R. C., & Bear, M. F. (2004). "LTP and LTD: An embarrassment of riches." Neuron, 44(5), 5-21.

**Связь с компонентами:**
- `NPulseLTZoneThreshold` реализует LT-зону с пороговым механизмом активации
- Порог (`Threshold`) определяет минимальную активность для активации пластичности
- Временная константа (`TimeConstant`) определяет скорость накопления активности

**Использование в конфигурациях:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

#### 5. Классификация на основе спайков

**Компоненты PulseLib:** `NSpikeClassifier`, `NClassifier`, `NPCAClassifier`

**Научная основа:**

Классификация паттернов спайков — важная задача для импульсных нейросетей, связанная с распознаванием образов и обработкой временных последовательностей.

**Ключевые концепции:**
- Кодирование данных в паттерны спайков
- Обучение классификаторов на основе временных корреляций
- Использование PCA для снижения размерности признаков

**Связь с компонентами:**
- `NSpikeClassifier` классифицирует паттерны спайков от входных нейронов
- `NPCAClassifier` использует PCA для предобработки данных перед классификацией
- `NPattern` и `NDataset` используются для хранения обучающих данных

**Использование в конфигурациях:** `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`

#### 6. Когнитивная навигация и пространственная память

**Компоненты PulseLib:** `NSPNeuronGen`, `NPulseGeneratorTransit`, `NPulseLTZoneThreshold`, логические операции через комбинации нейронов

**Научная основа:**

Моделирование когнитивной навигации связано с исследованиями пространственной памяти, когнитивных карт и гиппокампальных нейронов места.

**Ключевые публикации:**
- O'Keefe, J., & Nadel, L. (1978). "The Hippocampus as a Cognitive Map." Oxford University Press.
- Moser, E. I., Kropff, E., & Moser, M. B. (2008). "Place cells, grid cells, and the brain's spatial representation system." Annual Review of Neuroscience, 31, 69-89.

**Связь с компонентами:**
- Сети с обратными связями и LT-зонами формируют пространственную память
- Логические операции (NOT, AND, OR) реализуются через комбинации нейронов
- Генераторы входных сигналов представляют пространственные стимулы (Forward, Back, Left, Right, Danger)

**Использование в конфигурациях:** `Bin/Configs/User/CognitiveNavigation/`

#### 7. Управление движением и моторный контроль

**Компоненты PulseLib:** `NMuscle`, `NEyeMuscle`, `NMotoneuron`, `NAfferentNeuron`, `NSAfferentNeuron`

**Научная основа:**

Моделирование моторного контроля через импульсные нейросети связано с исследованиями спинного мозга, моторных нейронов и проприоцепции.

**Ключевые концепции:**
- Моторные нейроны преобразуют сигналы ЦНС в мышечные команды
- Афферентные нейроны передают сенсорную информацию
- Обратная связь через проприоцепцию для контроля движения

**Связь с компонентами:**
- `NMotoneuron` моделирует моторные нейроны
- `NMuscle` / `NEyeMuscle` моделируют эффекторы (мышцы)
- `NAfferentNeuron` / `NSAfferentNeuron` моделируют афферентные нейроны

**Использование в конфигурациях:** `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/MC-Muscles/`

#### 8. Обработка биосигналов (sEMG, EEG)

**Компоненты PulseLib:** `NReceptor`, `NAfferentNeuron`, `NFileGenerator`

**Научная основа:**

Обработка биосигналов (поверхностная электромиография sEMG, электроэнцефалография EEG) через импульсные нейросети для классификации и анализа.

**Ключевые публикации:**
- Lobov, S. A., et al. (2015). "A Spiking Neural Network in sEMG Feature Extraction." Sensors, 15(11), 27894-27904.
- Lobov, S. A., et al. (2023). "Spatial Computing in Modular Spiking Neural Networks with a Robotic Embodiment." Mathematics, 11(1), 234.

**Связь с компонентами:**
- `NReceptor` используется для приема внешних сигналов
- `NAfferentNeuron` кодирует сигналы в паттерны спайков
- `NFileGenerator` может загружать данные из файлов для обработки

**Использование в конфигурациях:** `Bin/Configs/!OldConfigs/SpikeEEG/`, `Bin/Configs/!OldConfigs/NReceptor/`

### Модульные импульсные нейросети

**Компоненты PulseLib:** `NNet`, `NModel`, `NNeuronsLayer`, модульная структура компонентов

**Научная основа:**

Модульные архитектуры импульсных нейросетей позволяют создавать сложные системы из простых компонентов.

**Ключевые концепции:**
- Модульность и композиция сетей
- Иерархическая организация компонентов
- Переиспользование модулей для различных задач

**Связь с компонентами:**
- `NNet` и `NModel` служат контейнерами для организации модулей
- Компоненты могут быть вложены друг в друга (нейрон содержит мембраны, каналы, синапсы)
- Конфигурационные проекты позволяют создавать сложные модульные системы

### Связанные работы и дальнейшее чтение

Для более глубокого понимания компонентов библиотеки рекомендуется ознакомиться с:

1. **Общие работы по импульсным нейросетям:**
   - Maass, W. (1997). "Networks of spiking neurons: The third generation of neural network models." Neural Networks, 10(9), 1659-1671.
   - Gerstner, W., & Kistler, W. M. (2002). "Spiking neuron models: Single neurons, populations, plasticity." Cambridge University Press.

2. **STDP и синаптическая пластичность:**
   - Caporale, N., & Dan, Y. (2008). "Spike timing-dependent plasticity: A Hebbian learning rule." Annual Review of Neuroscience, 31, 25-46.

3. **Биоинспирированные модели:**
   - Hodgkin, A. L., & Huxley, A. F. (1952). "A quantitative description of membrane current and its application to conduction and excitation in nerve." Journal of Physiology, 117(4), 500-544.

4. **Когнитивная навигация:**
   - Buzsáki, G., & Moser, E. I. (2013). "Memory, navigation and theta rhythm in the hippocampal-entorhinal system." Nature Neuroscience, 16(2), 130-138.

### См. также

- [Architecture.md](Architecture.md) - архитектура библиотеки
- [API-Overview.md](API-Overview.md) - описание API компонентов
- [Config-Overview.md](Config-Overview.md) - описание конфигурационных проектов
- [Usage-Examples.md](Usage-Examples.md) - примеры использования

---

## EN

### Purpose

This document describes the scientific concepts and publications underlying the components of the Nmsdk-PulseLib library. The library implements various spiking neural network (SNN) models and learning mechanisms described in scientific literature.

### Main Topics

#### 1. Spiking Neuron Models

##### Izhikevich Model

**PulseLib Components:** `NPulseNeuronIzhikevich`, `NPulseChannelIzhikevich`, `NPulseMembraneIzhikevich`, `NPulseLTZoneIzhikevich`

**Key Publication:**
- Izhikevich, E. M. (2003). "Simple model of spiking neurons." IEEE Transactions on Neural Networks, 14(6), 1569-1572.

##### Integrate-and-Fire Model

**PulseLib Components:** `NIntegrateAndFireNeuron`, `NPulseChannelIaF`, `NPulseMembraneIaF`

**Key Publications:**
- Gerstner, W., & Kistler, W. M. (2002). "Spiking neuron models: Single neurons, populations, plasticity." Cambridge University Press.

#### 2. Synaptic Plasticity and Learning

##### STDP (Spike-Timing Dependent Plasticity)

**PulseLib Components:** `NSynapseStdp`, `NPulseSynapseStdp`, `NSynapseTrainerStdp*`

**Key Publications:**
- Bi, G. Q., & Poo, M. M. (1998). "Synaptic modifications in cultured hippocampal neurons: Dependence on spike timing, synaptic strength, and postsynaptic cell type." Journal of Neuroscience, 18(24), 10464-10472.
- Song, S., Miller, K. D., & Abbott, L. F. (2000). "Competitive Hebbian learning through spike-timing-dependent synaptic plasticity." Nature Neuroscience, 3(9), 919-926.

##### Hebb's Rule

**PulseLib Components:** `NPulseHebbSynapse`, `NPHebbNeuron`

**Key Publication:**
- Hebb, D. O. (1949). "The Organization of Behavior: A Neuropsychological Theory." Wiley.

#### 3. Bio-Inspired Models

**PulseLib Components:** `NPMembraneBio`, `NPExcChannelBio`, `NPInhChannelBio`, `NPSynapseBio`

**Key Concepts:**
- Hodgkin-Huxley model of ion channels
- Neurotransmitter diffusion in synaptic cleft
- Compartmental neuron model

#### 4. Long-Term Plasticity (LT-Zones)

**PulseLib Components:** `NPulseLTZoneThreshold`, `NPLTZone`

**Key Publications:**
- Bliss, T. V., & Lomo, T. (1973). "Long-lasting potentiation of synaptic transmission in the dentate area of the anaesthetized rabbit following stimulation of the perforant path." Journal of Physiology, 232(2), 331-356.

#### 5. Spike-Based Classification

**PulseLib Components:** `NSpikeClassifier`, `NClassifier`, `NPCAClassifier`

#### 6. Cognitive Navigation and Spatial Memory

**PulseLib Components:** `NSPNeuronGen`, `NPulseGeneratorTransit`, `NPulseLTZoneThreshold`

**Key Publications:**
- O'Keefe, J., & Nadel, L. (1978). "The Hippocampus as a Cognitive Map." Oxford University Press.
- Moser, E. I., Kropff, E., & Moser, M. B. (2008). "Place cells, grid cells, and the brain's spatial representation system." Annual Review of Neuroscience, 31, 69-89.

#### 7. Motion Control and Motor Control

**PulseLib Components:** `NMuscle`, `NEyeMuscle`, `NMotoneuron`, `NAfferentNeuron`

#### 8. Biosignal Processing (sEMG, EEG)

**PulseLib Components:** `NReceptor`, `NAfferentNeuron`, `NFileGenerator`

**Key Publications:**
- Lobov, S. A., et al. (2015). "A Spiking Neural Network in sEMG Feature Extraction." Sensors, 15(11), 27894-27904.
- Lobov, S. A., et al. (2023). "Spatial Computing in Modular Spiking Neural Networks with a Robotic Embodiment." Mathematics, 11(1), 234.

### See Also

- [Architecture.md](Architecture.md) - library architecture
- [API-Overview.md](API-Overview.md) - component API description
- [Config-Overview.md](Config-Overview.md) - configuration projects description
- [Usage-Examples.md](Usage-Examples.md) - usage examples
