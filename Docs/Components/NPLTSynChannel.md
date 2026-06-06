# NPLTSynChannel — LT-синаптический канал

## RU

### Назначение

**Класс**: `NPLTSynChannel` — конфигурационный вариант синаптического импульсного канала с параметрами для LT-зоны.  
**Аббревиатуры**: `LT` — **L**ow **T**hreshold (низкопороговая зона); `Syn` — **Syn**apse (синапс).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynChannel", ...)`.  
**Storage-инстансы**: `ClassName = "NPLTSynChannel"` в `Bin/Configs/*/Model_*.xml`.

`NPLTSynChannel` является конфигурационным вариантом базового класса `NPulseSynChannel` с предустановленными параметрами для LT-зоны. Создается из `NPulseSynChannel` с настройками:
- `Capacity = 1e-8` (10 нФ) — увеличенная емкость для LT-зоны
- `RestingResistance = 1e6` (1 МОм) — сопротивление покоя для LT-зоны

Комбинирует функциональность синаптического канала (обработка синапсов с моделью медиатора) с параметрами, оптимизированными для LT-зон.

**Использование:** Моделирование синаптических каналов в LT-зонах, эксперименты с LT-пластичностью и синаптической передачей

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NPulseSynChannel
    NPulseSynChannel <|.. NPLTSynChannel : configuration variant
    NPLTSynChannel ..> NPulseLTZoneCommon : используется в
    class NPulseSynChannel {
        +PulseAmplitude : double
        +SecretionTC : double
        +DissociationTC : double
        +Capacity : double
        +RestingResistance : double
    }
    class NPLTSynChannel {
        +Capacity : double = 1e-8
        +RestingResistance : double = 1e6
    }
```

**Иерархия наследования:**
- `NPulseChannelCommon` — общий импульсный канал
- `NPulseChannel` — базовый импульсный канал
- `NPulseSynChannel` — синаптический импульсный канал
- `NPLTSynChannel` — конфигурационный вариант для LT-зоны

**Связи:**
- Используется в LT-зонах для обработки синаптических входов

### Свойства

`NPLTSynChannel` использует все свойства базового класса `NPulseSynChannel` с предустановленными значениями:

**Параметры:**
- `Capacity = 1e-8` (10 нФ) — увеличенная емкость для LT-зоны
- `RestingResistance = 1e6` (1 МОм) — сопротивление покоя для LT-зоны

**Остальные параметры по умолчанию:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 мс)
- `DissociationTC = 0.01` (10 мс)
- `SynapseResistance = 1.0e8` (100 МОм)
- `Resistance = 1.0e7` (10 МОм)
- `FBResistance = 1.0e8` (100 МОм)
- `Type = 0` (нейтральный)

**Особенности:**
- Комбинирует функциональность синаптического канала с параметрами LT-зоны
- Обрабатывает синапсы с моделью медиатора
- Использует увеличенную емкость и уменьшенное сопротивление покоя для LT-зон

### Методы

`NPLTSynChannel` использует все методы базового класса `NPulseSynChannel`.

### Примеры использования

#### Пример 1: Создание канала в коде C++

```cpp
// Создание LT-синаптического канала
auto channel = storage->CreateComponent("NPLTSynChannel");
channel->SetName("LTSynChannel");

// Инициализация (использует параметры для LT-зоны)
channel->Default();

// Использование
channel->Build();
```

#### Пример 2: Конфигурация XML

```xml
<Channel1 Class="NPLTSynChannel">
    <Parameters>
        <Type>0</Type>
        <Capacity>1e-8</Capacity>
        <Resistance>1.0e7</Resistance>
        <FBResistance>1.0e8</FBResistance>
        <RestingResistance>1e6</RestingResistance>
        <PulseAmplitude>1.0</PulseAmplitude>
        <SecretionTC>0.001</SecretionTC>
        <DissociationTC>0.01</DissociationTC>
        <SynapseResistance>1.0e8</SynapseResistance>
    </Parameters>
</Channel1>
```

### Использование в конфигурациях

`NPLTSynChannel` используется в экспериментах с синаптическими каналами в LT-зонах:

- Моделирование синаптических каналов в LT-зонах
- Эксперименты с LT-пластичностью и синаптической передачей
- Изучение влияния синаптических входов на генерацию спайков

**Особенности:**
- Комбинирует обработку синапсов с параметрами LT-зоны
- Использует модель динамики медиатора для синапсов
- Параметры оптимизированы для работы в LT-зонах

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseSynChannel`](NPulseSynChannel.md) — синаптический импульсный канал (базовый класс)
- [`NPLTChannel`](NPLTChannel.md) — LT-канал
- [`NPLTSynExcChannel`](NPLTSynExcChannel.md) — возбуждающий LT-синаптический канал
- [`NPLTSynInhChannel`](NPLTSynInhChannel.md) — тормозной LT-синаптический канал
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — общая импульсная LT-зона
- [`NPulseSynapse`](NPulseSynapse.md) — импульсный синапс
- [Architecture.md](../Architecture.md) — архитектура библиотеки
- [Scientific-Background.md](../Scientific-Background.md) — научный фон (LT-пластичность, синаптическая передача)

---

## EN

### Purpose

**Class**: `NPLTSynChannel` — configuration variant of synaptic spiking channel with parameters for LT-zone.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynChannel", ...)`.  
**Instances**: `ClassName = "NPLTSynChannel"` in `Bin/Configs/*/Model_*.xml`.

`NPLTSynChannel` is a configuration variant of the base class `NPulseSynChannel` with preset parameters for LT-zone. Created from `NPulseSynChannel` with settings:
- `Capacity = 1e-8` (10 nF) — increased capacitance for LT-zone
- `RestingResistance = 1e6` (1 MΩ) — resting resistance for LT-zone

Combines synaptic channel functionality (processing synapses with neurotransmitter model) with parameters optimized for LT-zones.

**Usage:** Modeling synaptic channels in LT-zones, experiments with LT plasticity and synaptic transmission

### UML Class Diagram

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NPulseSynChannel
    NPulseSynChannel <|.. NPLTSynChannel : configuration variant
    NPLTSynChannel ..> NPulseLTZoneCommon : used in
    class NPulseSynChannel {
        +PulseAmplitude : double
        +SecretionTC : double
        +DissociationTC : double
        +Capacity : double
        +RestingResistance : double
    }
    class NPLTSynChannel {
        +Capacity : double = 1e-8
        +RestingResistance : double = 1e6
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Channel as NPLTSynChannel
    participant Synapses as NPulseSynapse
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Channel: New() + Default()
    Storage->>Channel: Build()
    Channel->>Channel: Set Capacity = 1e-8
    Channel->>Channel: Set RestingResistance = 1e6
    Channel->>Channel: ABuild()
    Channel->>Synapses: Connect synapses
    loop Each step
        Synapses->>Channel: PreOutput signals
        Storage->>Channel: Calculate()
        Channel->>Channel: Process synapses
        Channel->>Channel: Apply mediator dynamics
        Channel->>Channel: Calculate output current
        Channel->>LTZone: Output (LT-zone potential)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetParams: Set LT-zone parameters
    SetParams --> Building: Build()
    Building --> Connecting: Connect synapses
    Connecting --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessSynapses: Process synapse inputs
    ProcessSynapses --> ApplyMediator: Apply mediator dynamics
    ApplyMediator --> CalculateOutput: Calculate output current
    CalculateOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInputs[Receive PreOutput from synapses]
    ReceiveInputs --> AggregateInputs[Aggregate synapse inputs]
    AggregateInputs --> ProcessMediator["Process mediator dynamics<br/>SecretionTC, DissociationTC"]
    ProcessMediator --> CalculateCurrent["Calculate output current<br/>from synapse outputs"]
    CalculateCurrent --> IntegrateOutput["Integrate output potential<br/>with Capacity=1e-8 and RestingResistance=1e6"]
    IntegrateOutput --> SetOutput[Set output]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseSynChannel["NPulseSynChannel Base"]
        BaseChannel[NPulseSynChannel]
    end
    
    subgraph NPLTSynChannel["NPLTSynChannel Configuration"]
        LTParams["LT-zone Parameters<br/>Capacity=1e-8, RestingResistance=1e6"]
        SynapseProcessing["Synapse Processing<br/>Mediator dynamics"]
    end
    
    subgraph External["External Components"]
        Synapses["NPulseSynapse<br/>Synapses"]
        LTZone[NPulseLTZoneCommon]
    end
    
    BaseChannel -->|configured as| NPLTSynChannel
    NPLTSynChannel -->|uses| LTParams
    Synapses -->|PreOutput| NPLTSynChannel
    NPLTSynChannel -->|Output current| LTZone
    NPLTSynChannel -->|uses| SynapseProcessing
```

### Properties

`NPLTSynChannel` uses all properties of base class `NPulseSynChannel` with preset values:

**Parameters:**
- `Capacity = 1e-8` (10 nF) — increased capacitance for LT-zone
- `RestingResistance = 1e6` (1 MΩ) — resting resistance for LT-zone

**Other default parameters:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 ms)
- `DissociationTC = 0.01` (10 ms)
- `SynapseResistance = 1.0e8` (100 MΩ)
- `Resistance = 1.0e7` (10 MΩ)
- `FBResistance = 1.0e8` (100 MΩ)
- `Type = 0` (neutral)

**Features:**
- Combines synaptic channel functionality with LT-zone parameters
- Processes synapses with mediator model
- Uses increased capacitance and decreased resting resistance for LT-zones

### Methods

`NPLTSynChannel` uses all methods of base class `NPulseSynChannel`.

### Usage in configurations

`NPLTSynChannel` is used in experiments with synaptic channels in LT-zones:

- **Synaptic channels in LT-zones**: Modeling synaptic channels in LT-zones
- **LT plasticity**: Experiments with LT plasticity and synaptic transmission
- **Spike generation**: Studying influence of synaptic inputs on spike generation

**Features:**
- Combines synapse processing with LT-zone parameters
- Uses mediator dynamics model for synapses
- Parameters optimized for work in LT-zones

**Typical parameter values:**
- **Capacity**: 1e-8 (10 nF) — increased for LT-zone
- **RestingResistance**: 1e6 (1 MΩ) — decreased for LT-zone
- **SecretionTC**: 0.001 (1 ms time constant)
- **DissociationTC**: 0.01 (10 ms time constant)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseSynChannel`](NPulseSynChannel.md) — synaptic spiking channel (base class)
- [`NPLTChannel`](NPLTChannel.md) — LT-channel
- [`NPLTSynExcChannel`](NPLTSynExcChannel.md) — excitatory LT-synaptic channel
- [`NPLTSynInhChannel`](NPLTSynInhChannel.md) — inhibitory LT-synaptic channel
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — common spiking LT-zone
- [`NPulseSynapse`](NPulseSynapse.md) — spiking synapse
- [Architecture.md](../Architecture.md) — library architecture

