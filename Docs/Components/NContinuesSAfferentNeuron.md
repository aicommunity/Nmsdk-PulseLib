# NContinuesSAfferentNeuron — непрерывный классический афферентный нейрон

## RU

### Назначение

**Класс**: `NContinuesSAfferentNeuron` — конфигурационный вариант классического афферентного нейрона с непрерывным выходом.  
**Префикс**: `NContinues` — **Continues** (Continuous, непрерывный вариант компонента).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSAfferentNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NContinuesSAfferentNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NContinuesSAfferentNeuron` является конфигурационным вариантом базового класса `NAfferentNeuron` с классической структурой и непрерывным выходом. Создается из `NAfferentNeuron` с настройками:
- `StructureBuildMode = 1` — классическая структура
- `MembraneClassName = "NCSynNeuronMembrane"` — классическая мембрана, оптимизированная для синапсов
- `LTZoneClassName = "NCLTZone"` — классическая LT-зона
- `ExcGeneratorClassName = "NCNeuronNegCGenerator"` — классический генератор
- Параметры рецептора: `ExpCoeff = 100`, `Gain = 2`, `SumCoeff = 2`, `MaxOutputRange = 1`, `OutputAdaptationMode = 4`

Классическая структура включает мембрану, LT-зону и рецептор. Непрерывный выход означает, что LT-зона выдает непрерывный сигнал вместо дискретных спайков.

**Использование:** Моделирование сенсорных систем с непрерывным выходом

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NAfferentNeuron
    NAfferentNeuron <|.. NContinuesSAfferentNeuron : configuration variant
    NContinuesSAfferentNeuron *-- NCSynNeuronMembrane : PulseMembrane
    NContinuesSAfferentNeuron *-- NCLTZone : LTZone
    NContinuesSAfferentNeuron *-- NReceptor : Receptor
    NContinuesSAfferentNeuron *-- NCNeuronNegCGenerator : ExcGenerator
    class NAfferentNeuron {
        +StructureBuildMode : int
        +MembraneClassName : string
        +LTZoneClassName : string
        +ReceptorClassName : string
        +ExcGeneratorClassName : string
    }
    class NContinuesSAfferentNeuron {
        +StructureBuildMode : int = 1
        +MembraneClassName : string = "NCSynNeuronMembrane"
        +LTZoneClassName : string = "NCLTZone"
        +ExcGeneratorClassName : string = "NCNeuronNegCGenerator"
    }
    class NReceptor {
        +ExpCoeff : double = 100
        +Gain : double = 2
        +SumCoeff : double = 2
        +MaxOutputRange : double = 1
        +OutputAdaptationMode : int = 4
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NAfferentNeuron` — базовый афферентный нейрон
- `NContinuesSAfferentNeuron` — конфигурационный вариант с классической структурой и непрерывным выходом

**Внутренняя структура:**
- **PulseMembrane** (`NCSynNeuronMembrane`) — классическая мембрана, оптимизированная для синапсов
- **LTZone** (`NCLTZone`) — классическая LT-зона с непрерывным выходом
- **Receptor** (`NReceptor`) — рецептор для приема внешних стимулов
- **ExcGenerator** (`NCNeuronNegCGenerator`) — классический генератор

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NContinuesSAfferentNeuron
    participant Receptor as NReceptor
    participant Membrane as NCSynNeuronMembrane
    participant LTZone as NCLTZone
    participant ExternalStimulus as Внешний стимул
    
    Storage->>Neuron: New() (из NAfferentNeuron)
    Storage->>Neuron: SetStructureBuildMode(1)
    Storage->>Neuron: SetMembraneClassName("NCSynNeuronMembrane")
    Storage->>Neuron: SetLTZoneClassName("NCLTZone")
    Storage->>Neuron: SetReceptorClassName("NReceptor")
    Storage->>Neuron: SetExcGeneratorClassName("NCNeuronNegCGenerator")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NAfferentNeuron::ABuild()
    Neuron->>Neuron: BuildClassicalStructure()
    Neuron->>Receptor: AddMissingComponent("Receptor", "NReceptor")
    Neuron->>Membrane: CreateComponent("PulseMembrane", "NCSynNeuronMembrane")
    Neuron->>LTZone: CreateComponent("LTZone", "NCLTZone")
    Neuron->>Neuron: CreateLink(Receptor->Output, Membrane->Input)
    Neuron->>Neuron: CreateLink(Receptor->Output, LTZone->Input)
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        ExternalStimulus->>Receptor: Input (внешний стимул)
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NAfferentNeuron::ACalculate()
        Neuron->>Receptor: ACalculate()
        Receptor->>Receptor: Преобразование стимула
        Receptor-->>Membrane: Output (преобразованный сигнал)
        Receptor-->>LTZone: Output (преобразованный сигнал)
        Neuron->>Membrane: ACalculate()
        Membrane->>Membrane: Расчет классической мембраны
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Расчет классической LT-зоны
        LTZone->>LTZone: Непрерывный выход (не спайки)
        LTZone-->>Neuron: Output (непрерывный сигнал)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Создание**: `NContinuesSAfferentNeuron` создается из `NAfferentNeuron` с настройкой параметров
2. **Настройка**: Устанавливается классическая структура, классические мембрана и LT-зона
3. **Сборка**: Автоматически создается структура нейрона с мембраной, LT-зоной, рецептором и генератором
4. **Расчет**: На каждом шаге рассчитываются рецептор, мембрана и LT-зона с непрерывным выходом

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка классической структуры
    Configuring --> SetClassicalParams: SetStructureBuildMode(1)<br/>SetMembraneClassName("NCSynNeuronMembrane")<br/>SetLTZoneClassName("NCLTZone")
    SetClassicalParams --> Building: Build()
    Building --> BuildClassical: BuildClassicalStructure()
    BuildClassical --> CreateReceptor: Создание NReceptor
    CreateReceptor --> CreateMembrane: Создание NCSynNeuronMembrane
    CreateMembrane --> CreateLTZone: Создание NCLTZone
    CreateLTZone --> CreateGenerator: Создание NCNeuronNegCGenerator
    CreateGenerator --> LinkComponents: Создание связей
    LinkComponents --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Расчет рецептора
    ReceptorCalc --> MembraneCalc: Расчет мембраны
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны (непрерывный выход)
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для классической структуры
- **SetClassicalParams** — установка параметров (классическая структура, классические компоненты)
- **Building** — выполняется сборка структуры
- **BuildClassical** — сборка классической структуры
- **CreateReceptor** — создание рецептора
- **CreateMembrane** — создание классической мембраны
- **CreateLTZone** — создание классической LT-зоны
- **CreateGenerator** — создание классического генератора
- **LinkComponents** — создание связей между компонентами
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **ReceptorCalc** — расчет рецептора
- **MembraneCalc** — расчет мембраны
- **LTZoneCalc** — расчет LT-зоны с непрерывным выходом
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Расчет NReceptor]
    CalcReceptor --> TransformStimulus[Преобразование внешнего стимула]
    TransformStimulus --> AdaptInput[Адаптация входного диапазона]
    AdaptInput --> TransformOutput[Преобразование выходного сигнала]
    TransformOutput --> DirectToMembrane[Направление в NCSynNeuronMembrane]
    DirectToMembrane --> DirectToLTZone[Направление в NCLTZone]
    DirectToLTZone --> CalcMembrane[Расчет классической мембраны]
    CalcMembrane --> CalcLTZone[Расчет классической LT-зоны]
    CalcLTZone --> ContinuousOutput[Непрерывный выход]
    ContinuousOutput --> End([End])
```

**Алгоритм расчета:**
1. Расчет рецептора: преобразование внешнего стимула с адаптацией и преобразованием
2. Направление выходного сигнала рецептора в мембрану и LT-зону
3. Расчет классической мембраны с оптимизацией для синапсов
4. Расчет классической LT-зоны с непрерывным выходом (не дискретные спайки)

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NAfferentNeuron["NAfferentNeuron Base"]
        BaseNeuron[NAfferentNeuron]
    end
    
    subgraph NContinuesSAfferentNeuron["NContinuesSAfferentNeuron Configuration"]
        Receptor[NReceptor<br/>Receptor]
        Membrane[NCSynNeuronMembrane<br/>PulseMembrane]
        LTZone[NCLTZone<br/>LTZone]
        Generator[NCNeuronNegCGenerator<br/>ExcGenerator]
    end
    
    subgraph External["Внешние компоненты"]
        ExternalStimulus[Внешний стимул]
        Channels[Каналы]
        Synapses[Синапсы]
    end
    
    BaseNeuron -->|конфигурируется как| NContinuesSAfferentNeuron
    NContinuesSAfferentNeuron -->|создает| Receptor
    NContinuesSAfferentNeuron -->|создает| Membrane
    NContinuesSAfferentNeuron -->|создает| LTZone
    NContinuesSAfferentNeuron -->|создает| Generator
    ExternalStimulus -->|входной сигнал| Receptor
    Receptor -->|выходной сигнал| Membrane
    Receptor -->|выходной сигнал| LTZone
    Channels -->|токи| Membrane
    Synapses -->|сигналы| Channels
    Membrane -->|потенциал| LTZone
    LTZone -->|непрерывный выход| NContinuesSAfferentNeuron
```

**Зависимости:**
- **Базовый класс**: `NAfferentNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NReceptor` (рецептор), `NCSynNeuronMembrane` (классическая мембрана), `NCLTZone` (классическая LT-зона), `NCNeuronNegCGenerator` (классический генератор)
- **Внешние компоненты**: внешние стимулы, каналы, синапсы

### Свойства

`NContinuesSAfferentNeuron` использует все свойства базового класса `NAfferentNeuron` с параметрами:
- `StructureBuildMode = 1` — классическая структура
- `MembraneClassName = "NCSynNeuronMembrane"` — классическая мембрана, оптимизированная для синапсов
- `LTZoneClassName = "NCLTZone"` — классическая LT-зона
- `ExcGeneratorClassName = "NCNeuronNegCGenerator"` — классический генератор
- `ReceptorClassName = "NReceptor"` — рецептор

**Параметры рецептора (в NReceptor):**
- `ExpCoeff = 100` — экспоненциальный коэффициент
- `Gain = 2` — коэффициент усиления
- `SumCoeff = 2` — коэффициент суммы
- `MaxOutputRange = 1` — максимальный диапазон выходного сигнала
- `OutputAdaptationMode = 4` — режим адаптации выходного сигнала (линейный с усилением)

### Методы

`NContinuesSAfferentNeuron` использует все методы базового класса `NAfferentNeuron`:
- `BuildClassicalStructure()` → `bool` — сборка классической структуры (мембрана + LT-зона + рецептор)

### Примеры использования

#### Пример 1: Создание непрерывного классического афферентного нейрона в коде C++

```cpp
// Создание непрерывного классического афферентного нейрона
auto neuron = storage->CreateComponent("NContinuesSAfferentNeuron");
neuron->SetName("ContinuesSAfferentNeuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NCSynNeuronMembrane, NCLTZone, NReceptor, NCNeuronNegCGenerator)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    // Установка внешнего стимула
    // neuron->Receptor->Input = stimulus;
    neuron->Calculate();
    // Получение непрерывного выходного сигнала
    // auto output = neuron->Output;
}
```

#### Пример 2: Конфигурация XML

```xml
<ContinuesSAfferentNeuron1 Class="NContinuesSAfferentNeuron">
    <Parameters>
        <!-- Параметры наследуются от NAfferentNeuron -->
    </Parameters>
</ContinuesSAfferentNeuron1>
```

### Использование в конфигурациях

`NContinuesSAfferentNeuron` используется в экспериментах с сенсорными системами с непрерывным выходом:

- **Непрерывные сенсорные системы**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где используется непрерывный выход для классических афферентных нейронов)
- **Классическое моделирование**: эксперименты с классическими мембранами и LT-зонами

**Типичные значения параметров:**
- **StructureBuildMode**: 1 (классическая структура)
- **MembraneClassName**: "NCSynNeuronMembrane" (классическая мембрана)
- **LTZoneClassName**: "NCLTZone" (классическая LT-зона)
- **ExcGeneratorClassName**: "NCNeuronNegCGenerator" (классический генератор)

**Особенности:**
- Классическая структура: включает мембрану, LT-зону и рецептор
- Непрерывный выход: LT-зона выдает непрерывный сигнал вместо дискретных спайков
- Классические компоненты: использует классические мембраны и LT-зоны для непрерывного моделирования

## Источники

См. [Literature-References.md](../Literature-References.md): **25**, **29**.

### См. также

- [`NAfferentNeuron`](NAfferentNeuron.md) — базовый афферентный нейрон
- [`NSAfferentNeuron`](NSAfferentNeuron.md) — классический афферентный нейрон
- [`NContinuesSimpleAfferentNeuron`](NContinuesSimpleAfferentNeuron.md) — непрерывный простой афферентный нейрон
- [`NCSynNeuronMembrane`](NCSynNeuronMembrane.md) — классическая мембрана, оптимизированная для синапсов
- [`NCLTZone`](NCLTZone.md) — классическая LT-зона
- [`NReceptor`](NReceptor.md) — рецептор
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NContinuesSAfferentNeuron` — configuration variant of classical afferent neuron with continuous output.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSAfferentNeuron", ...)`.  
**Instances**: `ClassName = "NContinuesSAfferentNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NContinuesSAfferentNeuron` is a configuration variant of the base class `NAfferentNeuron` with classical structure and continuous output. Created from `NAfferentNeuron` with settings:
- `StructureBuildMode = 1` — classical structure
- `MembraneClassName = "NCSynNeuronMembrane"` — classic membrane optimized for synapses
- `LTZoneClassName = "NCLTZone"` — classic LT-zone
- `ExcGeneratorClassName = "NCNeuronNegCGenerator"` — classic generator
- Receptor parameters: `ExpCoeff = 100`, `Gain = 2`, `SumCoeff = 2`, `MaxOutputRange = 1`, `OutputAdaptationMode = 4`

Classical structure includes membrane, LT-zone, and receptor. Continuous output means LT-zone produces continuous signal instead of discrete spikes.

**Usage:** Modeling sensory systems with continuous output

### UML Class Diagram

```mermaid
classDiagram
    NAfferentNeuron <|.. NContinuesSAfferentNeuron : configuration variant
    NContinuesSAfferentNeuron *-- NCSynNeuronMembrane : PulseMembrane
    NContinuesSAfferentNeuron *-- NCLTZone : LTZone
    NContinuesSAfferentNeuron *-- NReceptor : Receptor
    NContinuesSAfferentNeuron *-- NCNeuronNegCGenerator : ExcGenerator
    class NContinuesSAfferentNeuron {
        +StructureBuildMode : int = 1
        +MembraneClassName : string = "NCSynNeuronMembrane"
        +LTZoneClassName : string = "NCLTZone"
        +ExcGeneratorClassName : string = "NCNeuronNegCGenerator"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NContinuesSAfferentNeuron
    participant Receptor as NReceptor
    participant Membrane as NCSynNeuronMembrane
    participant LTZone as NCLTZone
    participant ExternalStimulus
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Receptor: AddMissingComponent()
    Neuron->>Membrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    loop Each step
        ExternalStimulus->>Receptor: Input
        Storage->>Neuron: Calculate()
        Neuron->>Receptor: ACalculate()
        Receptor-->>Membrane: Output
        Receptor-->>LTZone: Output
        Neuron->>Membrane: ACalculate()
        Neuron->>LTZone: ACalculate()
        LTZone-->>Neuron: Continuous output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> BuildClassical: Build classical structure
    BuildClassical --> CreateComponents: Create components
    CreateComponents --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Calculate receptor
    ReceptorCalc --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone (continuous)
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Calculate receptor]
    CalcReceptor --> TransformStimulus[Transform stimulus]
    TransformStimulus --> DirectToMembrane[Direct to membrane]
    DirectToMembrane --> DirectToLTZone[Direct to LT-zone]
    DirectToLTZone --> CalcMembrane[Calculate membrane]
    CalcMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> ContinuousOutput[Continuous output]
    ContinuousOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NAfferentNeuron["NAfferentNeuron Base"]
        BaseNeuron[NAfferentNeuron]
    end
    
    subgraph NContinuesSAfferentNeuron["NContinuesSAfferentNeuron Configuration"]
        Receptor[NReceptor]
        Membrane[NCSynNeuronMembrane]
        LTZone[NCLTZone]
        Generator[NCNeuronNegCGenerator]
    end
    
    subgraph External["External Components"]
        ExternalStimulus[External Stimulus]
        Channels[Channels]
    end
    
    BaseNeuron -->|configured as| NContinuesSAfferentNeuron
    NContinuesSAfferentNeuron -->|creates| Receptor
    NContinuesSAfferentNeuron -->|creates| Membrane
    NContinuesSAfferentNeuron -->|creates| LTZone
    ExternalStimulus -->|input| Receptor
    Receptor -->|output| Membrane
    Receptor -->|output| LTZone
    Membrane -->|potential| LTZone
    LTZone -->|continuous output| NContinuesSAfferentNeuron
```

### Properties

`NContinuesSAfferentNeuron` uses all properties of base class `NAfferentNeuron` with parameters:
- `StructureBuildMode = 1` — classical structure
- `MembraneClassName = "NCSynNeuronMembrane"` — classic membrane optimized for synapses
- `LTZoneClassName = "NCLTZone"` — classic LT-zone
- `ExcGeneratorClassName = "NCNeuronNegCGenerator"` — classic generator

**Receptor parameters (in NReceptor):**
- `ExpCoeff = 100` — exponential coefficient
- `Gain = 2` — gain coefficient
- `SumCoeff = 2` — sum coefficient
- `MaxOutputRange = 1` — maximum output range
- `OutputAdaptationMode = 4` — output adaptation mode (linear with gain)

### Methods

`NContinuesSAfferentNeuron` uses all methods of base class `NAfferentNeuron`:
- `BuildClassicalStructure()` → `bool` — build classical structure (membrane + LT-zone + receptor)

### Usage in configurations

`NContinuesSAfferentNeuron` is used in sensory system experiments with continuous output:

- **Continuous sensory systems**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where continuous output is used for classical afferent neurons)
- **Classical modeling**: experiments with classic membranes and LT-zones

**Typical parameter values:**
- **StructureBuildMode**: 1 (classical structure)
- **MembraneClassName**: "NCSynNeuronMembrane" (classic membrane)
- **LTZoneClassName**: "NCLTZone" (classic LT-zone)
- **ExcGeneratorClassName**: "NCNeuronNegCGenerator" (classic generator)

**Features:**
- Classical structure: includes membrane, LT-zone, and receptor
- Continuous output: LT-zone produces continuous signal instead of discrete spikes
- Classic components: uses classic membranes and LT-zones for continuous modeling

### References

See [Literature-References.md](../Literature-References.md): **25**, **29**.

### See Also

- [`NAfferentNeuron`](NAfferentNeuron.md) — base afferent neuron
- [`NSAfferentNeuron`](NSAfferentNeuron.md) — classical afferent neuron
- [`NContinuesSimpleAfferentNeuron`](NContinuesSimpleAfferentNeuron.md) — continuous simple afferent neuron
- [`NCSynNeuronMembrane`](NCSynNeuronMembrane.md) — classic membrane optimized for synapses
- [`NCLTZone`](NCLTZone.md) — classic LT-zone
- [`NReceptor`](NReceptor.md) — receptor
- [Architecture.md](../Architecture.md) — library architecture
