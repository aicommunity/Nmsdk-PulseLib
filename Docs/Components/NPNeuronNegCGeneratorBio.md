# NPNeuronNegCGeneratorBio — генератор отрицательных токов для биоинспирированных моделей

## RU

### Назначение

**Класс**: `NPNeuronNegCGeneratorBio` — конфигурационный вариант генератора постоянного тока с отрицательной амплитудой для биоинспирированных импульсных нейронов.  
**Аббревиатура**: `Bio` — **Bio**logical (биологическая модель).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPNeuronNegCGeneratorBio", ...)`.  
**Storage-инстансы**: `ClassName = "NPNeuronNegCGeneratorBio"` в `Bin/Configs/*/Model_*.xml`.

`NPNeuronNegCGeneratorBio` является конфигурационным вариантом класса `NConstGenerator` с параметром `Amplitude = -1.0`. При создании компонента с `ClassName = "NPNeuronNegCGeneratorBio"` создается экземпляр генератора постоянного тока с отрицательной амплитудой для биоинспирированных моделей.

**Использование:** Генератор отрицательных токов для биоинспирированных импульсных нейронов

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NSource
    NSource <|-- NConstGenerator
    NConstGenerator <|.. NPNeuronNegCGeneratorBio : configuration variant
    class NPNeuronNegCGeneratorBio {
        +Amplitude : double = -1.0
    }
```

**Параметры конфигурации:**
- `Amplitude = -1.0` — отрицательная амплитуда для биоинспирированных моделей

### См. также

- [`NConstGenerator`](NConstGenerator.md) — генератор постоянного тока (базовый класс)
- [`NPNeuronNegCGenerator`](NPNeuronNegCGenerator.md) — генератор отрицательных токов
- [`NPNeuronPosCGeneratorBio`](NPNeuronPosCGeneratorBio.md) — генератор положительных токов для биоинспирированных моделей

---

## EN

### Purpose

**Class**: `NPNeuronNegCGeneratorBio` — configuration variant of constant current generator with negative amplitude for bio-inspired spiking neurons.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPNeuronNegCGeneratorBio", ...)`.  
**Instances**: `ClassName = "NPNeuronNegCGeneratorBio"` in `Bin/Configs/*/Model_*.xml`.

`NPNeuronNegCGeneratorBio` is a configuration variant of `NConstGenerator` class with parameter `Amplitude = -1.0`. When creating a component with `ClassName = "NPNeuronNegCGeneratorBio"`, an instance of constant current generator with negative amplitude for bio-inspired models is created.

**Usage:** Negative current generator for bio-inspired spiking neurons

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NSource
    NSource <|-- NConstGenerator
    NConstGenerator <|.. NPNeuronNegCGeneratorBio : configuration variant
    class NConstGenerator {
        +Amplitude : double
        +Output : MDMatrix~double~
    }
    class NPNeuronNegCGeneratorBio {
        +Amplitude : double = -1.0
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Generator as NPNeuronNegCGeneratorBio
    participant Neuron as Bio-Inspired Neuron
    
    Storage->>Generator: New() + Default()
    Storage->>Generator: Build()
    Generator->>Generator: Set Amplitude = -1.0
    loop Each step
        Storage->>Generator: Calculate()
        Generator->>Generator: Generate constant current
        Generator->>Generator: Output = Amplitude (-1.0)
        Generator-->>Neuron: Output (negative current)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetAmplitude: Set Amplitude = -1.0
    SetAmplitude --> Building: Build()
    Building --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> GenerateCurrent: Generate constant current
    GenerateCurrent --> SetOutput: Set Output = -1.0
    SetOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> GetAmplitude[Get Amplitude = -1.0]
    GetAmplitude --> GenerateCurrent[Generate constant current<br/>Output = Amplitude]
    GenerateCurrent --> SetOutput[Set Output = -1.0<br/>negative current for bio models]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NConstGenerator["NConstGenerator Base"]
        BaseGenerator[NConstGenerator]
    end
    
    subgraph NPNeuronNegCGeneratorBio["NPNeuronNegCGeneratorBio Configuration"]
        BioAmplitude[Bio-Inspired Amplitude<br/>Amplitude = -1.0]
    end
    
    subgraph External["External Components"]
        BioNeurons[Bio-Inspired Neurons]
    end
    
    BaseGenerator -->|configured as| NPNeuronNegCGeneratorBio
    NPNeuronNegCGeneratorBio -->|Output<br/>negative current| BioNeurons
```

### Properties

`NPNeuronNegCGeneratorBio` uses all properties of base class `NConstGenerator` with preset value:

**Configuration parameters:**
- `Amplitude = -1.0` — negative amplitude for bio-inspired models

**Inherited properties:**
- `Amplitude` (double) — current amplitude (preset to -1.0)
- `Output` (MDMatrix<double>) — output current signal

### Methods

`NPNeuronNegCGeneratorBio` uses all methods of base class `NConstGenerator`.

### Usage in configurations

`NPNeuronNegCGeneratorBio` is used in bio-inspired neuron experiments:

- **Bio-inspired neurons**: Generates negative constant current for bio-inspired spiking neurons
- **Inhibitory potential**: Provides negative amplitude (-1.0) for inhibition in bio models

**Features:**
- Automatically configured with negative amplitude (`Amplitude = -1.0`)
- Constant current: Generates constant negative current
- Bio-optimized: Optimized for bio-inspired models

**Typical parameter values:**
- **Amplitude**: -1.0 (negative amplitude for bio-inspired models)

### See Also

- [`NConstGenerator`](NConstGenerator.md) — constant current generator (base class)
- [`NPNeuronNegCGenerator`](NPNeuronNegCGenerator.md) — negative current generator
- [`NPNeuronPosCGeneratorBio`](NPNeuronPosCGeneratorBio.md) — positive current generator for bio-inspired models
