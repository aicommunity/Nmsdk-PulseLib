# NCNeuronPosCGenerator — генератор положительных токов для классических нейронов

## RU

### Назначение

**Класс**: `NCNeuronPosCGenerator` — конфигурационный вариант генератора постоянного тока с положительной амплитудой для классических нейронов.  
**Префикс**: `NC` — **C**ontinuous (непрерывный, классический), компонент с непрерывными входами/выходами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCNeuronPosCGenerator", ...)`.  
**Storage-инстансы**: `ClassName = "NCNeuronPosCGenerator"` в `Bin/Configs/*/Model_*.xml`.

`NCNeuronPosCGenerator` является конфигурационным вариантом класса `NConstGenerator` с параметром `Amplitude = 2.0`. При создании компонента с `ClassName = "NCNeuronPosCGenerator"` создается экземпляр генератора постоянного тока с положительной амплитудой для классических нейронов.

**Использование:** Генератор положительных токов для классических нейронов

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NSource
    NSource <|-- NConstGenerator
    NConstGenerator <|.. NCNeuronPosCGenerator : configuration variant
    class NCNeuronPosCGenerator {
        +Amplitude : double = 2.0
    }
```

**Параметры конфигурации:**
- `Amplitude = 2.0` — положительная амплитуда для классических нейронов

## Источники

См. [Literature-References.md](../Literature-References.md): **14**, **25**.

### См. также

- [`NConstGenerator`](NCGenerator.md) — генератор постоянного тока (базовый класс)
- [`NCNeuronNegCGenerator`](NCNeuronNegCGenerator.md) — генератор отрицательных токов для классических нейронов
- [`NPNeuronPosCGenerator`](NPNeuronPosCGenerator.md) — генератор для импульсных нейронов

---

## EN

### Purpose

**Class**: `NCNeuronPosCGenerator` — configuration variant of constant current generator with positive amplitude for classic neurons.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NCNeuronPosCGenerator", ...)`.  
**Instances**: `ClassName = "NCNeuronPosCGenerator"` in `Bin/Configs/*/Model_*.xml`.

`NCNeuronPosCGenerator` is a configuration variant of `NConstGenerator` class with parameter `Amplitude = 2.0`. When creating a component with `ClassName = "NCNeuronPosCGenerator"`, an instance of constant current generator with positive amplitude for classic neurons is created.

**Usage:** Positive current generator for classic neurons

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NSource
    NSource <|-- NConstGenerator
    NConstGenerator <|.. NCNeuronPosCGenerator : configuration variant
    class NConstGenerator {
        +Amplitude : double
        +Output : MDMatrix~double~
    }
    class NCNeuronPosCGenerator {
        +Amplitude : double = 2.0
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Generator as NCNeuronPosCGenerator
    participant Neuron as Classic Neuron
    
    Storage->>Generator: New() + Default()
    Storage->>Generator: Build()
    Generator->>Generator: Set Amplitude = 2.0
    loop Each step
        Storage->>Generator: Calculate()
        Generator->>Generator: Generate constant current
        Generator->>Generator: Output = Amplitude (2.0)
        Generator-->>Neuron: Output (positive current)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetAmplitude: Set Amplitude = 2.0
    SetAmplitude --> Building: Build()
    Building --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> GenerateCurrent: Generate constant current
    GenerateCurrent --> SetOutput: Set Output = 2.0
    SetOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> GetAmplitude[Get Amplitude = 2.0]
    GetAmplitude --> GenerateCurrent["Generate constant current<br/>Output = Amplitude"]
    GenerateCurrent --> SetOutput["Set Output = 2.0<br/>positive current"]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NConstGenerator["NConstGenerator Base"]
        BaseGenerator[NConstGenerator]
    end
    
    subgraph NCNeuronPosCGenerator["NCNeuronPosCGenerator Configuration"]
        PositiveAmplitude["Positive Amplitude<br/>Amplitude = 2.0"]
    end
    
    subgraph External["External Components"]
        ClassicNeurons[Classic Neurons]
    end
    
    BaseGenerator -->|configured as| NCNeuronPosCGenerator
    NCNeuronPosCGenerator -->|Output<br/>positive current| ClassicNeurons
    NCNeuronPosCGenerator -->|Output| External
```

### Properties

`NCNeuronPosCGenerator` uses all properties of base class `NConstGenerator` with preset value:

**Configuration parameters:**
- `Amplitude = 2.0` — positive amplitude for classic neurons

**Inherited properties:**
- `Amplitude` (double) — current amplitude (preset to 2.0)
- `Output` (MDMatrix<double>) — output current signal

### Methods

`NCNeuronPosCGenerator` uses all methods of base class `NConstGenerator`.

### Usage in configurations

`NCNeuronPosCGenerator` is used in classic neuron experiments:

- **Classic neurons**: Generates positive constant current for classic neurons
- **Positive current**: Provides positive amplitude (2.0) for excitation

**Features:**
- Automatically configured with positive amplitude (`Amplitude = 2.0`)
- Constant current: Generates constant positive current
- Classic model: Optimized for classic (non-spiking) neuron models

**Typical parameter values:**
- **Amplitude**: 2.0 (positive amplitude for classic neurons)

### References

See [Literature-References.md](../Literature-References.md): **14**, **25**.

### See Also

- [`NConstGenerator`](NCGenerator.md) — constant current generator (base class)
- [`NCNeuronNegCGenerator`](NCNeuronNegCGenerator.md) — negative current generator for classic neurons
- [`NPNeuronPosCGenerator`](NPNeuronPosCGenerator.md) — generator for spiking neurons
