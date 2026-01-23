# NContinuesSimpleAfferentNeuron — непрерывный простой афферентный нейрон

## RU

### Назначение

**Класс**: `NContinuesSimpleAfferentNeuron` — конфигурационный вариант простого афферентного нейрона с непрерывным выходом.  
**Префикс**: `NContinues` — **Continues** (Continuous, непрерывный вариант компонента).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSimpleAfferentNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NContinuesSimpleAfferentNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NContinuesSimpleAfferentNeuron` является конфигурационным вариантом базового класса `NAfferentNeuron` с простой структурой и непрерывным выходом. Создается из `NAfferentNeuron` с настройками:
- `StructureBuildMode = 2` — простая структура
- `LTZoneClassName = "NCSimpleLTZone"` — классическая простая LT-зона
- `ReceptorClassName = "NReceptor"` — рецептор
- Параметры рецептора: `ExpCoeff = 100`, `Gain = 2`, `MaxOutputRange = 1`, `OutputAdaptationMode = 0`

Простая структура включает только LT-зону и рецептор, без мембраны. Непрерывный выход означает, что LT-зона выдает непрерывный сигнал вместо дискретных спайков.

**Использование:** Моделирование простых сенсорных систем с непрерывным выходом

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NAfferentNeuron
    NAfferentNeuron <|.. NContinuesSimpleAfferentNeuron : configuration variant
    NContinuesSimpleAfferentNeuron *-- NCSimpleLTZone : LTZone
    NContinuesSimpleAfferentNeuron *-- NReceptor : Receptor
    class NAfferentNeuron {
        +StructureBuildMode : int
        +LTZoneClassName : string
        +ReceptorClassName : string
    }
    class NContinuesSimpleAfferentNeuron {
        +StructureBuildMode : int = 2
        +LTZoneClassName : string = "NCSimpleLTZone"
        +ReceptorClassName : string = "NReceptor"
    }
    class NReceptor {
        +ExpCoeff : double = 100
        +Gain : double = 2
        +MaxOutputRange : double = 1
        +OutputAdaptationMode : int = 0
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NAfferentNeuron` — базовый афферентный нейрон
- `NContinuesSimpleAfferentNeuron` — конфигурационный вариант с простой структурой и непрерывным выходом

**Внутренняя структура:**
- **LTZone** (`NCSimpleLTZone`) — классическая простая LT-зона с непрерывным выходом
- **Receptor** (`NReceptor`) — рецептор для приема внешних стимулов

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NContinuesSimpleAfferentNeuron
    participant Receptor as NReceptor
    participant LTZone as NCSimpleLTZone
    participant ExternalStimulus as Внешний стимул
    
    Storage->>Neuron: New() (из NAfferentNeuron)
    Storage->>Neuron: SetStructureBuildMode(2)
    Storage->>Neuron: SetLTZoneClassName("NCSimpleLTZone")
    Storage->>Neuron: SetReceptorClassName("NReceptor")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NAfferentNeuron::ABuild()
    Neuron->>Neuron: BuildSimpleStructure()
    Neuron->>Receptor: AddMissingComponent("Receptor", "NReceptor")
    Neuron->>LTZone: AddMissingComponent("LTZone", "NCSimpleLTZone")
    Neuron->>Neuron: CreateLink(Receptor->Output, LTZone->Input)
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        ExternalStimulus->>Receptor: Input (внешний стимул)
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NAfferentNeuron::ACalculate()
        Neuron->>Receptor: ACalculate()
        Receptor->>Receptor: Преобразование стимула
        Receptor-->>LTZone: Output (преобразованный сигнал)
        LTZone->>LTZone: Расчет классической простой LT-зоны
        LTZone->>LTZone: Непрерывный выход (не спайки)
        LTZone-->>Neuron: Output (непрерывный сигнал)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Создание**: `NContinuesSimpleAfferentNeuron` создается из `NAfferentNeuron` с настройкой параметров
2. **Настройка**: Устанавливается простая структура, классическая простая LT-зона
3. **Сборка**: Автоматически создается структура нейрона с LT-зоной и рецептором (без мембраны)
4. **Расчет**: На каждом шаге рассчитываются рецептор и LT-зона с непрерывным выходом

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка простой структуры
    Configuring --> SetSimpleParams: SetStructureBuildMode(2)<br/>SetLTZoneClassName("NCSimpleLTZone")<br/>SetReceptorClassName("NReceptor")
    SetSimpleParams --> Building: Build()
    Building --> BuildSimple: BuildSimpleStructure()
    BuildSimple --> CreateReceptor: Создание NReceptor
    CreateReceptor --> CreateLTZone: Создание NCSimpleLTZone
    CreateLTZone --> LinkComponents: CreateLink(Receptor->Output, LTZone->Input)
    LinkComponents --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Расчет рецептора
    ReceptorCalc --> LTZoneCalc: Расчет LT-зоны (непрерывный выход)
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для простой структуры
- **SetSimpleParams** — установка параметров (простая структура, классическая простая LT-зона)
- **Building** — выполняется сборка структуры
- **BuildSimple** — сборка простой структуры
- **CreateReceptor** — создание рецептора
- **CreateLTZone** — создание классической простой LT-зоны
- **LinkComponents** — создание связи между рецептором и LT-зоной
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **ReceptorCalc** — расчет рецептора
- **LTZoneCalc** — расчет LT-зоны с непрерывным выходом
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Расчет NReceptor]
    CalcReceptor --> TransformStimulus[Преобразование внешнего стимула]
    TransformStimulus --> AdaptInput[Адаптация входного диапазона]
    AdaptInput --> TransformOutput[Преобразование выходного сигнала]
    TransformOutput --> DirectToLTZone[Направление в NCSimpleLTZone]
    DirectToLTZone --> CalcLTZone[Расчет классической простой LT-зоны]
    CalcLTZone --> ContinuousOutput[Непрерывный выход]
    ContinuousOutput --> End([End])
```

**Алгоритм расчета:**
1. Расчет рецептора: преобразование внешнего стимула с адаптацией и преобразованием
2. Направление выходного сигнала рецептора в классическую простую LT-зону
3. Расчет классической простой LT-зоны с непрерывным выходом (не дискретные спайки)

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NAfferentNeuron["NAfferentNeuron Base"]
        BaseNeuron[NAfferentNeuron]
    end
    
    subgraph NContinuesSimpleAfferentNeuron["NContinuesSimpleAfferentNeuron Configuration"]
        Receptor[NReceptor<br/>Receptor]
        LTZone[NCSimpleLTZone<br/>LTZone]
    end
    
    subgraph External["Внешние компоненты"]
        ExternalStimulus[Внешний стимул]
    end
    
    BaseNeuron -->|конфигурируется как| NContinuesSimpleAfferentNeuron
    NContinuesSimpleAfferentNeuron -->|создает| Receptor
    NContinuesSimpleAfferentNeuron -->|создает| LTZone
    ExternalStimulus -->|входной сигнал| Receptor
    Receptor -->|выходной сигнал| LTZone
    LTZone -->|непрерывный выход| NContinuesSimpleAfferentNeuron
```

**Зависимости:**
- **Базовый класс**: `NAfferentNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NReceptor` (рецептор), `NCSimpleLTZone` (классическая простая LT-зона)
- **Внешние компоненты**: внешние стимулы

### Свойства

`NContinuesSimpleAfferentNeuron` использует все свойства базового класса `NAfferentNeuron` с параметрами:
- `StructureBuildMode = 2` — простая структура
- `LTZoneClassName = "NCSimpleLTZone"` — классическая простая LT-зона
- `ReceptorClassName = "NReceptor"` — рецептор

**Параметры рецептора (в NReceptor):**
- `ExpCoeff = 100` — экспоненциальный коэффициент
- `Gain = 2` — коэффициент усиления
- `MaxOutputRange = 1` — максимальный диапазон выходного сигнала
- `OutputAdaptationMode = 0` — режим адаптации выходного сигнала (линейный)

### Методы

`NContinuesSimpleAfferentNeuron` использует все методы базового класса `NAfferentNeuron`:
- `BuildSimpleStructure()` → `bool` — сборка простой структуры (LT-зона + рецептор)

### Примеры использования

#### Пример 1: Создание непрерывного простого афферентного нейрона в коде C++

```cpp
// Создание непрерывного простого афферентного нейрона
auto neuron = storage->CreateComponent("NContinuesSimpleAfferentNeuron");
neuron->SetName("ContinuesSimpleAfferentNeuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NCSimpleLTZone, NReceptor)
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
<ContinuesSimpleAfferentNeuron1 Class="NContinuesSimpleAfferentNeuron">
    <Parameters>
        <!-- Параметры наследуются от NAfferentNeuron -->
    </Parameters>
</ContinuesSimpleAfferentNeuron1>
```

### Использование в конфигурациях

`NContinuesSimpleAfferentNeuron` используется в экспериментах с простыми сенсорными системами с непрерывным выходом:

- **Непрерывные простые сенсорные системы**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где используется непрерывный выход для простых афферентных нейронов)
- **Упрощенное моделирование**: эксперименты с простой структурой без мембраны

**Типичные значения параметров:**
- **StructureBuildMode**: 2 (простая структура)
- **LTZoneClassName**: "NCSimpleLTZone" (классическая простая LT-зона)
- **ReceptorClassName**: "NReceptor" (рецептор)

**Особенности:**
- Простая структура: включает только LT-зону и рецептор, без мембраны
- Непрерывный выход: LT-зона выдает непрерывный сигнал вместо дискретных спайков
- Упрощенное моделирование: минимальная структура для базовых сенсорных систем

### См. также

- [`NAfferentNeuron`](NAfferentNeuron.md) — базовый афферентный нейрон
- [`NSimpleAfferentNeuron`](NSimpleAfferentNeuron.md) — простой афферентный нейрон
- [`NContinuesSAfferentNeuron`](NContinuesSAfferentNeuron.md) — непрерывный классический афферентный нейрон
- [`NCSimpleLTZone`](NCSimpleLTZone.md) — классическая простая LT-зона
- [`NReceptor`](NReceptor.md) — рецептор
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NContinuesSimpleAfferentNeuron` — configuration variant of simple afferent neuron with continuous output.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSimpleAfferentNeuron", ...)`.  
**Instances**: `ClassName = "NContinuesSimpleAfferentNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NContinuesSimpleAfferentNeuron` is a configuration variant of the base class `NAfferentNeuron` with simple structure and continuous output. Created from `NAfferentNeuron` with settings:
- `StructureBuildMode = 2` — simple structure
- `LTZoneClassName = "NCSimpleLTZone"` — classic simple LT-zone
- `ReceptorClassName = "NReceptor"` — receptor
- Receptor parameters: `ExpCoeff = 100`, `Gain = 2`, `MaxOutputRange = 1`, `OutputAdaptationMode = 0`

Simple structure includes only LT-zone and receptor, without membrane. Continuous output means LT-zone produces continuous signal instead of discrete spikes.

**Usage:** Modeling simple sensory systems with continuous output

### UML Class Diagram

```mermaid
classDiagram
    NAfferentNeuron <|.. NContinuesSimpleAfferentNeuron : configuration variant
    NContinuesSimpleAfferentNeuron *-- NCSimpleLTZone : LTZone
    NContinuesSimpleAfferentNeuron *-- NReceptor : Receptor
    class NContinuesSimpleAfferentNeuron {
        +StructureBuildMode : int = 2
        +LTZoneClassName : string = "NCSimpleLTZone"
        +ReceptorClassName : string = "NReceptor"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NContinuesSimpleAfferentNeuron
    participant Receptor as NReceptor
    participant LTZone as NCSimpleLTZone
    participant ExternalStimulus
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Receptor: AddMissingComponent()
    Neuron->>LTZone: AddMissingComponent()
    loop Each step
        ExternalStimulus->>Receptor: Input
        Storage->>Neuron: Calculate()
        Neuron->>Receptor: ACalculate()
        Receptor-->>LTZone: Output
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
    Building --> BuildSimple: Build simple structure
    BuildSimple --> CreateComponents: Create components
    CreateComponents --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceptorCalc: Calculate receptor
    ReceptorCalc --> LTZoneCalc: Calculate LT-zone (continuous)
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcReceptor[Calculate receptor]
    CalcReceptor --> TransformStimulus[Transform stimulus]
    TransformStimulus --> DirectToLTZone[Direct to LT-zone]
    DirectToLTZone --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> ContinuousOutput[Continuous output]
    ContinuousOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NAfferentNeuron["NAfferentNeuron Base"]
        BaseNeuron[NAfferentNeuron]
    end
    
    subgraph NContinuesSimpleAfferentNeuron["NContinuesSimpleAfferentNeuron Configuration"]
        Receptor[NReceptor]
        LTZone[NCSimpleLTZone]
    end
    
    subgraph External["External Components"]
        ExternalStimulus[External Stimulus]
    end
    
    BaseNeuron -->|configured as| NContinuesSimpleAfferentNeuron
    NContinuesSimpleAfferentNeuron -->|creates| Receptor
    NContinuesSimpleAfferentNeuron -->|creates| LTZone
    ExternalStimulus -->|input| Receptor
    Receptor -->|output| LTZone
    LTZone -->|continuous output| NContinuesSimpleAfferentNeuron
```

### Properties

`NContinuesSimpleAfferentNeuron` uses all properties of base class `NAfferentNeuron` with parameters:
- `StructureBuildMode = 2` — simple structure
- `LTZoneClassName = "NCSimpleLTZone"` — classic simple LT-zone
- `ReceptorClassName = "NReceptor"` — receptor

**Receptor parameters (in NReceptor):**
- `ExpCoeff = 100` — exponential coefficient
- `Gain = 2` — gain coefficient
- `MaxOutputRange = 1` — maximum output range
- `OutputAdaptationMode = 0` — output adaptation mode (linear)

### Methods

`NContinuesSimpleAfferentNeuron` uses all methods of base class `NAfferentNeuron`:
- `BuildSimpleStructure()` → `bool` — build simple structure (LT-zone + receptor)

### Usage in configurations

`NContinuesSimpleAfferentNeuron` is used in simple sensory system experiments with continuous output:

- **Continuous simple sensory systems**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where continuous output is used for simple afferent neurons)
- **Simplified modeling**: experiments with simple structure without membrane

**Typical parameter values:**
- **StructureBuildMode**: 2 (simple structure)
- **LTZoneClassName**: "NCSimpleLTZone" (classic simple LT-zone)
- **ReceptorClassName**: "NReceptor" (receptor)

**Features:**
- Simple structure: includes only LT-zone and receptor, without membrane
- Continuous output: LT-zone produces continuous signal instead of discrete spikes
- Simplified modeling: minimal structure for basic sensory systems

### See Also

- [`NAfferentNeuron`](NAfferentNeuron.md) — base afferent neuron
- [`NSimpleAfferentNeuron`](NSimpleAfferentNeuron.md) — simple afferent neuron
- [`NContinuesSAfferentNeuron`](NContinuesSAfferentNeuron.md) — continuous classical afferent neuron
- [`NCSimpleLTZone`](NCSimpleLTZone.md) — classic simple LT-zone
- [`NReceptor`](NReceptor.md) — receptor
- [Architecture.md](../Architecture.md) — library architecture
