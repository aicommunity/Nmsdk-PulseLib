# Шаблоны типовых экспериментов Nmsdk-PulseLib

## RU

### Назначение

Этот документ описывает шаблоны (паттерны) типовых экспериментов с импульсными нейронными сетями, которые можно использовать как основу для создания новых конфигурационных проектов.

### Шаблон 1: Простая сеть с двумя нейронами и STDP

**Назначение:** Демонстрация базового механизма STDP-обучения между двумя нейронами.

**Структура:**
- Два нейрона Ижикевича (`NPulseNeuronIzhikevich`)
- Один STDP-синапс (`NSynapseStdp`) или `NPulseSynapseStdp`
- Генератор входных сигналов (`NPulseGenerator`)

**Пример конфигурации:** `Bin/Configs/!OldConfigs/STDP-Simple-01/`

**Ключевые параметры:**
```xml
<!-- Нейрон 1 -->
<Neuron1 Class="NPulseNeuronIzhikevich">
    <Parameters>
        <A>0.02</A>
        <B>0.2</B>
        <C>-65.0</C>
        <D>8.0</D>
    </Parameters>
</Neuron1>

<!-- Нейрон 2 -->
<Neuron2 Class="NPulseNeuronIzhikevich">
    <Parameters>
        <A>0.02</A>
        <B>0.2</B>
        <C>-65.0</C>
        <D>8.0</D>
    </Parameters>
</Neuron2>

<!-- STDP-синапс -->
<Synapse1 Class="NSynapseStdp">
    <Parameters>
        <PreNeuron>Neuron1</PreNeuron>
        <PostNeuron>Neuron2</PostNeuron>
        <Weight>0.5</Weight>
        <LearningRate>0.01</LearningRate>
        <TauPlus>0.02</TauPlus>
        <TauMinus>0.02</TauMinus>
    </Parameters>
</Synapse1>

<!-- Генератор входных сигналов -->
<InputGenerator Class="NPulseGenerator">
    <Parameters>
        <Frequency>10.0</Frequency>
        <Amplitude>1.0</Amplitude>
    </Parameters>
</InputGenerator>
```

**Связи:**
- `InputGenerator.Output` → `Neuron1.Input`
- `Neuron1.Output` → `Synapse1.Input`
- `Synapse1.Output` → `Neuron2.Input`

### Шаблон 2: Классификатор паттернов спайков

**Назначение:** Обучение сети для классификации различных паттернов входных спайков.

**Структура:**
- Входной слой нейронов (кодирование входных данных)
- Скрытый слой (опционально)
- Классификатор (`NSpikeClassifier` или `NClassifier`)
- Генераторы паттернов (`NPattern`)

**Пример конфигурации:** `Bin/Configs/!OldConfigs/SpikeANPA3/`, `Bin/Configs/!OldConfigs/SpikeClassifier/`

**Ключевые компоненты:**
```xml
<!-- Входной слой нейронов -->
<InputLayer Class="NNeuronsLayer">
    <Parameters>
        <NeuronsClassName>NPulseNeuronIzhikevich</NeuronsClassName>
        <LayerWidth>10</LayerWidth>
        <LayerHeight>1</LayerHeight>
    </Parameters>
</InputLayer>

<!-- Классификатор -->
<Classifier Class="NSpikeClassifier">
    <Parameters>
        <InputNeurons>InputLayer.Neurons</InputNeurons>
        <OutputClasses>5</OutputClasses>
    </Parameters>
</Classifier>

<!-- Паттерны для обучения -->
<Pattern1 Class="NPattern">
    <Parameters>
        <Label>0</Label>
        <!-- Данные паттерна -->
    </Parameters>
</Pattern1>
```

### Шаблон 3: Биоинспирированный нейрон с LT-зоной

**Назначение:** Моделирование нейрона с биоинспирированной структурой (сома, дендриты, LT-зона) для долгосрочной пластичности.

**Структура:**
- Нейрон с настраиваемой структурой (`NSPNeuronGen`)
- Биоинспирированные мембраны (`NPMembraneBio`)
- Возбуждающие и тормозные каналы (`NPExcChannelBio`, `NPInhChannelBio`)
- Биоинспирированные синапсы (`NPSynapseBio`)
- LT-зона (`NPulseLTZoneThreshold`)

**Пример конфигурации:** `Bin/Configs/User/CognitiveNavigation/Ivan_VKR_3-4_13/`

**Ключевые компоненты:**
```xml
<!-- Нейрон с настраиваемой структурой -->
<Neuron1 Class="NSPNeuronGen">
    <Parameters>
        <MembraneClassName>NPMembraneBio</MembraneClassName>
        <LTZoneClassName>NPulseLTZoneThreshold</LTZoneClassName>
        <ExcGeneratorClassName>NPNeuronPosCGenerator</ExcGeneratorClassName>
        <InhGeneratorClassName>NPNeuronNegCGenerator</InhGeneratorClassName>
        <NumSomaMembraneParts>1</NumSomaMembraneParts>
        <NumDendriteMembranePartsVec>1</NumDendriteMembranePartsVec>
    </Parameters>
    <Components>
        <!-- Сома -->
        <Soma1 Class="NPMembraneBio">
            <Parameters>
                <ExcChannelClassName>NPExcChannelBio</ExcChannelClassName>
                <InhChannelClassName>NPInhChannelBio</InhChannelClassName>
                <SynapseClassName>NPSynapseBio</SynapseClassName>
                <NumExcitatorySynapses>1</NumExcitatorySynapses>
                <NumInhibitorySynapses>1</NumInhibitorySynapses>
            </Parameters>
            <Components>
                <!-- Возбуждающий синапс -->
                <ExcSynapse1 Class="NPSynapseBio">
                    <Parameters>
                        <Weight>1.0</Weight>
                        <PulseAmplitude>1.0</PulseAmplitude>
                        <SecretionTC>0.001</SecretionTC>
                        <DissociationTC>0.005</DissociationTC>
                        <Resistance>86000000</Resistance>
                        <Type>-1</Type>
                    </Parameters>
                </ExcSynapse1>
                <!-- Возбуждающий канал -->
                <ExcChannel Class="NPExcChannelBio">
                    <Parameters>
                        <Resistance>10000000</Resistance>
                        <Capacity>1e-9</Capacity>
                        <Type>-1</Type>
                    </Parameters>
                </ExcChannel>
            </Components>
        </Soma1>
        <!-- LT-зона -->
        <LTZone Class="NPulseLTZoneThreshold">
            <Parameters>
                <Threshold>1e-5</Threshold>
                <TimeConstant>0.005</TimeConstant>
                <PulseAmplitude>1.0</PulseAmplitude>
            </Parameters>
        </LTZone>
    </Components>
</Neuron1>
```

### Шаблон 4: Логическая операция (NOT, AND, OR)

**Назначение:** Реализация логических операций через комбинации нейронов и генераторов.

**Структура:**
- Генераторы входных сигналов (`NPulseGeneratorTransit`)
- Нейроны с инвертирующими связями (`NSPNeuronGen`)
- Комбинирующие нейроны для AND/OR операций

**Пример конфигурации:** `Bin/Configs/User/CognitiveNavigation/Ivan_VKR_3-4_13/`

**Логика NOT:**
- Генератор входного сигнала
- Нейрон с возбуждающим входом на дендрит и тормозным на сому
- Выход нейрона инвертирует входной сигнал

**Логика AND:**
- Несколько входных генераторов
- Нейрон с несколькими возбуждающими входами
- LT-зона для накопления активности
- Нейрон срабатывает только при одновременной активации всех входов

**Логика OR:**
- Несколько входных генераторов
- Нейрон с несколькими возбуждающими входами
- Нейрон срабатывает при активации любого из входов

### Шаблон 5: Афферентный нейрон с рецептором

**Назначение:** Моделирование сенсорного входа через афферентный нейрон и рецептор.

**Структура:**
- Рецептор (`NReceptor`) для приема внешних сигналов
- Афферентный нейрон (`NAfferentNeuron` или `NSAfferentNeuron`)
- Связь рецептора с нейроном

**Пример конфигурации:** `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`, `Bin/Configs/!OldConfigs/NReceptor/`

**Ключевые компоненты:**
```xml
<!-- Рецептор -->
<Receptor1 Class="NReceptor">
    <Parameters>
        <!-- Параметры рецептора -->
    </Parameters>
</Receptor1>

<!-- Афферентный нейрон -->
<AfferentNeuron1 Class="NSAfferentNeuron">
    <Parameters>
        <!-- Параметры нейрона -->
    </Parameters>
</AfferentNeuron1>
```

**Связи:**
- `Receptor1.Output` → `AfferentNeuron1.Input`

### Шаблон 6: Управление мышцей через моторный нейрон

**Назначение:** Моделирование управления эффектором (мышцей) через моторный нейрон.

**Структура:**
- Моторный нейрон (`NMotoneuron` или `NNewMotoneuron`)
- Мышца (`NMuscle` или `NEyeMuscle`)
- Входные сигналы от других нейронов

**Пример конфигурации:** `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/MC-Muscles/`

**Ключевые компоненты:**
```xml
<!-- Моторный нейрон -->
<Motoneuron1 Class="NMotoneuron">
    <Parameters>
        <!-- Параметры моторного нейрона -->
    </Parameters>
</Motoneuron1>

<!-- Мышца -->
<Muscle1 Class="NMuscle">
    <Parameters>
        <!-- Параметры мышцы -->
    </Parameters>
</Muscle1>
```

**Связи:**
- `InputNeuron.Output` → `Motoneuron1.Input`
- `Motoneuron1.Output` → `Muscle1.Input`

### Шаблон 7: Сеть с обратными связями для памяти

**Назначение:** Создание сети с обратными связями для формирования кратковременной и долгосрочной памяти.

**Структура:**
- Основные нейроны с LT-зонами
- Обратные связи от выходов к входам
- Генераторы для инициализации активности

**Пример конфигурации:** `Bin/Configs/User/CognitiveNavigation/Ivan_VKR_3-4_13/`

**Особенности:**
- LT-зоны накапливают активность и поддерживают состояние
- Обратные связи создают рекуррентную динамику
- Генераторы могут инициировать или сбрасывать состояние

### Рекомендации по созданию новых экспериментов

1. **Начните с простого:** Используйте шаблон 1 (два нейрона + STDP) для понимания базовых механизмов.

2. **Используйте существующие конфигурации:** Изучите похожие проекты в `Bin/Configs` и адаптируйте их под свои задачи.

3. **Итеративная разработка:** Создавайте несколько версий модели (`Model_00.xml`, `Model_01.xml`, ...) для отслеживания изменений.

4. **Документируйте параметры:** Записывайте, какие параметры вы изменяли и как это влияло на поведение сети.

5. **Визуализация:** Настройте графики в `Interface.xml` для наблюдения за ключевыми величинами (потенциалы, спайки, веса синапсов).

6. **Тестирование:** Проверяйте работу сети на простых входных сигналах перед переходом к сложным задачам.

### См. также

- [Config-Overview.md](Config-Overview.md) - обзор структуры конфигураций
- [Usage-Examples.md](Usage-Examples.md) - примеры использования компонентов
- [API-Overview.md](API-Overview.md) - описание API компонентов

---

## EN

### Purpose

This document describes templates (patterns) for typical spiking neural network experiments that can be used as a basis for creating new configuration projects.

### Template 1: Simple Network with Two Neurons and STDP

**Purpose:** Demonstration of basic STDP learning mechanism between two neurons.

**Structure:**
- Two Izhikevich neurons (`NPulseNeuronIzhikevich`)
- One STDP synapse (`NSynapseStdp`)
- Input signal generator (`NPulseGenerator`)

### Template 2: Spike Pattern Classifier

**Purpose:** Training a network to classify different input spike patterns.

**Structure:**
- Input neuron layer (data encoding)
- Hidden layer (optional)
- Classifier (`NSpikeClassifier`)
- Pattern generators (`NPattern`)

### Template 3: Bio-Inspired Neuron with LT-Zone

**Purpose:** Modeling a neuron with bio-inspired structure (soma, dendrites, LT-zone) for long-term plasticity.

**Structure:**
- Neuron with configurable structure (`NSPNeuronGen`)
- Bio-inspired membranes (`NPMembraneBio`)
- Excitatory and inhibitory channels
- LT-zone (`NPulseLTZoneThreshold`)

### Template 4: Logical Operations (NOT, AND, OR)

**Purpose:** Implementation of logical operations through neuron and generator combinations.

### Template 5: Afferent Neuron with Receptor

**Purpose:** Modeling sensory input through afferent neuron and receptor.

### Template 6: Muscle Control via Motor Neuron

**Purpose:** Modeling effector (muscle) control through motor neuron.

### Template 7: Network with Feedback Connections for Memory

**Purpose:** Creating a network with feedback connections for short-term and long-term memory formation.

### See Also

- [Config-Overview.md](Config-Overview.md) - configuration structure overview
- [Usage-Examples.md](Usage-Examples.md) - component usage examples
- [API-Overview.md](API-Overview.md) - component API description
