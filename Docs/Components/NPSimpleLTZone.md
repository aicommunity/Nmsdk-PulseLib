# NPSimpleLTZone — простая импульсная LT-зона

## RU

### Назначение

**Класс**: `NPSimpleLTZone` — простая импульсная LT-зона с встроенным генератором импульсов.  
**Аббревиатура**: `LT` — **L**ow **T**hreshold (низкопороговая зона).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPSimpleLTZone", ...)`.  
**Storage-инстансы**: `ClassName = "NPSimpleLTZone"` в `Bin/Configs/*/Model_*.xml`.

`NPSimpleLTZone` является расширением класса `NPulseLTZone` с встроенным генератором импульсов (`NPulseGenerator`). Наследуется от `NPulseLTZone` и добавляет параметр `MaxFrequency` для ограничения максимальной частоты генерации спайков. Внутренний генератор используется для управления генерацией импульсов.

**Использование:** Простая LT-зона с встроенным генератором, ограничение максимальной частоты

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NLTZone
    NLTZone <|-- NPulseLTZoneCommon
    NPulseLTZoneCommon <|-- NPulseLTZoneThreshold
    NPulseLTZoneThreshold <|-- NPulseLTZone
    NPulseLTZone <|-- NPSimpleLTZone
    NPSimpleLTZone *-- NPulseGenerator : generator
    class NPulseLTZone {
        +TimeConstant : double
        +UseLTZIntegtation : bool
        +UseSpikeStabilizer : bool
    }
    class NPSimpleLTZone {
        +MaxFrequency : double
        -generator : NPulseGenerator
        +New() NPSimpleLTZone*
        +ADefault() bool
        +ABuild() bool
        +ACalculate2() bool
    }
```

**Иерархия наследования:**
- `UNet` — базовая сеть Rdk Framework
- `NLTZone` — базовая LT-зона
- `NPulseLTZoneCommon` — общая импульсная LT-зона
- `NPulseLTZoneThreshold` — базовая импульсная LT-зона с порогом
- `NPulseLTZone` — импульсная LT-зона (расширенная)
- `NPSimpleLTZone` — простая импульсная LT-зона

### Свойства

#### Параметры (ptPubParameter)

- **`MaxFrequency`** (double) — максимальная частота генерации спайков (Гц). Диапазон: [0; 500]. Значение по умолчанию: 100.0

**Наследуемые от NPulseLTZone:**
- `TimeConstant` — временная константа
- `UseLTZIntegtation` — использовать интегрирование LT-зоны
- `UseSpikeStabilizer` — использовать стабилизатор спайков

**Наследуемые от NPulseLTZoneCommon:**
- `Threshold`, `ThresholdOff` — пороги
- `PulseAmplitude`, `PulseLength` — параметры импульсов
- `NumChannelsInGroup` — количество каналов в группе

### Методы

- **`ACalculate2()`** → `bool` — выполняет расчет простой LT-зоны. Использует встроенный генератор для управления генерацией импульсов с учетом ограничения `MaxFrequency`.

### Примеры использования

#### Пример 1: Создание простой LT-зоны в коде C++

```cpp
// Создание простой LT-зоны
auto ltZone = storage->CreateComponent("NPSimpleLTZone");
ltZone->SetName("SimpleLTZone");

// Инициализация
ltZone->Default();

// Настройка параметров
ltZone->MaxFrequency = 100.0;  // Максимальная частота 100 Гц
ltZone->Threshold = 30.0;
ltZone->PulseAmplitude = 1.0;

// Использование
ltZone->Build();
```

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseLTZone`](NPulseLTZone.md) — импульсная LT-зона (базовый класс)
- [`NCSimpleLTZone`](NCSimpleLTZone.md) — простая классическая LT-зона
- [`NPulseGenerator`](NPulseGenerator.md) — генератор импульсов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPSimpleLTZone` — simple spiking LT-zone with built-in pulse generator.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPSimpleLTZone", ...)`.  
**Instances**: `ClassName = "NPSimpleLTZone"` in `Bin/Configs/*/Model_*.xml`.

`NPSimpleLTZone` is an extension of `NPulseLTZone` class with built-in pulse generator (`NPulseGenerator`). Inherits from `NPulseLTZone` and adds `MaxFrequency` parameter to limit maximum spike generation frequency. Internal generator is used for managing impulse generation.

**Usage:** Simple LT-zone with built-in generator, maximum frequency limiting

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NLTZone
    NLTZone <|-- NPulseLTZoneCommon
    NPulseLTZoneCommon <|-- NPulseLTZoneThreshold
    NPulseLTZoneThreshold <|-- NPulseLTZone
    NPulseLTZone <|-- NPSimpleLTZone
    NPSimpleLTZone *-- NPulseGenerator : generator
    class NPulseLTZone {
        +TimeConstant : double
        +UseLTZIntegtation : bool
        +UseSpikeStabilizer : bool
    }
    class NPSimpleLTZone {
        +MaxFrequency : double
        -generator : NPulseGenerator
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant LTZone as NPSimpleLTZone
    participant Generator as NPulseGenerator
    participant Channels as Input Channels
    participant Neuron as Neuron
    
    Storage->>LTZone: New() + Default()
    Storage->>LTZone: Build()
    LTZone->>LTZone: Set MaxFrequency
    LTZone->>LTZone: ABuild()
    LTZone->>Generator: Create NPulseGenerator
    LTZone->>Generator: Configure generator
    loop Each step
        Channels->>LTZone: Input potentials
        Storage->>LTZone: Calculate()
        LTZone->>LTZone: ACalculate2()
        LTZone->>LTZone: Check threshold
        LTZone->>Generator: Check MaxFrequency limit
        Generator->>LTZone: Generator state
        LTZone->>LTZone: Generate spike if conditions met
        LTZone-->>Neuron: Output (spike)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateGenerator: Create NPulseGenerator
    CreateGenerator --> SetMaxFrequency: Set MaxFrequency
    SetMaxFrequency --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CheckThreshold: Check threshold
    CheckThreshold -->|Threshold reached| CheckFrequency: Check MaxFrequency limit
    CheckThreshold -->|Threshold not reached| Ready: Step completed
    CheckFrequency -->|Frequency OK| GenerateSpike: Generate spike
    CheckFrequency -->|Frequency exceeded| Ready: Skip spike
    GenerateSpike --> UpdateOutput: Update output
    UpdateOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> AggregateInputs[Aggregate input potentials]
    AggregateInputs --> GetPotential[Get Potential from channels]
    GetPotential --> CheckThreshold{Potential >= Threshold?}
    CheckThreshold -->|No| End([End])
    CheckThreshold -->|Yes| CheckFrequency{Current frequency < MaxFrequency?}
    CheckFrequency -->|No| End
    CheckFrequency -->|Yes| GenerateSpike[Generate spike<br/>using internal generator]
    GenerateSpike --> UpdateOutput[Update Output]
    UpdateOutput --> End
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseLTZone["NPulseLTZone Base"]
        BaseLTZone[NPulseLTZone]
    end
    
    subgraph NPSimpleLTZone["NPSimpleLTZone Configuration"]
        MaxFreqLimit[MaxFrequency Limit<br/>MaxFrequency parameter]
        InternalGenerator[NPulseGenerator<br/>Internal Generator]
    end
    
    subgraph External["External Components"]
        Channels[Input Channels]
        Neuron[Neuron]
    end
    
    BaseLTZone -->|extends| NPSimpleLTZone
    NPSimpleLTZone -->|creates| InternalGenerator
    Channels -->|Input potentials| NPSimpleLTZone
    NPSimpleLTZone -->|Output spike| Neuron
    NPSimpleLTZone -->|uses| MaxFreqLimit
    InternalGenerator -->|frequency control| NPSimpleLTZone
```

### Properties

`NPSimpleLTZone` uses all properties of base class `NPulseLTZone` with additional parameter:

**Configuration parameters:**
- `MaxFrequency` (double) — maximum spike generation frequency (Hz). Range: [0; 500]. Default: 100.0

**Inherited from NPulseLTZone:**
- `TimeConstant` — time constant
- `UseLTZIntegtation` — use LT-zone integration
- `UseSpikeStabilizer` — use spike stabilizer

**Inherited from NPulseLTZoneCommon:**
- `Threshold`, `ThresholdOff` — thresholds
- `PulseAmplitude`, `PulseLength` — pulse parameters
- `NumChannelsInGroup` — number of channels in group

### Methods

`NPSimpleLTZone` uses all methods of base class `NPulseLTZone` with additional method:

- **`ACalculate2()`** → `bool` — performs calculation of simple LT-zone. Uses built-in generator to manage impulse generation with `MaxFrequency` limit.

### Usage in configurations

`NPSimpleLTZone` is used in experiments requiring simple LT-zone with frequency limiting:

- **Simple LT-zones**: `Bin/Configs/*/Model_*.xml` (where simple LT-zone with frequency limit is required)
- **Frequency control**: experiments with maximum frequency limiting for spike generation

**Features:**
- Built-in generator: uses internal `NPulseGenerator` for spike generation control
- Frequency limiting: limits maximum spike generation frequency via `MaxFrequency` parameter
- Simple configuration: simplified setup compared to full `NPulseLTZone`

**Typical parameter values:**
- **MaxFrequency**: 100.0 (maximum frequency 100 Hz)
- **Threshold**: 30.0 (threshold for spike generation)
- **PulseAmplitude**: 1.0 (pulse amplitude)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseLTZone`](NPulseLTZone.md) — spiking LT-zone (base class)
- [`NCSimpleLTZone`](NCSimpleLTZone.md) — simple classic LT-zone
- [`NPulseGenerator`](NPulseGenerator.md) — pulse generator
- [Architecture.md](../Architecture.md) — library architecture
