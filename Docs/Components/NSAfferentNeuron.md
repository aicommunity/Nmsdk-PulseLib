# NSAfferentNeuron — классический афферентный нейрон

## RU

### Назначение

**Класс**: `NSAfferentNeuron` — конфигурационный вариант классического афферентного нейрона с полной структурой.  
**Префикс**: `NS` — **S**imple (простой) или **S**tandard (стандартный/классический).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSAfferentNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NSAfferentNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NSAfferentNeuron` является конфигурационным вариантом базового класса `NAfferentNeuron` с классической структурой. Создается из `NAfferentNeuron` с настройками:
- `StructureBuildMode = 1` — классическая структура
- Полная структура: мембрана + LT-зона + рецептор + генератор

**Использование:** Моделирование сенсорных систем с полной структурой

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NAfferentNeuron
    NAfferentNeuron <|.. NSAfferentNeuron : configuration variant
    NSAfferentNeuron *-- NPulseMembrane : PulseMembrane
    NSAfferentNeuron *-- NPulseLTZone : LTZone
    NSAfferentNeuron *-- NReceptor : Receptor
    NSAfferentNeuron *-- NConstGenerator : ExcGenerator
    class NAfferentNeuron {
        +StructureBuildMode : int
        +MembraneClassName : string
        +LTZoneClassName : string
        +ReceptorClassName : string
    }
    class NSAfferentNeuron {
        +StructureBuildMode : int = 1
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NAfferentNeuron` — базовый афферентный нейрон
- `NSAfferentNeuron` — классический афферентный нейрон (конфигурационный вариант)

**Внутренняя структура:**
- **Receptor** (`NReceptor`) — рецептор для приема внешних стимулов
- **PulseMembrane** (`NPulseMembrane`) — мембрана нейрона
- **LTZone** (`NPulseLTZone`) — LT-зона для генерации спайков
- **ExcGenerator** (`NConstGenerator`) — возбуждающий генератор

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NSAfferentNeuron
    participant Receptor as NReceptor
    participant Membrane as NPulseMembrane
    participant LTZone as NPulseLTZone
    participant External as Внешний стимул
    
    Storage->>Neuron: New()
    Storage->>Neuron: Default()
    Note over Neuron: StructureBuildMode = 1<br/>(классическая структура)
    Storage->>Neuron: Build()
    Neuron->>Neuron: ABuild()
    Neuron->>Neuron: BuildClassicalStructure()
    Neuron->>Membrane: AddMissingComponent("PulseMembrane")
    Neuron->>LTZone: AddMissingComponent("LTZone")
    Neuron->>Receptor: AddMissingComponent("Receptor")
    Neuron->>Neuron: CreateLinks()
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        External->>Receptor: Внешний стимул
        Storage->>Neuron: Calculate()
        Neuron->>Receptor: ACalculate()
        Receptor->>Receptor: Прием и преобразование стимула
        Receptor-->>Membrane: Выходной сигнал рецептора
        Neuron->>Membrane: ACalculate()
        Membrane->>Membrane: Интеграция токов
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (спайк)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Инициализация**: Установка `StructureBuildMode = 1` (классическая структура)
2. **Сборка**: Автоматическое создание мембраны, LT-зоны и рецептора
3. **Расчет**: Прием внешнего стимула через рецептор, обработка мембраной, генерация спайков LT-зоной

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Note over Defaulted: StructureBuildMode = 1
    Defaulted --> Building: Build()
    Building --> CreatingMembrane: Создание мембраны
    CreatingMembrane --> CreatingLTZone: Создание LT-зоны
    CreatingLTZone --> CreatingReceptor: Создание рецептора
    CreatingReceptor --> Linking: Создание связей
    Linking --> Built: Структура построена
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Расчет рецептора
    ReceptorCalc --> MembraneCalc: Расчет мембраны
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> CheckThreshold: Проверка порога
    CheckThreshold -->|Порог достигнут| Spiking: Генерация спайка
    CheckThreshold -->|Порог не достигнут| Ready: Шаг завершен
    Spiking --> Ready
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены (`StructureBuildMode = 1`)
- **Building** — выполняется сборка классической структуры
- **CreatingMembrane** — создание мембраны
- **CreatingLTZone** — создание LT-зоны
- **CreatingReceptor** — создание рецептора
- **Linking** — создание связей между компонентами
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **ReceptorCalc** — расчет рецептора (прием и преобразование стимула)
- **MembraneCalc** — расчет мембраны (интеграция токов)
- **LTZoneCalc** — расчет LT-зоны (проверка порога)
- **CheckThreshold** — проверка достижения порога
- **Spiking** — генерация спайка
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Расчет рецептора NReceptor]
    CalcReceptor --> ReceiveStimulus[Прием внешнего стимула]
    ReceiveStimulus --> TransformStimulus[Преобразование стимула]
    TransformStimulus --> CalcMembrane[Расчет мембраны NPulseMembrane]
    CalcMembrane --> AggregateChannels[Агрегация токов от каналов]
    AggregateChannels --> IntegratePotential[Интеграция потенциала]
    IntegratePotential --> CalcLTZone[Расчет LT-зоны NPulseLTZone]
    CalcLTZone --> CheckThreshold{Порог достигнут?}
    CheckThreshold -->|Да| GenerateSpike[Генерация спайка]
    CheckThreshold -->|Нет| NoSpike[Без спайка]
    GenerateSpike --> SetOutput[Output = PulseAmplitude]
    NoSpike --> SetOutputZero[Output = 0]
    SetOutput --> End([End])
    SetOutputZero --> End
```

**Алгоритм расчета:**
1. Расчет рецептора: прием внешнего стимула и его преобразование
2. Расчет мембраны: агрегация токов от каналов, интеграция потенциала
3. Расчет LT-зоны: проверка порога генерации спайка
4. Генерация спайка при достижении порога
5. Обновление выходного сигнала нейрона

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NAfferentNeuron["NAfferentNeuron Base"]
        BaseNeuron[NAfferentNeuron]
    end
    
    subgraph NSAfferentNeuron["NSAfferentNeuron"]
        Receptor[NReceptor]
        Membrane[NPulseMembrane]
        LTZone[NPulseLTZone]
        ExcGenerator[NConstGenerator]
    end
    
    subgraph External["Внешние компоненты"]
        ExternalStimulus[Внешний стимул]
        Channels[Каналы]
        Synapses[Синапсы]
    end
    
    BaseNeuron -->|конфигурация| NSAfferentNeuron
    NSAfferentNeuron -->|создает| Receptor
    NSAfferentNeuron -->|создает| Membrane
    NSAfferentNeuron -->|создает| LTZone
    NSAfferentNeuron -->|создает| ExcGenerator
    ExternalStimulus -->|входной сигнал| Receptor
    Receptor -->|выходной сигнал| Membrane
    Channels -->|токи| Membrane
    Synapses -->|сигналы| Channels
    Membrane -->|потенциал| LTZone
    LTZone -->|спайки| External
```

**Зависимости:**
- **Базовый класс**: `NAfferentNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NReceptor` (рецептор), `NPulseMembrane` (мембрана), `NPulseLTZone` (LT-зона), `NConstGenerator` (возбуждающий генератор)
- **Внешние компоненты**: внешние стимулы (источники входных сигналов), каналы, синапсы

### Свойства

`NSAfferentNeuron` использует все свойства базового класса `NAfferentNeuron` с параметрами:
- `StructureBuildMode = 1`

### Методы

`NSAfferentNeuron` использует все методы базового класса `NAfferentNeuron`.

### Использование в конфигурациях

`NSAfferentNeuron` используется в экспериментах с сенсорными системами:

- **Афферентные нейроны**: `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`
- **Управление движением**: `Bin/Configs/!OldConfigs/OldExperiments/SimplestMotionControlSAfferent/`

**Типичные значения параметров:**
- **StructureBuildMode**: 1 (классическая структура)
- **ReceptorClassName**: "NReceptor" (рецептор для приема стимулов)
- **MembraneClassName**: "NPulseMembrane" (мембрана)
- **LTZoneClassName**: "NPulseLTZone" (LT-зона)

## Источники

См. [Literature-References.md](../Literature-References.md): **25**, **29**.

### См. также

- [`NAfferentNeuron`](NAfferentNeuron.md) — базовый афферентный нейрон
- [`NSimpleAfferentNeuron`](NSimpleAfferentNeuron.md) — простой афферентный нейрон
- [`NReceptor`](NReceptor.md) — рецептор для приема стимулов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSAfferentNeuron` — configuration variant of classical afferent neuron with full structure.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSAfferentNeuron", ...)`.  
**Instances**: `ClassName = "NSAfferentNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NSAfferentNeuron` is a configuration variant of the base class `NAfferentNeuron` with classical structure. Created from `NAfferentNeuron` with settings:
- `StructureBuildMode = 1` — classical structure
- Full structure: membrane + LT-zone + receptor + generator

**Usage:** Modeling sensory systems with full structure

### UML Class Diagram

```mermaid
classDiagram
    NAfferentNeuron <|.. NSAfferentNeuron : configuration variant
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSAfferentNeuron
    participant Receptor as NReceptor
    participant Membrane as NPulseMembrane
    participant LTZone as NPulseLTZone
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Receptor: AddMissingComponent()
    Neuron->>Membrane: AddMissingComponent()
    Neuron->>LTZone: AddMissingComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Receptor: ACalculate()
        Neuron->>Membrane: ACalculate()
        Neuron->>LTZone: ACalculate()
        Neuron-->>Storage: Output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Calculate receptor
    ReceptorCalc --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Calculate receptor]
    CalcReceptor --> CalcMembrane[Calculate membrane]
    CalcMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
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
    
    subgraph NSAfferentNeuron["NSAfferentNeuron Configuration"]
        Receptor[NReceptor]
        Membrane[NPulseMembrane]
        LTZone[NPulseLTZone]
        ExcGenerator[NConstGenerator]
    end
    
    subgraph External["External Components"]
        ExternalStimulus[External Stimulus]
        Channels[Channels]
        Synapses[Synapses]
    end
    
    BaseNeuron -->|configured as| NSAfferentNeuron
    NSAfferentNeuron -->|creates| Receptor
    NSAfferentNeuron -->|creates| Membrane
    NSAfferentNeuron -->|creates| LTZone
    NSAfferentNeuron -->|creates| ExcGenerator
    ExternalStimulus -->|input signal| Receptor
    Receptor -->|output signal| Membrane
    Channels -->|currents| Membrane
    Synapses -->|signals| Channels
    Membrane -->|potential| LTZone
    LTZone -->|spikes| External
```

### Properties

`NSAfferentNeuron` uses all properties of base class `NAfferentNeuron` with parameters:
- `StructureBuildMode = 1` — classical structure mode

**Inherited properties:**
- `MembraneClassName` — membrane class name (default "NPulseMembrane")
- `LTZoneClassName` — LT-zone class name (default "NPulseLTZone")
- `ReceptorClassName` — receptor class name (by default "NReceptor")
- `ExcGeneratorClassName` — excitatory generator class name
- `NumSomaMembraneParts` — number of soma parts

### Methods

`NSAfferentNeuron` uses all methods of base class `NAfferentNeuron`:
- `ADefault()` — setting default parameters (sets StructureBuildMode = 1)
- `ABuild()` — building neuron structure (calls BuildClassicalStructure)
- `AReset()` — resetting neuron states
- `ACalculate()` — neuron calculation step

### Usage in configurations

`NSAfferentNeuron` is used in sensory system experiments:

- **Afferent neurons**: `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`
- **Motion control**: `Bin/Configs/!OldConfigs/OldExperiments/SimplestMotionControlSAfferent/`

**Typical parameter values:**
- **StructureBuildMode**: 1 (classical structure)
- **ReceptorClassName**: "NReceptor" (receptor for receiving stimuli)
- **MembraneClassName**: "NPulseMembrane" (membrane)
- **LTZoneClassName**: "NPulseLTZone" (LT-zone)

**Features:**
- Classical structure: full structure with membrane, LT-zone, receptor, and generator
- External stimulus reception: receptor receives and transforms external stimuli
- Complete signal processing: stimulus → receptor → membrane → LT-zone → spike

### References

See [Literature-References.md](../Literature-References.md): **25**, **29**.

### See Also

- [`NAfferentNeuron`](NAfferentNeuron.md) — base afferent neuron
- [`NSimpleAfferentNeuron`](NSimpleAfferentNeuron.md) — simple afferent neuron
- [`NReceptor`](NReceptor.md) — receptor for receiving stimuli
- [Architecture.md](../Architecture.md) — library architecture
