# NSynMotoneuron — син-мотонейрон

## RU

### Назначение

**Класс**: `NSynMotoneuron` — конфигурационный вариант мотонейрона с оптимизированными синапсами.  
**Аббревиатура**: `Syn` — **Syn**apse (синапс).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynMotoneuron", ...)`.  
**Storage-инстансы**: `ClassName = "NSynMotoneuron"` в `Bin/Configs/*/Model_*.xml`.

`NSynMotoneuron` является конфигурационным вариантом базового класса `NPulseNeuron` для моделирования мотонейронов (двигательных нейронов) с оптимизированными синапсами. Создается из `NPulseNeuron` с настройками:
- `LTMembraneClassName = ""` — без LT-мембраны
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов
- `NumSomaMembraneParts = 3` — три части сомы

Мотонейроны преобразуют входные сигналы в моторную активность для управления мышцами и движениями. Мембрана `NPSynNeuronMembrane` оптимизирована для эффективной обработки синапсов.

**Использование:** Моделирование двигательных систем с оптимизированными синапсами, управление мышцами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NSynMotoneuron : configuration variant
    NSynMotoneuron *-- NPSynNeuronMembrane : PulseMembrane
    NSynMotoneuron *-- NPulseLTZoneCommon : LTZone
    class NPulseNeuron {
        +MembraneClassName : string
        +LTMembraneClassName : string
        +NumSomaMembraneParts : int
    }
    class NSynMotoneuron {
        +MembraneClassName : string = "NPSynNeuronMembrane"
        +LTMembraneClassName : string = ""
        +NumSomaMembraneParts : int = 3
    }
    class NPSynNeuronMembrane {
        +SynapseClassName : string
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NSynMotoneuron` — конфигурационный вариант мотонейрона с оптимизированными синапсами

**Внутренняя структура:**
- **PulseMembrane** (`NPSynNeuronMembrane`) — мембрана, оптимизированная для синапсов (3 части сомы)
- **LTZone** (`NPulseLTZoneCommon`) — стандартная LT-зона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NSynMotoneuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    participant Muscle as NMuscle
    participant Synapses as NPulseSynapse[]
    
    Storage->>Neuron: New() (из NPulseNeuron)
    Storage->>Neuron: SetMembraneClassName("NPSynNeuronMembrane")
    Storage->>Neuron: SetLTMembraneClassName("")
    Storage->>Neuron: SetNumSomaMembraneParts(3)
    Storage->>Neuron: SetName("Motoneuron")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    Neuron->>Membrane: CreateComponent("PulseMembrane", "NPSynNeuronMembrane")
    Note over Membrane: Создание 3 частей сомы
    Neuron->>LTZone: CreateComponent("LTZone")
    Membrane->>Synapses: CreateComponent("ExcSynapse1", "NPulseSynapse")
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseNeuronCommon::ACalculate()
        Neuron->>Membrane: ACalculate() (3 части сомы)
        Membrane->>Synapses: ACalculate() (оптимизированная обработка)
        Synapses-->>Membrane: Output (ток)
        Membrane->>Membrane: Интеграция токов (3 части)
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (моторная активность)
        Neuron-->>Muscle: Output (управление мышцей)
    end
```

**Жизненный цикл:**
1. **Создание**: `NSynMotoneuron` создается из `NPulseNeuron` с настройкой параметров
2. **Настройка**: Устанавливается мембрана с оптимизацией для синапсов, три части сомы
3. **Сборка**: Автоматически создается структура мотонейрона с оптимизированной мембраной (3 части) и LT-зоной
4. **Расчет**: На каждом шаге рассчитываются синапсы (оптимизированная обработка), мембрана (3 части) и LT-зона, генерируется моторная активность

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка Syn мотонейрона
    Configuring --> SetSynMotoneuronParams: SetMembraneClassName("NPSynNeuronMembrane")<br/>SetLTMembraneClassName("")<br/>SetNumSomaMembraneParts(3)
    SetSynMotoneuronParams --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateMembrane: Создание NPSynNeuronMembrane (3 части)
    CreateMembrane --> CreateLTZone: Создание NPulseLTZoneCommon
    CreateLTZone --> CreateSynapses: Создание синапсов
    CreateSynapses --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> SynapseCalc: Расчет синапсов (оптимизированный)
    SynapseCalc --> MembraneCalc: Расчет мембраны (3 части)
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> CheckThreshold: Проверка порога
    CheckThreshold -->|Порог достигнут| MotorSpike: Генерация моторного спайка
    CheckThreshold -->|Порог не достигнут| Ready: Шаг завершен
    MotorSpike --> Ready
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для мотонейрона с оптимизированными синапсами
- **SetSynMotoneuronParams** — установка параметров (мембрана с оптимизацией, три части сомы)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateMembrane** — создание мембраны с тремя частями сомы
- **CreateLTZone** — создание LT-зоны
- **CreateSynapses** — создание синапсов
- **Built** — структура мотонейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет мотонейрона
- **SynapseCalc** — расчет синапсов (оптимизированная обработка)
- **MembraneCalc** — расчет мембраны (три части сомы)
- **LTZoneCalc** — расчет LT-зоны
- **CheckThreshold** — проверка достижения порога
- **MotorSpike** — генерация моторного спайка
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> CalcMembrane[Расчет NPSynNeuronMembrane (3 части)]
    CalcMembrane --> LoopSynapses[Цикл по синапсам]
    LoopSynapses --> CalcSynapse["Расчет синапса<br/>оптимизированная обработка"]
    CalcSynapse --> CheckMoreSynapses{Есть еще синапсы?}
    CheckMoreSynapses -->|Да| LoopSynapses
    CheckMoreSynapses -->|Нет| CalcMembranePart1[Расчет части 1 сомы]
    CalcMembranePart1 --> CalcMembranePart2[Расчет части 2 сомы]
    CalcMembranePart2 --> CalcMembranePart3[Расчет части 3 сомы]
    CalcMembranePart3 --> AggregateMembrane[Агрегация потенциалов]
    AggregateMembrane --> CalcLTZone[Расчет LT-зоны]
    CalcLTZone --> CheckThreshold{Potential >= Threshold?}
    CheckThreshold -->|Да| GenerateMotorSpike[Генерация моторного спайка]
    CheckThreshold -->|Нет| NoSpike[Без спайка]
    GenerateMotorSpike --> SetOutput[Output = PulseAmplitude]
    NoSpike --> SetOutputZero[Output = 0]
    SetOutput --> SendToMuscle[Передача в мышцу]
    SetOutputZero --> SendToMuscle
    SendToMuscle --> End([End])
```

**Алгоритм расчета:**
1. Вызов базового расчета нейрона (`NPulseNeuronCommon::ACalculate`)
2. Расчет мембраны с оптимизированной обработкой синапсов
3. Для каждого синапса: оптимизированный расчет тока
4. Расчет трех частей сомы: последовательный расчет каждой части
5. Агрегация потенциалов от всех частей мембраны
6. Расчет LT-зоны: проверка порога
7. Генерация моторного спайка при достижении порога
8. Передача моторной активности мышцам

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSynMotoneuron["NSynMotoneuron Configuration"]
        Membrane["NPSynNeuronMembrane<br/>PulseMembrane<br/>3 части сомы<br/>оптимизированная для синапсов"]
        LTZone["NPulseLTZoneCommon<br/>LTZone"]
        Synapses["NPulseSynapse<br/>ExcSynapse1..N"]
    end
    
    subgraph External["Внешние компоненты"]
        PreNeurons[Пресинаптические нейроны]
        Muscle[NMuscle]
    end
    
    BaseNeuron -->|конфигурируется как| NSynMotoneuron
    NSynMotoneuron -->|создает| Membrane
    NSynMotoneuron -->|создает| LTZone
    NSynMotoneuron -->|создает| Synapses
    PreNeurons -->|Input| Synapses
    Synapses -->|ток<br/>оптимизированная обработка| Membrane
    Membrane -->|потенциал| LTZone
    LTZone -->|Output<br/>моторная активность| Muscle
    LTZone -->|Output| NSynMotoneuron
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPSynNeuronMembrane` (мембрана с 3 частями сомы и оптимизацией), `NPulseLTZoneCommon` (LT-зона), `NPulseSynapse` (синапсы)
- **Внешние компоненты**: пресинаптические нейроны, мышцы

### Свойства

`NSynMotoneuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов
- `LTMembraneClassName = ""` — без LT-мембраны
- `NumSomaMembraneParts = 3` — три части сомы

### Методы

`NSynMotoneuron` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание син-мотонейрона в коде C++

```cpp
// Создание мотонейрона с оптимизированными синапсами
auto neuron = storage->CreateComponent("NSynMotoneuron");
neuron->SetName("SynMotoneuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NPSynNeuronMembrane с 3 частями сомы, NPulseLTZoneCommon)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Мотонейрон генерирует моторную активность с оптимизированной обработкой синапсов
}
```

#### Пример 2: Конфигурация XML

```xml
<SynMotoneuron1 Class="NSynMotoneuron">
    <Parameters>
        <!-- Параметры наследуются от NPulseNeuron -->
    </Parameters>
</SynMotoneuron1>
```

### Использование в конфигурациях

`NSynMotoneuron` используется в экспериментах с двигательными системами с оптимизированными синапсами:

- **Двигательные системы**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где требуется эффективная обработка синапсов для мотонейронов)
- **Управление мышцами**: эксперименты с управлением движением с оптимизированными синапсами

**Типичные значения параметров:**
- **MembraneClassName**: "NPSynNeuronMembrane" (мембрана с оптимизацией)
- **LTMembraneClassName**: "" (без LT-мембраны)
- **NumSomaMembraneParts**: 3 (три части сомы для мотонейронов)

**Особенности:**
- Оптимизация синапсов: мембрана оптимизирована для эффективной обработки синапсов
- Производительность: улучшенная производительность при работе с большим количеством синапсов
- Моторная активность: генерирует моторную активность для управления мышцами

## Источники

См. [Literature-References.md](../Literature-References.md): **19**, **20**, **21**, **22**, **28**, **31**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NMotoneuron`](NMotoneuron.md) — базовый мотонейрон
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — мембрана, оптимизированная для синапсов
- [`NSynSPNeuron`](NSynSPNeuron.md) — мелкий син-нейрон
- [`NSynLPNeuron`](NSynLPNeuron.md) — крупный син-нейрон
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSynMotoneuron` — configuration variant of motoneuron with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSynMotoneuron", ...)`.  
**Instances**: `ClassName = "NSynMotoneuron"` in `Bin/Configs/*/Model_*.xml`.

`NSynMotoneuron` is a configuration variant of the base class `NPulseNeuron` for modeling motoneurons (motor neurons) with optimized synapses. Created from `NPulseNeuron` with settings:
- `LTMembraneClassName = ""` — without LT-membrane
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses
- `NumSomaMembraneParts = 3` — three soma parts

Motoneurons convert input signals into motor activity for controlling muscles and movements. Membrane `NPSynNeuronMembrane` is optimized for efficient synapse processing.

**Usage:** Modeling motor systems with optimized synapses, muscle control

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NSynMotoneuron : configuration variant
    NSynMotoneuron *-- NPSynNeuronMembrane : PulseMembrane
    NSynMotoneuron *-- NPulseLTZoneCommon : LTZone
    class NSynMotoneuron {
        +MembraneClassName : string = "NPSynNeuronMembrane"
        +LTMembraneClassName : string = ""
        +NumSomaMembraneParts : int = 3
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSynMotoneuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    participant Muscle as NMuscle
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent() (3 soma parts)
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate() (3 parts)
        Neuron->>LTZone: ACalculate()
        Neuron-->>Muscle: Motor activity
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateMembrane: Create membrane (3 parts)
    CreateMembrane --> CreateLTZone: Create LT-zone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> SynapseCalc: Calculate synapses (optimized)
    SynapseCalc --> MembraneCalc: Calculate membrane (3 parts)
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> CheckThreshold: Check threshold
    CheckThreshold -->|Reached| MotorSpike: Generate motor spike
    CheckThreshold -->|Not reached| Ready: Step completed
    MotorSpike --> Ready
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcMembrane[Calculate membrane (3 parts)]
    CalcMembrane --> LoopSynapses[Loop through synapses]
    LoopSynapses --> CalcSynapse["Calculate synapse<br/>optimized"]
    CalcSynapse --> CalcMembraneParts[Calculate 3 soma parts]
    CalcMembraneParts --> AggregateMembrane[Aggregate potentials]
    AggregateMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateMotorSpike[Generate motor spike]
    CheckThreshold -->|No| End([End])
    GenerateMotorSpike --> SendToMuscle[Send to muscle]
    SendToMuscle --> End
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSynMotoneuron["NSynMotoneuron Configuration"]
        Membrane["NPSynNeuronMembrane<br/>3 soma parts<br/>optimized"]
        LTZone[NPulseLTZoneCommon]
        Synapses[NPulseSynapse]
    end
    
    subgraph External["External Components"]
        PreNeurons[Presynaptic Neurons]
        Muscle[NMuscle]
    end
    
    BaseNeuron -->|configured as| NSynMotoneuron
    NSynMotoneuron -->|creates| Membrane
    NSynMotoneuron -->|creates| LTZone
    NSynMotoneuron -->|creates| Synapses
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|motor activity| Muscle
```

### Properties

`NSynMotoneuron` uses all properties of base class `NPulseNeuron` with parameters:
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses
- `LTMembraneClassName = ""` — without LT-membrane
- `NumSomaMembraneParts = 3` — three soma parts

### Methods

`NSynMotoneuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NSynMotoneuron` is used in motor system experiments with optimized synapses:

- **Motor systems**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where efficient synapse processing is required for motoneurons)
- **Muscle control**: experiments with movement control with optimized synapses

**Typical parameter values:**
- **MembraneClassName**: "NPSynNeuronMembrane" (membrane with optimization)
- **LTMembraneClassName**: "" (without LT-membrane)
- **NumSomaMembraneParts**: 3 (three soma parts for motoneurons)

**Features:**
- Synapse optimization: membrane optimized for efficient synapse processing
- Performance: improved performance when working with large numbers of synapses
- Motor activity: generates motor activity for controlling muscles

### References

See [Literature-References.md](../Literature-References.md): **19**, **20**, **21**, **22**, **28**, **31**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NMotoneuron`](NMotoneuron.md) — base motoneuron
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — membrane optimized for synapses
- [`NSynSPNeuron`](NSynSPNeuron.md) — small syn-neuron
- [`NSynLPNeuron`](NSynLPNeuron.md) — large syn-neuron
- [Architecture.md](../Architecture.md) — library architecture
