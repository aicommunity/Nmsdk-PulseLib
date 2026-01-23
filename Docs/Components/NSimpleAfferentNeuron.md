# NSimpleAfferentNeuron — простой афферентный нейрон

## RU

### Назначение

**Класс**: `NSimpleAfferentNeuron` — конфигурационный вариант простого афферентного нейрона с упрощенной структурой.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSimpleAfferentNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NSimpleAfferentNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NSimpleAfferentNeuron` является конфигурационным вариантом базового класса `NAfferentNeuron` с простой структурой. Создается из `NAfferentNeuron` с настройками:
- `StructureBuildMode = 2` — простая структура
- `LTZoneClassName = "NPSimpleLTZone"` — простая LT-зона
- `ReceptorClassName = "NReceptor"` — рецептор
- Параметры рецептора: `ExpCoeff = 10e-5`, `Gain = 1`, `SumCoeff = 2`, `MaxOutputRange = 100`, `OutputAdaptationMode = 0`

Простая структура включает только LT-зону и рецептор, без мембраны.

**Использование:** Моделирование простых сенсорных систем

### UML-диаграмма классов

```mermaid
classDiagram
    NAfferentNeuron <|.. NSimpleAfferentNeuron : configuration variant
    NSimpleAfferentNeuron *-- NPSimpleLTZone : LTZone
    NSimpleAfferentNeuron *-- NReceptor : Receptor
    class NSimpleAfferentNeuron {
        +StructureBuildMode : int = 2
        +LTZoneClassName : string = "NPSimpleLTZone"
        +ReceptorClassName : string = "NReceptor"
    }
```

### Свойства

`NSimpleAfferentNeuron` использует все свойства базового класса `NAfferentNeuron` с параметрами:
- `StructureBuildMode = 2`
- `LTZoneClassName = "NPSimpleLTZone"`
- `ReceptorClassName = "NReceptor"`

### Методы

`NSimpleAfferentNeuron` использует все методы базового класса `NAfferentNeuron`.

### См. также

- [`NAfferentNeuron`](NAfferentNeuron.md) — базовый афферентный нейрон
- [`NSAfferentNeuron`](NSAfferentNeuron.md) — классический афферентный нейрон
- [`NPSimpleLTZone`](NPSimpleLTZone.md) — простая LT-зона
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSimpleAfferentNeuron` — configuration variant of simple afferent neuron with simplified structure.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSimpleAfferentNeuron", ...)`.  
**Instances**: `ClassName = "NSimpleAfferentNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NSimpleAfferentNeuron` is a configuration variant of the base class `NAfferentNeuron` with simple structure. Created from `NAfferentNeuron` with settings:
- `StructureBuildMode = 2` — simple structure
- `LTZoneClassName = "NPSimpleLTZone"` — simple LT-zone
- `ReceptorClassName = "NReceptor"` — receptor

Simple structure includes only LT-zone and receptor, without membrane.

**Usage:** Modeling simple sensory systems

### UML Class Diagram

```mermaid
classDiagram
    NAfferentNeuron <|.. NSimpleAfferentNeuron : configuration variant
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSimpleAfferentNeuron
    participant Receptor as NReceptor
    participant LTZone as NPSimpleLTZone
    
    Storage->>Neuron: New() (from NAfferentNeuron)
    Storage->>Neuron: SetStructureBuildMode(2)
    Storage->>Neuron: SetLTZoneClassName("NPSimpleLTZone")
    Storage->>Neuron: SetReceptorClassName("NReceptor")
    Storage->>Neuron: Build()
    Neuron->>Receptor: AddMissingComponent()
    Neuron->>LTZone: AddMissingComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Receptor: ACalculate()
        Receptor-->>LTZone: Output
        LTZone->>LTZone: Check threshold
        LTZone-->>Neuron: Output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Set simple structure params
    Configuring --> Building: Build()
    Building --> CreatingLTZone: Create NPSimpleLTZone
    CreatingLTZone --> CreatingReceptor: Create NReceptor
    CreatingReceptor --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Calculate receptor
    ReceptorCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Calculate NReceptor]
    CalcReceptor --> TransformStimulus[Transform stimulus]
    TransformStimulus --> DirectToLTZone[Direct to NPSimpleLTZone]
    DirectToLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateSpike[Generate spike]
    CheckThreshold -->|No| NoSpike[No spike]
    GenerateSpike --> End([End])
    NoSpike --> End
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NAfferentNeuron["NAfferentNeuron Base"]
        BaseNeuron[NAfferentNeuron]
    end
    
    subgraph NSimpleAfferentNeuron["NSimpleAfferentNeuron Configuration"]
        Receptor[NReceptor]
        LTZone[NPSimpleLTZone]
    end
    
    subgraph External["External Components"]
        ExternalStimulus[External Stimulus]
    end
    
    BaseNeuron -->|configured as| NSimpleAfferentNeuron
    NSimpleAfferentNeuron -->|creates| Receptor
    NSimpleAfferentNeuron -->|creates| LTZone
    ExternalStimulus -->|input| Receptor
    Receptor -->|Output| LTZone
    LTZone -->|Output| NSimpleAfferentNeuron
```

### Properties

`NSimpleAfferentNeuron` uses all properties of base class `NAfferentNeuron` with parameters:
- `StructureBuildMode = 2` — simple structure mode
- `LTZoneClassName = "NPSimpleLTZone"` — simple LT-zone
- `ReceptorClassName = "NReceptor"` — receptor

**Receptor parameters (configured in receptor):**
- `ExpCoeff = 10e-5` — exponential coefficient
- `Gain = 1` — gain coefficient
- `SumCoeff = 2` — sum coefficient
- `MaxOutputRange = 100` — maximum output range
- `OutputAdaptationMode = 0` — output adaptation mode (linear)

### Methods

`NSimpleAfferentNeuron` uses all methods of base class `NAfferentNeuron`:
- `ADefault()` — установка параметров по умолчанию (устанавливает StructureBuildMode = 2)
- `ABuild()` — сборка структуры нейрона (вызывает BuildSimpleStructure)
- `AReset()` — сброс состояний нейрона
- `ACalculate()` — выполнение шага расчета нейрона

### Usage in configurations

`NSimpleAfferentNeuron` is used in simple sensory system modeling:

- **Simple sensory systems**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where simple afferent neurons are required)

**Typical parameter values:**
- **StructureBuildMode**: 2 (simple structure)
- **LTZoneClassName**: "NPSimpleLTZone" (simple LT-zone)
- **ReceptorClassName**: "NReceptor" (receptor)
- **Receptor parameters**: ExpCoeff=10e-5, Gain=1, SumCoeff=2, MaxOutputRange=100, OutputAdaptationMode=0

**Features:**
- Simple structure: only LT-zone and receptor, without membrane
- Optimized for simple sensory processing
- Lower computational complexity compared to classical structure

### See Also

- [`NAfferentNeuron`](NAfferentNeuron.md) — base afferent neuron
- [`NSAfferentNeuron`](NSAfferentNeuron.md) — classical afferent neuron
- [`NPSimpleLTZone`](NPSimpleLTZone.md) — simple LT-zone
- [Architecture.md](../Architecture.md) — library architecture
