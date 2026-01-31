# NPNeuron4x1 — пульсовый нейрон 4x1

## RU

### Назначение

**Класс**: `NPNeuron4x1` — конфигурационный вариант пульсового нейрона с четырьмя сомальными частями и одной дендритной частью мембраны.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPNeuron4x1", ...)`.  
**Storage-инстансы**: `ClassName = "NPNeuron4x1"` в `Bin/Configs/*/Model_*.xml`.

`NPNeuron4x1` является конфигурационным вариантом базового класса `NPulseNeuron` с предустановленными параметрами структурирования. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 4` — четыре сомальные части мембраны
- `NumDendriteMembraneParts = 1` — одна дендритная часть мембраны (обозначение "4x1" указывает на структуру: 4 сомы, 1 дендрит)
- `LTMembraneClassName = ""` — без LT-мембраны

Нейрон с такой структурой имеет четыре сомальные мембраны и одну дендритную мембрану, что позволяет моделировать нейроны с сложной сомальной структурой.

**Использование:** Моделирование нейронов со сложной сомальной структурой

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NPNeuron4x1 : configuration variant
    NPNeuron4x1 *-- NPulseMembraneCommon : SomaMembranes
    NPNeuron4x1 *-- NPulseMembraneCommon : DendriteMembrane
    NPNeuron4x1 *-- NPulseLTZoneCommon : LTZone
    class NPulseNeuron {
        +NumSomaMembraneParts : int
        +NumDendriteMembraneParts : int
        +LTMembraneClassName : string
    }
    class NPNeuron4x1 {
        +NumSomaMembraneParts : int = 4
        +NumDendriteMembraneParts : int = 1
        +LTMembraneClassName : string = ""
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NPNeuron4x1` — конфигурационный вариант с структурой 4x1

**Внутренняя структура:**
- **SomaMembranes** (`NPulseMembraneCommon[]`) — четыре сомальные мембраны
- **DendriteMembrane** (`NPulseMembraneCommon`) — одна дендритная мембрана
- **LTZone** (`NPulseLTZoneCommon`) — стандартная LT-зона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NPNeuron4x1
    participant SomaMembranes as NPulseMembraneCommon[]
    participant DendriteMembrane as NPulseMembraneCommon
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() (из NPulseNeuron)
    Storage->>Neuron: SetNumSomaMembraneParts(4)
    Storage->>Neuron: SetNumDendriteMembraneParts(1)
    Storage->>Neuron: SetLTMembraneClassName("")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    loop Для каждой сомы (4 части)
        Neuron->>SomaMembranes: CreateComponent("Soma" + i)
    end
    Neuron->>DendriteMembrane: CreateComponent("Dendrite0")
    Neuron->>LTZone: CreateComponent("LTZone")
    Neuron->>Neuron: CreateLinks()
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseNeuronCommon::ACalculate()
        Neuron->>DendriteMembrane: ACalculate()
        DendriteMembrane-->>SomaMembranes: Потенциал дендрита
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
1. **Создание**: `NPNeuron4x1` создается из `NPulseNeuron` с настройкой параметров
2. **Настройка**: Устанавливается четыре сомальные части и одна дендритная часть
3. **Сборка**: Автоматически создается структура нейрона с четырьмя сомами, дендритом и LT-зоной
4. **Расчет**: На каждом шаге рассчитываются дендрит, затем сомы, затем LT-зона

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка 4x1 структуры
    Configuring --> Set4x1Params: SetNumSomaMembraneParts(4)<br/>SetNumDendriteMembraneParts(1)
    Set4x1Params --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateSomas: Создание сомальных мембран (4 части)
    CreateSomas --> CreateDendrite: Создание дендритной мембраны
    CreateDendrite --> CreateLTZone: Создание LT-зоны
    CreateLTZone --> LinkComponents: Создание связей
    LinkComponents --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> DendriteCalc: Расчет дендритной мембраны
    DendriteCalc --> SomaCalc: Расчет сомальных мембран
    SomaCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для структуры 4x1
- **Set4x1Params** — установка параметров (4 сомы, 1 дендрит)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateSomas** — создание сомальных мембран
- **CreateDendrite** — создание дендритной мембраны
- **CreateLTZone** — создание LT-зоны
- **LinkComponents** — создание связей между компонентами
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **DendriteCalc** — расчет дендритной мембраны
- **SomaCalc** — расчет сомальных мембран
- **LTZoneCalc** — расчет LT-зоны
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> CalcDendrite[Расчет дендритной мембраны]
    CalcDendrite --> AggregateDendriteChannels[Агрегация токов дендрита]
    AggregateDendriteChannels --> IntegrateDendritePotential[Интеграция потенциала дендрита]
    IntegrateDendritePotential --> LoopSomas[Цикл по сомальным мембранам]
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
2. Расчет дендритной мембраны: агрегация токов и интеграция потенциала
3. Для каждой сомальной мембраны: агрегация токов от дендрита и собственных каналов, интеграция потенциала
4. Расчет LT-зоны: проверка порога
5. Генерация спайка при достижении порога

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NPNeuron4x1["NPNeuron4x1 Configuration"]
        Soma1[NPulseMembraneCommon<br/>Soma0]
        Soma2[NPulseMembraneCommon<br/>Soma1]
        Soma3[NPulseMembraneCommon<br/>Soma2]
        Soma4[NPulseMembraneCommon<br/>Soma3]
        DendriteMembrane[NPulseMembraneCommon<br/>Dendrite0]
        LTZone[NPulseLTZoneCommon<br/>LTZone]
    end
    
    subgraph External["Внешние компоненты"]
        Synapses[Синапсы]
        Channels[Каналы]
        PreNeurons[Пресинаптические нейроны]
    end
    
    BaseNeuron -->|конфигурируется как| NPNeuron4x1
    NPNeuron4x1 -->|создает| Soma1
    NPNeuron4x1 -->|создает| Soma2
    NPNeuron4x1 -->|создает| Soma3
    NPNeuron4x1 -->|создает| Soma4
    NPNeuron4x1 -->|создает| DendriteMembrane
    NPNeuron4x1 -->|создает| LTZone
    PreNeurons -->|Input| Synapses
    Synapses -->|ток| Channels
    Channels -->|ток| DendriteMembrane
    Channels -->|ток| Soma1
    Channels -->|ток| Soma2
    Channels -->|ток| Soma3
    Channels -->|ток| Soma4
    DendriteMembrane -->|потенциал| Soma1
    DendriteMembrane -->|потенциал| Soma2
    DendriteMembrane -->|потенциал| Soma3
    DendriteMembrane -->|потенциал| Soma4
    Soma1 -->|потенциал| LTZone
    Soma2 -->|потенциал| LTZone
    Soma3 -->|потенциал| LTZone
    Soma4 -->|потенциал| LTZone
    LTZone -->|Output| NPNeuron4x1
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPulseMembraneCommon` (4 сомальные мембраны, 1 дендритная мембрана), `NPulseLTZoneCommon` (LT-зона)
- **Внешние компоненты**: синапсы, каналы, пресинаптические нейроны

### Свойства

`NPNeuron4x1` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 4` — четыре сомальные части мембраны
- `NumDendriteMembraneParts = 1` — одна дендритная часть мембраны
- `LTMembraneClassName = ""` — без LT-мембраны

### Методы

`NPNeuron4x1` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание нейрона 4x1 в коде C++

```cpp
// Создание нейрона 4x1
auto neuron = storage->CreateComponent("NPNeuron4x1");
neuron->SetName("Neuron4x1");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает 4 сомальные мембраны, 1 дендритную мембрану, LT-зону)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Нейрон генерирует спайки при достижении порога
}
```

#### Пример 2: Конфигурация XML

```xml
<Neuron4x1_1 Class="NPNeuron4x1">
    <Parameters>
        <!-- Параметры наследуются от NPulseNeuron -->
    </Parameters>
</Neuron4x1_1>
```

### Использование в конфигурациях

`NPNeuron4x1` используется в экспериментах с нейронами, имеющими сложную сомальную структуру:

- **Сложная сома**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где требуется моделирование нейронов с множественными сомальными частями)
- **Сомальная интеграция**: эксперименты с интеграцией сигналов в нескольких сомальных частях

**Типичные значения параметров:**
- **NumSomaMembraneParts**: 4 (четыре сомальные части)
- **NumDendriteMembraneParts**: 1 (одна дендритная часть)
- **LTMembraneClassName**: "" (без LT-мембраны)

**Особенности:**
- Сложная сома: позволяет моделировать нейроны с множественными сомальными частями
- Интеграция сигналов: несколько сомальных частей интегрируют сигналы от дендрита
- Морфология: отражает биологическую структуру нейрона со сложной сомой

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NPNeuron`](NPNeuron.md) — базовый пульсовый нейрон
- [`NPNeuron1x4`](NPNeuron1x4.md) — нейрон 1x4
- [`NPNeuron4x4`](NPNeuron4x4.md) — нейрон 4x4
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPNeuron4x1` — configuration variant of spiking neuron with four soma parts and one dendrite part.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPNeuron4x1", ...)`.  
**Instances**: `ClassName = "NPNeuron4x1"` in `Bin/Configs/*/Model_*.xml`.

`NPNeuron4x1` is a configuration variant of the base class `NPulseNeuron` with preset structuring parameters. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 4` — four soma membrane parts
- `NumDendriteMembraneParts = 1` — one dendrite membrane part (notation "4x1" indicates structure: 4 somas, 1 dendrite)
- `LTMembraneClassName = ""` — without LT-membrane

Neuron with such structure has four soma membranes and one dendrite membrane, allowing modeling of neurons with complex soma structure.

**Usage:** Modeling neurons with complex soma structure

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NPNeuron4x1 : configuration variant
    NPNeuron4x1 *-- NPulseMembraneCommon : SomaMembranes
    NPNeuron4x1 *-- NPulseMembraneCommon : DendriteMembrane
    NPNeuron4x1 *-- NPulseLTZoneCommon : LTZone
    class NPNeuron4x1 {
        +NumSomaMembraneParts : int = 4
        +NumDendriteMembraneParts : int = 1
        +LTMembraneClassName : string = ""
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NPNeuron4x1
    participant SomaMembranes as NPulseMembraneCommon[]
    participant DendriteMembrane as NPulseMembraneCommon
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    loop For each soma (4 parts)
        Neuron->>SomaMembranes: CreateComponent()
    end
    Neuron->>DendriteMembrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>DendriteMembrane: ACalculate()
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
    CreateSomas --> CreateDendrite: Create dendrite membrane
    CreateDendrite --> CreateLTZone: Create LT-zone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> DendriteCalc: Calculate dendrite
    DendriteCalc --> SomaCalc: Calculate somas
    SomaCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcDendrite[Calculate dendrite]
    CalcDendrite --> LoopSomas[Loop through soma membranes]
    LoopSomas --> CalcSoma[Calculate soma]
    CalcSoma --> CheckMore{More somas?}
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
    
    subgraph NPNeuron4x1["NPNeuron4x1 Configuration"]
        Soma1[NPulseMembraneCommon]
        Soma2[NPulseMembraneCommon]
        Soma3[NPulseMembraneCommon]
        Soma4[NPulseMembraneCommon]
        DendriteMembrane[NPulseMembraneCommon]
        LTZone[NPulseLTZoneCommon]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses]
        PreNeurons[Presynaptic Neurons]
    end
    
    BaseNeuron -->|configured as| NPNeuron4x1
    NPNeuron4x1 -->|creates| Soma1
    NPNeuron4x1 -->|creates| Soma2
    NPNeuron4x1 -->|creates| Soma3
    NPNeuron4x1 -->|creates| Soma4
    NPNeuron4x1 -->|creates| DendriteMembrane
    PreNeurons -->|Input| Synapses
    Synapses -->|current| DendriteMembrane
    DendriteMembrane -->|potential| Soma1
    Soma1 -->|potential| LTZone
```

### Properties

`NPNeuron4x1` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 4` — four soma membrane parts
- `NumDendriteMembraneParts = 1` — one dendrite membrane part
- `LTMembraneClassName = ""` — without LT-membrane

### Methods

`NPNeuron4x1` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NPNeuron4x1` is used in experiments with neurons having complex soma structure:

- **Complex soma**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where modeling of neurons with multiple soma parts is required)
- **Soma integration**: experiments with signal integration in multiple soma parts

**Typical parameter values:**
- **NumSomaMembraneParts**: 4 (four soma parts)
- **NumDendriteMembraneParts**: 1 (one dendrite part)
- **LTMembraneClassName**: "" (without LT-membrane)

**Features:**
- Complex soma: allows modeling neurons with multiple soma parts
- Signal integration: multiple soma parts integrate signals from dendrite
- Morphology: reflects biological neuron structure with complex soma

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NPNeuron`](NPNeuron.md) — base spiking neuron
- [`NPNeuron1x4`](NPNeuron1x4.md) — neuron 1x4
- [`NPNeuron4x4`](NPNeuron4x4.md) — neuron 4x4
- [Architecture.md](../Architecture.md) — library architecture
