# Обзор конфигурационных проектов Nmsdk-PulseLib

## RU

### Назначение

Конфигурационные проекты в директории `Bin/Configs` представляют собой готовые к запуску эксперименты с импульсными нейронными сетями. Каждый проект содержит полное описание структуры сети, параметров компонентов и настроек интерфейса для визуализации результатов.

### Структура конфигурационного проекта

Каждый проект состоит из следующих файлов:

#### 1. `project.ini`

Метаданные проекта и настройки движка:

```ini
[Project]
Name=Имя проекта
Description=Описание проекта

[Engine]
TimeStep=2000          # Шаг моделирования в микросекундах
Libraries=PulseLib     # Используемые библиотеки
```

#### 2. `Interface.xml`

Настройка графического интерфейса и наблюдаемых величин:

- Графики (watch) для визуализации сигналов компонентов
- Виджеты для управления параметрами
- Панели для отображения статистики

Пример структуры:
```xml
<UGEngineControllWidget.UWatch.tab_1>
    <graph_0>
        <serie_0>
            <SerieNameComponent>Forward</SerieNameComponent>
            <SerieNameProperty>Output</SerieNameProperty>
        </serie_0>
    </graph_0>
</UGEngineControllWidget.UWatch.tab_1>
```

#### 3. `Model_*.xml`

Структура сети: какие компоненты создаются и как они связаны.

Основные элементы:
- Корневой компонент (`Model` класса `NModel` или `NNet`)
- Иерархия компонентов (нейроны, синапсы, каналы, генераторы)
- Связи между компонентами (`Links`)

Пример:
```xml
<Model Class="NModel">
    <Components>
        <Neuron1 Class="NPulseNeuronIzhikevich">
            <Parameters>
                <A>0.02</A>
                <B>0.2</B>
                <!-- ... -->
            </Parameters>
        </Neuron1>
    </Components>
    <Links>
        <elem>
            <Item>Generator1.Output</Item>
            <Connector>Neuron1.Input</Connector>
        </elem>
    </Links>
</Model>
```

#### 4. `Parameters_*.xml`

Численные значения параметров компонентов. Структура аналогична `Model_*.xml`, но содержит только секции `<Parameters>` для каждого компонента.

Пример:
```xml
<Neuron1 Class="NPulseNeuronIzhikevich">
    <Parameters>
        <A Type="double">0.02</A>
        <B Type="double">0.2</B>
        <C Type="double">-65.0</C>
        <D Type="double">8.0</D>
    </Parameters>
</Neuron1>
```

#### 5. `Description.rtf` (опционально)

Текстовое описание проекта, его назначения и архитектуры сети.

### Типы конфигурационных проектов

#### Эксперименты с нейронами

**Расположение:** `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`

**Примеры:**
- `NM-PN-01-HebbNeuron-1M1St1In1/` — нейрон с обучением по правилу Хебба
- `NM-PN-07-LtmNeuron-1M1St3In3/` — нейрон с долгосрочной памятью
- `NM-AN-01-NSAfferentNeuron/` — афферентный нейрон

**Типичные компоненты:**
- `NPulseNeuronIzhikevich`, `NPHebbNeuron`, `NSPNeuronGen`
- `NPGenerator` — генераторы входных сигналов
- `NPLTZone` — зоны долгосрочной пластичности

#### Эксперименты по обучению (STDP)

**Расположение:** `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`

**Примеры:**
- `STDP-Simple-01/` — простой эксперимент по STDP-пластичности
- `SpikeAnsTrainer/` — обучение афферентных нейронов

**Типичные компоненты:**
- `NSynapseStdp`, `NPulseSynapseStdp` — синапсы с STDP
- `NSynapseTrainerStdp*` — различные варианты тренеров STDP
- `NPulseNeuronIzhikevich`, `NIntegrateAndFireNeuron`

#### Классификация

**Расположение:** `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`

**Примеры:**
- `SpikeANPA3/` — классификация паттернов спайков
- `SpikeIrisClassifier/` — классификация набора данных Iris

**Типичные компоненты:**
- `NSpikeClassifier`, `NClassifier` — классификаторы
- `NPattern`, `NDataset` — паттерны и датасеты для обучения
- Входные слои нейронов для кодирования данных

#### Когнитивная навигация

**Расположение:** `Bin/Configs/User/CognitiveNavigation/`

**Примеры:**
- `Ivan_VKR_3-4_13/` — сеть для пространственной навигации с логическими операциями

**Типичные компоненты:**
- `NSPNeuronGen` — нейроны с настраиваемой структурой
- `NPMembraneBio`, `NPExcChannelBio`, `NPInhChannelBio` — биоинспирированные компоненты
- `NPulseGeneratorTransit` — генераторы входных сигналов (Forward, Back, Left, Right, Danger)
- `NPulseLTZoneThreshold` — LT-зоны для долгосрочной памяти

**Особенности:**
- Сложная иерархическая структура (сома, дендриты, LT-зоны)
- Логические операции (NOT, AND, OR) через комбинации нейронов
- Обратные связи для формирования памяти

#### Управление движением

**Расположение:** `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/OldExperiments/SimplestMotionControl/`

**Типичные компоненты:**
- `NMuscle`, `NEyeMuscle` — эффекторы
- `NMotoneuron` — моторные нейроны
- `NAfferentNeuron`, `NSAfferentNeuron` — афферентные нейроны
- `NReceptor` — рецепторы для сенсорных сигналов

### Сопоставление XML-элементов с компонентами

#### Нейрон Ижикевича

```xml
<Neuron1 Class="NPulseNeuronIzhikevich">
    <Parameters>
        <A>0.02</A>        <!-- Параметр восстановления мембраны -->
        <B>0.2</B>         <!-- Чувствительность переменной восстановления -->
        <C>-65.0</C>       <!-- Потенциал после спайка -->
        <D>8.0</D>         <!-- Приращение переменной восстановления -->
    </Parameters>
</Neuron1>
```

#### Биоинспирированный синапс

```xml
<ExcSynapse1 Class="NPSynapseBio">
    <Parameters>
        <Weight>1.0</Weight>                    <!-- Вес синапса -->
        <PulseAmplitude>1.0</PulseAmplitude>     <!-- Амплитуда импульса -->
        <SecretionTC>0.001</SecretionTC>         <!-- Временная константа секреции -->
        <DissociationTC>0.005</DissociationTC>   <!-- Временная константа диссоциации -->
        <Resistance>86000000</Resistance>         <!-- Сопротивление -->
        <Type>-1</Type>                          <!-- -1 для возбуждающего -->
    </Parameters>
</ExcSynapse1>
```

#### Биоинспирированный канал

```xml
<ExcChannel Class="NPExcChannelBio">
    <Parameters>
        <Resistance>10000000</Resistance>        <!-- Сопротивление канала -->
        <RestingResistance>10000000</RestingResistance>
        <Capacity>1e-9</Capacity>                <!-- Емкость -->
        <Type>-1</Type>                         <!-- -1 для возбуждающего -->
    </Parameters>
</ExcChannel>
```

#### LT-зона

```xml
<LTZone Class="NPulseLTZoneThreshold">
    <Parameters>
        <Threshold>1e-5</Threshold>              <!-- Порог активации -->
        <TimeConstant>0.005</TimeConstant>       <!-- Временная константа -->
        <PulseAmplitude>1.0</PulseAmplitude>
        <PulseLength>0.001</PulseLength>
    </Parameters>
</LTZone>
```

### Рекомендации по изменению конфигураций

#### Настройка параметров нейронов

**Модель Ижикевича:**
- `A` (0.01-0.1): меньшие значения дают более медленное восстановление
- `B` (0.1-0.3): влияет на адаптацию частоты спайков
- `C` (-70 до -50): потенциал после спайка, влияет на рефрактерность
- `D` (2-20): определяет тип нейрона (регулярно спайкующий, быстро спайкующий и т.д.)

**Рекомендации:**
- Для регулярно спайкующих нейронов: A=0.02, B=0.2, C=-65, D=8
- Для быстро спайкующих: A=0.1, B=0.2, C=-65, D=2
- Для медленно спайкующих: A=0.02, B=0.2, C=-65, D=-10

#### Настройка синапсов

**Вес синапса (`Weight`):**
- Обычно в диапазоне 0.1-10.0
- Для возбуждающих синапсов: положительные значения
- Для тормозных синапсов: отрицательные значения или отдельный параметр `Type`

**Временные константы:**
- `SecretionTC` (0.0001-0.01): время секреции нейромедиатора
- `DissociationTC` (0.001-0.1): время диссоциации, влияет на длительность эффекта

#### Настройка каналов

**Сопротивление (`Resistance`):**
- Обычно 1e6 - 1e8 Ом
- Меньшее сопротивление → больший ток → более сильное влияние на потенциал

**Емкость (`Capacity`):**
- Обычно 1e-9 - 1e-8 Фарад
- Влияет на временную динамику изменения потенциала

#### Настройка LT-зон

**Порог (`Threshold`):**
- Обычно 1e-6 - 1e-4
- Определяет минимальную активность для активации долгосрочной пластичности

**Временная константа (`TimeConstant`):**
- Обычно 0.001-0.1 секунды
- Определяет скорость накопления и затухания активности

### Работа с несколькими версиями модели

Многие проекты содержат несколько версий:
- `Model_00.xml`, `Model_01.xml`, ... — различные варианты структуры сети
- `Parameters_00.xml`, `Parameters_01.xml`, ... — соответствующие наборы параметров

Это позволяет:
- Отслеживать эволюцию эксперимента
- Сравнивать различные архитектуры
- Сохранять рабочие конфигурации

### См. также

- [Config-Templates.md](Config-Templates.md) - шаблоны типовых экспериментов
- [Usage-Examples.md](Usage-Examples.md) - примеры использования компонентов
- [API-Overview.md](API-Overview.md) - описание API компонентов

---

## EN

### Purpose

Configuration projects in the `Bin/Configs` directory are ready-to-run experiments with spiking neural networks. Each project contains a complete description of the network structure, component parameters, and interface settings for visualizing results.

### Configuration Project Structure

Each project consists of the following files:

#### 1. `project.ini`

Project metadata and engine settings.

#### 2. `Interface.xml`

Graphical interface settings and observable quantities (graphs, widgets, panels).

#### 3. `Model_*.xml`

Network structure: which components are created and how they are connected.

#### 4. `Parameters_*.xml`

Numerical values of component parameters.

#### 5. `Description.rtf` (optional)

Text description of the project, its purpose, and network architecture.

### Configuration Project Types

#### Neuron Experiments

**Location:** `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`

**Typical components:**
- `NPulseNeuronIzhikevich`, `NPHebbNeuron`, `NSPNeuronGen`
- `NPGenerator` — input signal generators
- `NPLTZone` — long-term plasticity zones

#### Learning Experiments (STDP)

**Location:** `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`

**Typical components:**
- `NSynapseStdp`, `NPulseSynapseStdp` — STDP synapses
- `NSynapseTrainerStdp*` — various STDP trainer variants

#### Classification

**Location:** `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`

**Typical components:**
- `NSpikeClassifier`, `NClassifier` — classifiers
- `NPattern`, `NDataset` — patterns and datasets for training

#### Cognitive Navigation

**Location:** `Bin/Configs/User/CognitiveNavigation/`

**Typical components:**
- `NSPNeuronGen` — neurons with configurable structure
- `NPMembraneBio`, `NPExcChannelBio`, `NPInhChannelBio` — bio-inspired components
- `NPulseGeneratorTransit` — input signal generators

#### Motion Control

**Location:** `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`

**Typical components:**
- `NMuscle`, `NEyeMuscle` — effectors
- `NMotoneuron` — motor neurons
- `NAfferentNeuron` — afferent neurons

### See Also

- [Config-Templates.md](Config-Templates.md) - templates for typical experiments
- [Usage-Examples.md](Usage-Examples.md) - component usage examples
- [API-Overview.md](API-Overview.md) - component API description
