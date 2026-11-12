# NSynSPNeuron — мелкий син-нейрон

## RU

### Назначение

**Класс**: `NSynSPNeuron` — конфигурационный вариант мелкого импульсного нейрона с оптимизированными синапсами.  
**Аббревиатуры**: `Syn` — **Syn**apse (синапс); `SP` — **S**pike **P**attern (паттерн спайков).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynSPNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NSynSPNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NSynSPNeuron` является конфигурационным вариантом базового класса `NPulseNeuron` с мембраной, оптимизированной для синапсов. Создается из `NPulseNeuron` с настройками:
- `LTMembraneClassName = ""` — без LT-мембраны
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов
- `NumSomaMembraneParts = 1` — одна часть сомы

Мембрана `NPSynNeuronMembrane` оптимизирована для эффективной обработки синапсов, что улучшает производительность при работе с большим количеством синаптических соединений.

**Использование:** Эксперименты с оптимизированными синапсами для мелких нейронов

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NSynSPNeuron : configuration variant
    NSynSPNeuron *-- NPSynNeuronMembrane : PulseMembrane
    NSynSPNeuron *-- NPulseLTZoneCommon : LTZone
    class NPulseNeuron {
        +MembraneClassName : string
        +LTMembraneClassName : string
        +NumSomaMembraneParts : int
    }
    class NSynSPNeuron {
        +MembraneClassName : string = "NPSynNeuronMembrane"
        +LTMembraneClassName : string = ""
        +NumSomaMembraneParts : int = 1
    }
    class NPSynNeuronMembrane {
        +SynapseClassName : string
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NSynSPNeuron` — конфигурационный вариант с оптимизированными синапсами

**Внутренняя структура:**
- **PulseMembrane** (`NPSynNeuronMembrane`) — мембрана, оптимизированная для синапсов
- **LTZone** (`NPulseLTZoneCommon`) — стандартная LT-зона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NSynSPNeuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    participant Synapses as NPulseSynapse[]
    
    Storage->>Neuron: New() (из NPulseNeuron)
    Storage->>Neuron: SetMembraneClassName("NPSynNeuronMembrane")
    Storage->>Neuron: SetLTMembraneClassName("")
    Storage->>Neuron: SetNumSomaMembraneParts(1)
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    Neuron->>Membrane: CreateComponent("PulseMembrane", "NPSynNeuronMembrane")
    Neuron->>LTZone: CreateComponent("LTZone")
    Membrane->>Synapses: CreateComponent("ExcSynapse1", "NPulseSynapse")
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseNeuronCommon::ACalculate()
        Neuron->>Membrane: ACalculate()
        Membrane->>Synapses: ACalculate() (оптимизированная обработка)
        Synapses-->>Membrane: Output (ток)
        Membrane->>Membrane: Интеграция токов
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (спайк)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Создание**: `NSynSPNeuron` создается из `NPulseNeuron` с настройкой параметров
2. **Настройка**: Устанавливается мембрана с оптимизацией для синапсов, одна часть сомы
3. **Сборка**: Автоматически создается структура нейрона с оптимизированной мембраной и LT-зоной
4. **Расчет**: На каждом шаге рассчитываются синапсы (оптимизированная обработка), мембрана и LT-зона

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка Syn параметров
    Configuring --> SetSynParams: SetMembraneClassName("NPSynNeuronMembrane")<br/>SetLTMembraneClassName("")<br/>SetNumSomaMembraneParts(1)
    SetSynParams --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateMembrane: Создание NPSynNeuronMembrane
    CreateMembrane --> CreateLTZone: Создание NPulseLTZoneCommon
    CreateLTZone --> CreateSynapses: Создание синапсов
    CreateSynapses --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> SynapseCalc: Расчет синапсов (оптимизированный)
    SynapseCalc --> MembraneCalc: Расчет мембраны
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для оптимизированных синапсов
- **SetSynParams** — установка параметров (мембрана с оптимизацией, одна часть сомы)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateMembrane** — создание мембраны с оптимизацией для синапсов
- **CreateLTZone** — создание LT-зоны
- **CreateSynapses** — создание синапсов
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **SynapseCalc** — расчет синапсов (оптимизированная обработка)
- **MembraneCalc** — расчет мембраны
- **LTZoneCalc** — расчет LT-зоны
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> CalcMembrane[Расчет NPSynNeuronMembrane]
    CalcMembrane --> LoopSynapses[Цикл по синапсам]
    LoopSynapses --> CalcSynapse["Расчет синапса<br/>оптимизированная обработка"]
    CalcSynapse --> CheckMoreSynapses{Есть еще синапсы?}
    CheckMoreSynapses -->|Да| LoopSynapses
    CheckMoreSynapses -->|Нет| AggregateCurrents[Агрегация токов]
    AggregateCurrents --> IntegratePotential[Интеграция мембранного потенциала]
    IntegratePotential --> CalcLTZone[Расчет LT-зоны]
    CalcLTZone --> CheckThreshold{Порог достигнут?}
    CheckThreshold -->|Да| GenerateSpike[Генерация спайка]
    CheckThreshold -->|Нет| UpdateOutput[Обновление Output]
    GenerateSpike --> UpdateOutput
    UpdateOutput --> End([End])
```

**Алгоритм расчета:**
1. Вызов базового расчета нейрона (`NPulseNeuronCommon::ACalculate`)
2. Расчет мембраны с оптимизированной обработкой синапсов
3. Для каждого синапса: оптимизированный расчет тока
4. Агрегация токов от всех синапсов
5. Интеграция мембранного потенциала
6. Расчет LT-зоны: проверка порога
7. Генерация спайка при достижении порога

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSynSPNeuron["NSynSPNeuron Configuration"]
        Membrane["NPSynNeuronMembrane<br/>PulseMembrane<br/>оптимизированная для синапсов"]
        LTZone["NPulseLTZoneCommon<br/>LTZone"]
        Synapses["NPulseSynapse<br/>ExcSynapse1..N"]
    end
    
    subgraph External["Внешние компоненты"]
        PreNeurons[Пресинаптические нейроны]
    end
    
    BaseNeuron -->|конфигурируется как| NSynSPNeuron
    NSynSPNeuron -->|создает| Membrane
    NSynSPNeuron -->|создает| LTZone
    NSynSPNeuron -->|создает| Synapses
    PreNeurons -->|Input| Synapses
    Synapses -->|ток<br/>оптимизированная обработка| Membrane
    Membrane -->|потенциал| LTZone
    LTZone -->|Output| NSynSPNeuron
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPSynNeuronMembrane` (мембрана с оптимизацией), `NPulseLTZoneCommon` (LT-зона), `NPulseSynapse` (синапсы)
- **Внешние компоненты**: пресинаптические нейроны

### Свойства

`NSynSPNeuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов
- `LTMembraneClassName = ""` — без LT-мембраны
- `NumSomaMembraneParts = 1` — одна часть сомы

### Методы

`NSynSPNeuron` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание мелкого син-нейрона в коде C++

```cpp
// Создание мелкого нейрона с оптимизированными синапсами
auto neuron = storage->CreateComponent("NSynSPNeuron");
neuron->SetName("SynSPNeuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NPSynNeuronMembrane, NPulseLTZoneCommon)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Нейрон генерирует спайки с оптимизированной обработкой синапсов
}
```

#### Пример 2: Конфигурация XML

```xml
<SynSPNeuron1 Class="NSynSPNeuron">
    <Parameters>
        <!-- Параметры наследуются от NPulseNeuron -->
    </Parameters>
</SynSPNeuron1>
```

### Использование в конфигурациях

`NSynSPNeuron` используется в экспериментах с оптимизированными синапсами для мелких нейронов:

- **Оптимизированные синапсы**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где требуется эффективная обработка синапсов)
- **Производительность**: эксперименты с большим количеством синаптических соединений

**Типичные значения параметров:**
- **MembraneClassName**: "NPSynNeuronMembrane" (мембрана с оптимизацией)
- **LTMembraneClassName**: "" (без LT-мембраны)
- **NumSomaMembraneParts**: 1 (одна часть сомы для мелких нейронов)

**Особенности:**
- Оптимизация синапсов: мембрана оптимизирована для эффективной обработки синапсов
- Производительность: улучшенная производительность при работе с большим количеством синапсов
- Мелкие нейроны: одна часть сомы для базовых экспериментов

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NSPNeuron`](NSPNeuron.md) — базовый мелкий нейрон
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — мембрана, оптимизированная для синапсов
- [`NSynLPNeuron`](NSynLPNeuron.md) — крупный син-нейрон
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSynSPNeuron` — configuration variant of small spiking neuron with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSynSPNeuron", ...)`.  
**Instances**: `ClassName = "NSynSPNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NSynSPNeuron` is a configuration variant of the base class `NPulseNeuron` with membrane optimized for synapses. Created from `NPulseNeuron` with settings:
- `LTMembraneClassName = ""` — without LT-membrane
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses
- `NumSomaMembraneParts = 1` — one soma part

Membrane `NPSynNeuronMembrane` is optimized for efficient synapse processing, improving performance when working with large numbers of synaptic connections.

**Usage:** Experiments with optimized synapses for small neurons

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NSynSPNeuron : configuration variant
    NSynSPNeuron *-- NPSynNeuronMembrane : PulseMembrane
    NSynSPNeuron *-- NPulseLTZoneCommon : LTZone
    class NSynSPNeuron {
        +MembraneClassName : string = "NPSynNeuronMembrane"
        +LTMembraneClassName : string = ""
        +NumSomaMembraneParts : int = 1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSynSPNeuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    participant Synapses as NPulseSynapse[]
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate()
        Membrane->>Synapses: ACalculate() (optimized)
        Neuron->>LTZone: ACalculate()
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateComponents: Create components
    CreateComponents --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> SynapseCalc: Calculate synapses (optimized)
    SynapseCalc --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcMembrane[Calculate membrane]
    CalcMembrane --> LoopSynapses[Loop through synapses]
    LoopSynapses --> CalcSynapse["Calculate synapse<br/>optimized processing"]
    CalcSynapse --> CheckMore{More synapses?}
    CheckMore -->|Yes| LoopSynapses
    CheckMore -->|No| AggregateCurrents[Aggregate currents]
    AggregateCurrents --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateSpike[Generate spike]
    CheckThreshold -->|No| End([End])
    GenerateSpike --> End
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSynSPNeuron["NSynSPNeuron Configuration"]
        Membrane["NPSynNeuronMembrane<br/>optimized for synapses"]
        LTZone[NPulseLTZoneCommon]
        Synapses[NPulseSynapse]
    end
    
    subgraph External["External Components"]
        PreNeurons[Presynaptic Neurons]
    end
    
    BaseNeuron -->|configured as| NSynSPNeuron
    NSynSPNeuron -->|creates| Membrane
    NSynSPNeuron -->|creates| LTZone
    NSynSPNeuron -->|creates| Synapses
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
```

### Properties

`NSynSPNeuron` uses all properties of base class `NPulseNeuron` with parameters:
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses
- `LTMembraneClassName = ""` — without LT-membrane
- `NumSomaMembraneParts = 1` — one soma part

### Methods

`NSynSPNeuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NSynSPNeuron` is used in experiments with optimized synapses for small neurons:

- **Optimized synapses**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where efficient synapse processing is required)
- **Performance**: experiments with large numbers of synaptic connections

**Typical parameter values:**
- **MembraneClassName**: "NPSynNeuronMembrane" (membrane with optimization)
- **LTMembraneClassName**: "" (without LT-membrane)
- **NumSomaMembraneParts**: 1 (one soma part for small neurons)

**Features:**
- Synapse optimization: membrane optimized for efficient synapse processing
- Performance: improved performance when working with large numbers of synapses
- Small neurons: one soma part for basic experiments

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NSPNeuron`](NSPNeuron.md) — base small neuron
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — membrane optimized for synapses
- [`NSynLPNeuron`](NSynLPNeuron.md) — large syn-neuron
- [Architecture.md](../Architecture.md) — library architecture
