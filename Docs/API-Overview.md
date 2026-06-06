# Обзор API Nmsdk-PulseLib

## RU

### Основные классы

#### PulseLibrary

Главный класс библиотеки, наследник `ULibrary`. Автоматически регистрирует все компоненты библиотеки при инициализации.

### Нейроны

#### NPulseNeuronIzhikevich

Нейрон по модели Ижикевича — одна из наиболее часто используемых моделей для имитации различных типов нейронов.

**Основные свойства (часто настраиваются в конфигурациях):**
- `A` (double) - параметр восстановления мембраны (обычно 0.02)
- `B` (double) - чувствительность переменной восстановления (обычно 0.2)
- `C` (double) - значение потенциала после спайка (обычно -65.0)
- `D` (double) - приращение переменной восстановления после спайка (обычно 8.0)
- `V` (double) - мембранный потенциал
- `U` (double) - переменная восстановления

**Использование:** `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`, `Bin/Configs/User/CognitiveNavigation/`

**См. также:** [NPulseNeuronIzhikevich](Components/NPulseNeuronIzhikevich.md)

#### NIntegrateAndFireNeuron

Классическая модель импульсного нейрона "интегрировать и стрелять".

**Параметры (ptPubParameter):**
- `C` (double) — параметр модели (ёмкость/постоянная времени)
- `U0` (double) — начальный потенциал
- `Upr` (double) — порог срабатывания

**Входы:** `I` (MDMatrix<double>) — входной ток.

**Выходы:** `U` (MDMatrix<double>) — мембранный потенциал (выход).

**См. также:** [NIntegrateAndFireNeuron](Components/NIntegrateAndFireNeuron.md)

#### NSPNeuronGen

Генерируемый нейрон с настраиваемой структурой (сома, дендриты, LT-зоны).

**Основные свойства (из конфигураций):**
- `MembraneClassName` - класс мембраны (например, "NPMembraneBio")
- `LTZoneClassName` - класс LT-зоны (например, "NPulseLTZoneThreshold")
- `ExcGeneratorClassName` - класс генератора возбуждающих токов
- `InhGeneratorClassName` - класс генератора тормозных токов
- `NumSomaMembraneParts` - количество частей сомы
- `NumDendriteMembranePartsVec` - вектор количества частей дендритов

**Использование:** `Bin/Configs/User/CognitiveNavigation/` (используется как `NSPNeuronGen`)

**См. также:** [NSPNeuron](Components/NSPNeuron.md)

### Синапсы

#### NSynapseStdp

Синапс с обучением STDP (Spike-Timing Dependent Plasticity) — наиболее распространенный механизм пластичности.

**Параметры STDP (ptPubParameter):**
- `XModCoeff`, `YModCoeff` — коэффициенты модификации для пост- и пресинаптической активности
- `APlus`, `AMinus` — амплитуды LTP/LTD (обычно ~0.01–0.1)
- `XTau`, `YTau` — временные константы для X/Y (обычно ~0.02)

**Состояние (ptPubState):** `XAvg`, `YAvg` — средние активности; `XYDiff` — разница влияний.

**Входы/выходы:** `PsActivityInput` (MDMatrix<double>) — активность постсинаптического нейрона; `StdpInfluence` (MDMatrix<double>) — влияние STDP на вес. От базового класса: `Weight`, `Input`, `Output`.

**Использование:** `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/SpikeSamples/STDP/`, эксперименты по обучению

**См. также:** [NSynapseStdp](Components/NSynapseStdp.md), [NSynapseTrainerStdp](Components/NSynapseTrainerStdp.md)

#### NPSynapseBio

Биоинспирированный синапс с моделированием секреции и диссоциации нейромедиатора.

**Основные свойства (из конфигураций):**
- `Weight` - вес синапса
- `PulseAmplitude` - амплитуда импульса (обычно 1.0)
- `SecretionTC` - временная константа секреции (обычно 0.001)
- `DissociationTC` - временная константа диссоциации (обычно 0.005)
- `Resistance` - сопротивление синапса (обычно 86000000)
- `Type` - тип синапса (-1 для возбуждающего, 1 для тормозного)

**Использование:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

**См. также:** [NPSynapseBio](Components/NPSynapseBio.md)

### Каналы и мембраны

#### NPExcChannelBio / NPInhChannelBio

Биоинспирированные каналы для возбуждающих и тормозных токов.

**Основные свойства:**
- `Resistance` - сопротивление канала (обычно 10000000)
- `RestingResistance` - сопротивление в покое
- `FBResistance` - сопротивление обратной связи
- `Capacity` - емкость (обычно 1e-9)
- `Type` - тип канала (-1 для возбуждающего, 1 для тормозного)

**Использование:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

**См. также:** [NPExcChannelBio](Components/NPExcChannelBio.md), [NPInhChannelBio](Components/NPInhChannelBio.md)

#### NPMembraneBio

Биоинспирированная мембрана с возбуждающими и тормозными каналами.

**Основные свойства:**
- `ExcChannelClassName` - класс возбуждающего канала
- `InhChannelClassName` - класс тормозного канала
- `SynapseClassName` - класс синапса
- `NumExcitatorySynapses` - количество возбуждающих синапсов
- `NumInhibitorySynapses` - количество тормозных синапсов
- `FeedbackGain` - коэффициент обратной связи

**Использование:** `Bin/Configs/User/CognitiveNavigation/`

**См. также:** [NPMembraneBio](Components/NPMembraneBio.md)

### LT-зоны (Long-Term пластичность)

#### NPulseLTZoneThreshold

LT-зона с пороговым механизмом для долгосрочной потенциации.

**Основные свойства:**
- `Threshold` - порог активации (обычно 1e-5)
- `TimeConstant` - временная константа (обычно 0.005)
- `PulseAmplitude` - амплитуда импульса
- `PulseLength` - длительность импульса

**Использование:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

**См. также:** [NPulseLTZoneThreshold](Components/NPulseLTZoneThreshold.md), [NPLTZone](Components/NPLTZone.md)

### Генераторы

#### NPulseGeneratorTransit

Генератор импульсов с транзитным режимом (часто используется как входной сигнал).

**Основные свойства:**
- `Frequency` - частота генерации импульсов
- `Amplitude` - амплитуда импульса
- `PulseLength` - длительность импульса

**Использование:** `Bin/Configs/User/CognitiveNavigation/` (используется для входных сигналов Forward, Back, Left, Right и т.д.)

**См. также:** [NPulseGeneratorTransit](Components/NPulseGeneratorTransit.md), [NPulseGenerator](Components/NPulseGenerator.md)

#### NPGenerator

Базовый генератор импульсов.

**Основные свойства:**
- `Frequency` - частота генерации
- `Amplitude` - амплитуда
- `PulseLength` - длительность импульса
- `AvgInterval` - средний интервал между импульсами

**Использование:** `Bin/Configs/!OldConfigs/NM-Neurons/`

**См. также:** [NPGenerator](Components/NPGenerator.md)

### Классификаторы

#### NSpikeClassifier

Классификатор на основе паттернов спайков.

**Основные свойства:**
- `InputNeurons` - вектор входных нейронов
- `OutputClasses` - количество классов для классификации
- `ClassLabel` - текущая метка класса (результат классификации)

**Использование:** `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`

**См. также:** [NSpikeClassifier](Components/NSpikeClassifier.md)

#### NClassifier

Базовый классификатор на основе импульсных нейросетей.

**Основные свойства:**
- `InputNeurons` - входные нейроны
- `OutputClasses` - количество классов

**См. также:** [NClassifier](Components/NClassifier.md)

### Сети и модели

#### NNet

Базовый контейнер для организации нейронов и синапсов в сеть.

**Использование:** Корневой компонент в некоторых конфигурационных проектах.

**См. также:** [NNet](Components/NNet.md)

#### NModel

Расширенная модель импульсной нейронной сети с дополнительными возможностями.

**Использование:** Корневой компонент в большинстве конфигурационных проектов (`Model_*.xml`).

**См. также:** [NModel](Components/NModel.md)

### См. также

- Исходный код: `Libraries/Nmsdk-PulseLib/Core/`
- [Component-Catalog.md](Component-Catalog.md) - полный каталог компонентов
- [Config-Overview.md](Config-Overview.md) - описание конфигурационных проектов
- [Scientific-Background.md](Scientific-Background.md) - научный фон и связанные публикации

---

## EN

### Main Classes

#### PulseLibrary

Main library class, inherits from `ULibrary`. Automatically registers all library components on initialization.

### Neurons

#### NPulseNeuronIzhikevich

Izhikevich model neuron — one of the most commonly used models for simulating various neuron types.

**Main properties (often configured in configs):**
- `A` (double) - membrane recovery parameter (usually 0.02)
- `B` (double) - recovery variable sensitivity (usually 0.2)
- `C` (double) - post-spike potential value (usually -65.0)
- `D` (double) - recovery variable increment after spike (usually 8.0)
- `V` (double) - membrane potential
- `U` (double) - recovery variable

**Usage:** `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`, `Bin/Configs/User/CognitiveNavigation/`

**See also:** [NPulseNeuronIzhikevich](Components/NPulseNeuronIzhikevich.md)

#### NIntegrateAndFireNeuron

Classic integrate-and-fire spiking neuron model.

**Parameters (ptPubParameter):**
- `C` (double) — model parameter (capacitance/time constant)
- `U0` (double) — initial potential
- `Upr` (double) — firing threshold

**Inputs:** `I` (MDMatrix<double>) — input current.

**Outputs:** `U` (MDMatrix<double>) — membrane potential (output).

**See also:** [NIntegrateAndFireNeuron](Components/NIntegrateAndFireNeuron.md)

#### NSPNeuronGen

Configurable neuron with adjustable structure (soma, dendrites, LT-zones).

**Main properties (from configs):**
- `MembraneClassName` - membrane class (e.g., "NPMembraneBio")
- `LTZoneClassName` - LT-zone class (e.g., "NPulseLTZoneThreshold")
- `ExcGeneratorClassName` - excitatory current generator class
- `InhGeneratorClassName` - inhibitory current generator class
- `NumSomaMembraneParts` - number of soma parts
- `NumDendriteMembranePartsVec` - vector of dendrite part counts

**Usage:** `Bin/Configs/User/CognitiveNavigation/` (used as `NSPNeuronGen`)

**See also:** [NSPNeuron](Components/NSPNeuron.md)

### Synapses

#### NSynapseStdp

Synapse with STDP (Spike-Timing Dependent Plasticity) learning — the most common plasticity mechanism.

**STDP parameters (ptPubParameter):**
- `XModCoeff`, `YModCoeff` — modification coefficients for post- and presynaptic activity
- `APlus`, `AMinus` — LTP/LTD amplitudes (usually ~0.01–0.1)
- `XTau`, `YTau` — time constants for X/Y (usually ~0.02)

**State (ptPubState):** `XAvg`, `YAvg` — average activities; `XYDiff` — influence difference.

**Inputs/outputs:** `PsActivityInput` (MDMatrix<double>) — postsynaptic neuron activity; `StdpInfluence` (MDMatrix<double>) — STDP influence on weight. From base class: `Weight`, `Input`, `Output`.

**Usage:** `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/SpikeSamples/STDP/`, training experiments

**See also:** [NSynapseStdp](Components/NSynapseStdp.md), [NSynapseTrainerStdp](Components/NSynapseTrainerStdp.md)

#### NPSynapseBio

Bio-inspired synapse modeling neurotransmitter secretion and dissociation.

**Main properties (from configs):**
- `Weight` - synapse weight
- `PulseAmplitude` - pulse amplitude (usually 1.0)
- `SecretionTC` - secretion time constant (usually 0.001)
- `DissociationTC` - dissociation time constant (usually 0.005)
- `Resistance` - synapse resistance (usually 86000000)
- `Type` - synapse type (-1 for excitatory, 1 for inhibitory)

**Usage:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

**See also:** [NPSynapseBio](Components/NPSynapseBio.md)

### Channels and Membranes

#### NPExcChannelBio / NPInhChannelBio

Bio-inspired channels for excitatory and inhibitory currents.

**Main properties:**
- `Resistance` - channel resistance (usually 10000000)
- `RestingResistance` - resting resistance
- `FBResistance` - feedback resistance
- `Capacity` - capacitance (usually 1e-9)
- `Type` - channel type (-1 for excitatory, 1 for inhibitory)

**Usage:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

**See also:** [NPExcChannelBio](Components/NPExcChannelBio.md), [NPInhChannelBio](Components/NPInhChannelBio.md)

#### NPMembraneBio

Bio-inspired membrane with excitatory and inhibitory channels.

**Main properties:**
- `ExcChannelClassName` - excitatory channel class
- `InhChannelClassName` - inhibitory channel class
- `SynapseClassName` - synapse class
- `NumExcitatorySynapses` - number of excitatory synapses
- `NumInhibitorySynapses` - number of inhibitory synapses
- `FeedbackGain` - feedback gain

**Usage:** `Bin/Configs/User/CognitiveNavigation/`

**See also:** [NPMembraneBio](Components/NPMembraneBio.md)

### LT-Zones (Long-Term plasticity)

#### NPulseLTZoneThreshold

LT-zone with threshold mechanism for long-term potentiation.

**Main properties:**
- `Threshold` - activation threshold (usually 1e-5)
- `TimeConstant` - time constant (usually 0.005)
- `PulseAmplitude` - pulse amplitude
- `PulseLength` - pulse duration

**Usage:** `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`

**See also:** [NPulseLTZoneThreshold](Components/NPulseLTZoneThreshold.md), [NPLTZone](Components/NPLTZone.md)

### Generators

#### NPulseGeneratorTransit

Pulse generator with transit mode (often used as input signal).

**Main properties:**
- `Frequency` - pulse generation frequency
- `Amplitude` - pulse amplitude
- `PulseLength` - pulse duration

**Usage:** `Bin/Configs/User/CognitiveNavigation/` (used for input signals Forward, Back, Left, Right, etc.)

**See also:** [NPulseGeneratorTransit](Components/NPulseGeneratorTransit.md), [NPulseGenerator](Components/NPulseGenerator.md)

#### NPGenerator

Base pulse generator.

**Main properties:**
- `Frequency` - generation frequency
- `Amplitude` - amplitude
- `PulseLength` - pulse duration
- `AvgInterval` - average interval between pulses

**Usage:** `Bin/Configs/!OldConfigs/NM-Neurons/`

**See also:** [NPGenerator](Components/NPGenerator.md)

### Classifiers

#### NSpikeClassifier

Classifier based on spike patterns.

**Main properties:**
- `InputNeurons` - input neuron vector
- `OutputClasses` - number of classification classes
- `ClassLabel` - current class label (classification result)

**Usage:** `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`

**See also:** [NSpikeClassifier](Components/NSpikeClassifier.md)

#### NClassifier

Base classifier for spiking neural networks.

**Main properties:**
- `InputNeurons` - input neurons
- `OutputClasses` - number of classes

**See also:** [NClassifier](Components/NClassifier.md)

### Networks and Models

#### NNet

Base container for organizing neurons and synapses into a network.

**Usage:** Root component in some configuration projects.

**See also:** [NNet](Components/NNet.md)

#### NModel

Extended spiking neural network model with additional capabilities.

**Usage:** Root component in most configuration projects (`Model_*.xml`).

**See also:** [NModel](Components/NModel.md)

### See Also

- Source code: `Libraries/Nmsdk-PulseLib/Core/`
- [Component-Catalog.md](Component-Catalog.md) - complete component catalog
- [Config-Overview.md](Config-Overview.md) - configuration project description
- [Scientific-Background.md](Scientific-Background.md) - scientific background and related publications
