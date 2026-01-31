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

Main library class, inherits from `ULibrary`.

#### NPulseNeuron

Base spiking neuron.

**Main Properties:**
- `MembranePotential` - membrane potential
- `Threshold` - firing threshold
- `OutputSpike` - output spike

#### NPulseNeuronIzhikevich

Izhikevich model neuron.

**Main Properties:**
- `A`, `B`, `C`, `D` - model parameters
- `V` - membrane potential
- `U` - recovery variable

#### NPulseSynapse

Base synapse.

**Main Properties:**
- `PreNeuron` - presynaptic neuron
- `PostNeuron` - postsynaptic neuron
- `Weight` - synapse weight

#### NSynapseStdp

Synapse with STDP learning.

**Main Properties:** `XModCoeff`, `YModCoeff`, `APlus`, `AMinus`, `XTau`, `YTau` (STDP parameters); `XAvg`, `YAvg`, `XYDiff` (state); `PsActivityInput`, `StdpInfluence` (input/output); inherited `Weight`, `Input`, `Output`.

#### NSpikeClassifier

Spike-based classifier.

**Main Properties:**
- `InputNeurons` - input neurons
- `OutputClasses` - number of classes
- `ClassLabel` - class label

### See Also

- Source code: `Libraries/Nmsdk-PulseLib/Core/`
