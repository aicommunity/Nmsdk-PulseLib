# NPNeuron4x4 — пульсовый нейрон 4x4

## RU

### Назначение

**Класс**: `NPNeuron4x4` — конфигурационный вариант пульсового нейрона с четырьмя сомальными частями и четырьмя дендритными частями мембраны.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPNeuron4x4", ...)`.  
**Storage-инстансы**: `ClassName = "NPNeuron4x4"` в `Bin/Configs/*/Model_*.xml`.

`NPNeuron4x4` является конфигурационным вариантом базового класса `NPulseNeuron` с предустановленными параметрами структурирования. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 4` — четыре сомальные части мембраны
- `NumDendriteMembraneParts = 3` — три дендритные части мембраны (обозначение "4x4" указывает на структуру: 4 сомы, 4 дендрита)
- `LTMembraneClassName = ""` — без LT-мембраны

Нейрон с такой структурой имеет четыре сомальные мембраны и несколько дендритных мембран, что позволяет моделировать нейроны с наиболее сложной морфологией.

**Использование:** Моделирование нейронов с наиболее сложной структурой (сложная сома и разветвленные дендриты)

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NPNeuron4x4 : configuration variant
    NPNeuron4x4 *-- NPulseMembraneCommon : SomaMembranes
    NPNeuron4x4 *-- NPulseMembraneCommon : DendriteMembranes
    NPNeuron4x4 *-- NPulseLTZoneCommon : LTZone
    class NPulseNeuron {
        +NumSomaMembraneParts : int
        +NumDendriteMembraneParts : int
        +LTMembraneClassName : string
    }
    class NPNeuron4x4 {
        +NumSomaMembraneParts : int = 4
        +NumDendriteMembraneParts : int = 3
        +LTMembraneClassName : string = ""
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NPNeuron4x4` — конфигурационный вариант с структурой 4x4

**Внутренняя структура:**
- **SomaMembranes** (`NPulseMembraneCommon[]`) — четыре сомальные мембраны
- **DendriteMembranes** (`NPulseMembraneCommon[]`) — три дендритные мембраны
- **LTZone** (`NPulseLTZoneCommon`) — стандартная LT-зона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NPNeuron4x4
    participant SomaMembranes as NPulseMembraneCommon[]
    participant DendriteMembranes as NPulseMembraneCommon[]
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() (из NPulseNeuron)
    Storage->>Neuron: SetNumSomaMembraneParts(4)
    Storage->>Neuron: SetNumDendriteMembraneParts(3)
    Storage->>Neuron: SetLTMembraneClassName("")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    loop Для каждой сомы (4 части)
        Neuron->>SomaMembranes: CreateComponent("Soma" + i)
    end
    loop Для каждого дендрита (3 части)
        Neuron->>DendriteMembranes: CreateComponent("Dendrite" + i)
    end
    Neuron->>LTZone: CreateComponent("LTZone")
    Neuron->>Neuron: CreateLinks()
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseNeuronCommon::ACalculate()
        loop Для каждого дендрита
            Neuron->>DendriteMembranes: ACalculate()
            DendriteMembranes-->>SomaMembranes: Потенциал дендрита
        end
        loop Для каждой сомы
            Neuron->>SomaMembranes: ACalculate()
            SomaMembranes->>SomaMembranes: Интеграция токов
        end
        SomaMembranes-->>LTZone: Выходной сигнал сом
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (спайк)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Создание**: `NPNeuron4x4` создается из `NPulseNeuron` с настройкой параметров
2. **Настройка**: Устанавливается четыре сомальные части и три дендритные части
3. **Сборка**: Автоматически создается структура нейрона с четырьмя сомами, тремя дендритами и LT-зоной
4. **Расчет**: На каждом шаге рассчитываются дендриты, затем сомы, затем LT-зона

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка 4x4 структуры
    Configuring --> Set4x4Params: SetNumSomaMembraneParts(4)<br/>SetNumDendriteMembraneParts(3)
    Set4x4Params --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateSomas: Создание сомальных мембран (4 части)
    CreateSomas --> CreateDendrites: Создание дендритных мембран (3 части)
    CreateDendrites --> CreateLTZone: Создание LT-зоны
    CreateLTZone --> LinkComponents: Создание связей
    LinkComponents --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> DendriteCalc: Расчет дендритных мембран
    DendriteCalc --> SomaCalc: Расчет сомальных мембран
    SomaCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для структуры 4x4
- **Set4x4Params** — установка параметров (4 сомы, 3 дендрита)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateSomas** — создание сомальных мембран
- **CreateDendrites** — создание дендритных мембран
- **CreateLTZone** — создание LT-зоны
- **LinkComponents** — создание связей между компонентами
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **DendriteCalc** — расчет дендритных мембран
- **SomaCalc** — расчет сомальных мембран
- **LTZoneCalc** — расчет LT-зоны
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> LoopDendrites[Цикл по дендритным мембранам]
    LoopDendrites --> CalcDendrite[Расчет дендритной мембраны]
    CalcDendrite --> AggregateDendriteChannels[Агрегация токов дендрита]
    AggregateDendriteChannels --> IntegrateDendritePotential[Интеграция потенциала дендрита]
    IntegrateDendritePotential --> CheckMoreDendrites{Есть еще дендриты?}
    CheckMoreDendrites -->|Да| LoopDendrites
    CheckMoreDendrites -->|Нет| LoopSomas[Цикл по сомальным мембранам]
    LoopSomas --> CalcSoma[Расчет сомальной мембраны]
    CalcSoma --> AggregateSomaChannels[Агрегация токов сомы]
    AggregateSomaChannels --> IntegrateSomaPotential[Интеграция потенциала сомы]
    IntegrateSomaPotential --> CheckMoreSomas{Есть еще сомы?}
    CheckMoreSomas -->|Да| LoopSomas
    CheckMoreSomas -->|Нет| CalcLTZone[Расчет LT-зоны]
    CalcLTZone --> CheckThreshold{Potential >= Threshold?}
    CheckThreshold -->|Да| GenerateSpike[Генерация спайка]
    CheckThreshold -->|Нет| NoSpike[Без спайка]
    GenerateSpike --> End([End])
    NoSpike --> End
```

**Алгоритм расчета:**
1. Вызов базового расчета нейрона
2. Для каждой дендритной мембраны: расчет токов и интеграция потенциала
3. Для каждой сомальной мембраны: агрегация токов от дендритов и собственных каналов, интеграция потенциала
4. Расчет LT-зоны: проверка порога
5. Генерация спайка при достижении порога

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NPNeuron4x4["NPNeuron4x4 Configuration"]
        Soma1[NPulseMembraneCommon<br/>Soma0]
        Soma2[NPulseMembraneCommon<br/>Soma1]
        Soma3[NPulseMembraneCommon<br/>Soma2]
        Soma4[NPulseMembraneCommon<br/>Soma3]
        Dendrite1[NPulseMembraneCommon<br/>Dendrite0]
        Dendrite2[NPulseMembraneCommon<br/>Dendrite1]
        Dendrite3[NPulseMembraneCommon<br/>Dendrite2]
        LTZone[NPulseLTZoneCommon<br/>LTZone]
    end
    
    subgraph External["Внешние компоненты"]
        Synapses[Синапсы]
        Channels[Каналы]
        PreNeurons[Пресинаптические нейроны]
    end
    
    BaseNeuron -->|конфигурируется как| NPNeuron4x4
    NPNeuron4x4 -->|создает| Soma1
    NPNeuron4x4 -->|создает| Soma2
    NPNeuron4x4 -->|создает| Soma3
    NPNeuron4x4 -->|создает| Soma4
    NPNeuron4x4 -->|создает| Dendrite1
    NPNeuron4x4 -->|создает| Dendrite2
    NPNeuron4x4 -->|создает| Dendrite3
    NPNeuron4x4 -->|создает| LTZone
    PreNeurons -->|Input| Synapses
    Synapses -->|ток| Channels
    Channels -->|ток| Dendrite1
    Channels -->|ток| Dendrite2
    Channels -->|ток| Dendrite3
    Channels -->|ток| Soma1
    Channels -->|ток| Soma2
    Channels -->|ток| Soma3
    Channels -->|ток| Soma4
    Dendrite1 -->|потенциал| Soma1
    Dendrite2 -->|потенциал| Soma2
    Dendrite3 -->|потенциал| Soma3
    Dendrite1 -->|потенциал| Soma4
    Soma1 -->|потенциал| LTZone
    Soma2 -->|потенциал| LTZone
    Soma3 -->|потенциал| LTZone
    Soma4 -->|потенциал| LTZone
    LTZone -->|Output| NPNeuron4x4
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPulseMembraneCommon` (4 сомальные мембраны, 3 дендритные мембраны), `NPulseLTZoneCommon` (LT-зона)
- **Внешние компоненты**: синапсы, каналы, пресинаптические нейроны

### Свойства

`NPNeuron4x4` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 4` — четыре сомальные части мембраны
- `NumDendriteMembraneParts = 3` — три дендритные части мембраны
- `LTMembraneClassName = ""` — без LT-мембраны

### Методы

`NPNeuron4x4` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание нейрона 4x4 в коде C++

```cpp
// Создание нейрона 4x4
auto neuron = storage->CreateComponent("NPNeuron4x4");
neuron->SetName("Neuron4x4");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает 4 сомальные мембраны, 3 дендритные мембраны, LT-зону)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Нейрон генерирует спайки при достижении порога
}
```

#### Пример 2: Конфигурация XML

```xml
<Neuron4x4_1 Class="NPNeuron4x4">
    <Parameters>
        <!-- Параметры наследуются от NPulseNeuron -->
    </Parameters>
</Neuron4x4_1>
```

### Использование в конфигурациях

`NPNeuron4x4` используется в экспериментах с нейронами, имеющими наиболее сложную структуру:

- **Сложная морфология**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где требуется моделирование нейронов с наиболее сложной структурой)
- **Комплексная интеграция**: эксперименты с интеграцией сигналов от множественных дендритов в множественные сомы

**Типичные значения параметров:**
- **NumSomaMembraneParts**: 4 (четыре сомальные части)
- **NumDendriteMembraneParts**: 3 (три дендритные части)
- **LTMembraneClassName**: "" (без LT-мембраны)

**Особенности:**
- Наиболее сложная структура: позволяет моделировать нейроны с множественными сомами и дендритами
- Комплексная интеграция: множественные дендриты интегрируют сигналы и передают их в множественные сомы
- Морфология: отражает биологическую структуру нейрона с наиболее сложной морфологией

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NPNeuron`](NPNeuron.md) — базовый пульсовый нейрон
- [`NPNeuron1x4`](NPNeuron1x4.md) — нейрон 1x4
- [`NPNeuron4x1`](NPNeuron4x1.md) — нейрон 4x1
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPNeuron4x4` — configuration variant of spiking neuron with four soma parts and four dendrite parts.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPNeuron4x4", ...)`.  
**Instances**: `ClassName = "NPNeuron4x4"` in `Bin/Configs/*/Model_*.xml`.

`NPNeuron4x4` is a configuration variant of the base class `NPulseNeuron` with preset structuring parameters. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 4` — four soma membrane parts
- `NumDendriteMembraneParts = 3` — three dendrite membrane parts (notation "4x4" indicates structure: 4 somas, 4 dendrites)
- `LTMembraneClassName = ""` — without LT-membrane

Neuron with such structure has four soma membranes and several dendrite membranes, allowing modeling of neurons with most complex morphology.

**Usage:** Modeling neurons with most complex structure (complex soma and branched dendrites)

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NPNeuron4x4 : configuration variant
    NPNeuron4x4 *-- NPulseMembraneCommon : SomaMembranes
    NPNeuron4x4 *-- NPulseMembraneCommon : DendriteMembranes
    NPNeuron4x4 *-- NPulseLTZoneCommon : LTZone
    class NPNeuron4x4 {
        +NumSomaMembraneParts : int = 4
        +NumDendriteMembraneParts : int = 3
        +LTMembraneClassName : string = ""
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NPNeuron4x4
    participant SomaMembranes as NPulseMembraneCommon[]
    participant DendriteMembranes as NPulseMembraneCommon[]
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    loop For each soma (4 parts)
        Neuron->>SomaMembranes: CreateComponent()
    end
    loop For each dendrite (3 parts)
        Neuron->>DendriteMembranes: CreateComponent()
    end
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>DendriteMembranes: ACalculate()
        Neuron->>SomaMembranes: ACalculate()
        Neuron->>LTZone: ACalculate()
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateSomas: Create soma membranes
    CreateSomas --> CreateDendrites: Create dendrite membranes
    CreateDendrites --> CreateLTZone: Create LT-zone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> DendriteCalc: Calculate dendrites
    DendriteCalc --> SomaCalc: Calculate somas
    SomaCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> LoopDendrites[Loop through dendrite membranes]
    LoopDendrites --> CalcDendrite[Calculate dendrite]
    CalcDendrite --> LoopSomas[Loop through soma membranes]
    LoopSomas --> CalcSoma[Calculate soma]
    CalcSoma --> CheckMore{More components?}
    CheckMore -->|Yes| LoopSomas
    CheckMore -->|No| CalcLTZone[Calculate LT-zone]
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
    
    subgraph NPNeuron4x4["NPNeuron4x4 Configuration"]
        Soma1[NPulseMembraneCommon]
        Soma2[NPulseMembraneCommon]
        Soma3[NPulseMembraneCommon]
        Soma4[NPulseMembraneCommon]
        Dendrite1[NPulseMembraneCommon]
        Dendrite2[NPulseMembraneCommon]
        Dendrite3[NPulseMembraneCommon]
        LTZone[NPulseLTZoneCommon]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses]
        PreNeurons[Presynaptic Neurons]
    end
    
    BaseNeuron -->|configured as| NPNeuron4x4
    NPNeuron4x4 -->|creates| Soma1
    NPNeuron4x4 -->|creates| Soma2
    NPNeuron4x4 -->|creates| Soma3
    NPNeuron4x4 -->|creates| Soma4
    NPNeuron4x4 -->|creates| Dendrite1
    NPNeuron4x4 -->|creates| Dendrite2
    NPNeuron4x4 -->|creates| Dendrite3
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Dendrite1
    Dendrite1 -->|potential| Soma1
    Soma1 -->|potential| LTZone
```

### Properties

`NPNeuron4x4` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 4` — four soma membrane parts
- `NumDendriteMembraneParts = 3` — three dendrite membrane parts
- `LTMembraneClassName = ""` — without LT-membrane

### Methods

`NPNeuron4x4` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NPNeuron4x4` is used in experiments with neurons having most complex structure:

- **Complex morphology**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where modeling of neurons with most complex structure is required)
- **Complex integration**: experiments with signal integration from multiple dendrites into multiple somas

**Typical parameter values:**
- **NumSomaMembraneParts**: 4 (four soma parts)
- **NumDendriteMembraneParts**: 3 (three dendrite parts)
- **LTMembraneClassName**: "" (without LT-membrane)

**Features:**
- Most complex structure: allows modeling neurons with multiple somas and dendrites
- Complex integration: multiple dendrites integrate signals and transmit them to multiple somas
- Morphology: reflects biological neuron structure with most complex morphology

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NPNeuron`](NPNeuron.md) — base spiking neuron
- [`NPNeuron1x4`](NPNeuron1x4.md) — neuron 1x4
- [`NPNeuron4x1`](NPNeuron4x1.md) — neuron 4x1
- [Architecture.md](../Architecture.md) — library architecture
