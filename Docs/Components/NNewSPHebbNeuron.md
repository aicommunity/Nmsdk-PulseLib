# NNewSPHebbNeuron — новый мелкий импульсный нейрон с синапсами Хебба

## RU

### Назначение

**Класс**: `NNewSPHebbNeuron` — конфигурационный вариант нового мелкого импульсного нейрона с синапсами Хебба и улучшенной архитектурой.  
**Аббревиатура**: `Hebb` — **Hebb**ian (геббовская пластичность, обучение по правилу Хебба).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSPHebbNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NNewSPHebbNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NNewSPHebbNeuron` является конфигурационным вариантом базового класса `NPulseNeuron` с мембраной, поддерживающей синапсы Хебба, и новой архитектурой. Создается из `NPHebbNeuron` с настройками:
- `NumSomaMembraneParts = 1` — одна часть сомы
- `MembraneClassName = "NPNeuronHebbMembrane"` — мембрана с поддержкой синапсов Хебба
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — новая LT-мембрана нейрона
- `LTZoneClassName = "NPLTZone"` — стандартная LT-зона

Комбинирует возможности новой архитектуры (улучшенные мембраны) и обучения Хебба (пластичность синапсов).

**Использование:** Эксперименты с новыми мелкими нейронами и обучением Хебба

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NNewSPHebbNeuron : configuration variant
    NNewSPHebbNeuron *-- NPNeuronHebbMembrane : PulseMembrane
    NNewSPHebbNeuron *-- NPLTZoneNeuronMembrane : LTMembrane
    NNewSPHebbNeuron *-- NPLTZone : LTZone
    NPNeuronHebbMembrane *-- NPulseHebbSynapse : Synapses
    class NPulseNeuron {
        +MembraneClassName : string
        +LTMembraneClassName : string
        +LTZoneClassName : string
        +NumSomaMembraneParts : int
    }
    class NNewSPHebbNeuron {
        +MembraneClassName : string = "NPNeuronHebbMembrane"
        +LTMembraneClassName : string = "NPLTZoneNeuronMembrane"
        +LTZoneClassName : string = "NPLTZone"
        +NumSomaMembraneParts : int = 1
    }
    class NPulseHebbSynapse {
        +Min : double
        +Mout : double
        +Md : double
        +Kin : double
        +Kout : double
        +GdGain : double
        +GsGain : double
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NNewSPHebbNeuron` — конфигурационный вариант с новой архитектурой и синапсами Хебба

**Внутренняя структура:**
- **PulseMembrane** (`NPNeuronHebbMembrane`) — мембрана с поддержкой синапсов Хебба
- **LTMembrane** (`NPLTZoneNeuronMembrane`) — новая LT-мембрана нейрона
- **LTZone** (`NPLTZone`) — стандартная LT-зона
- **Synapses** (`NPulseHebbSynapse[]`) — синапсы Хебба

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NNewSPHebbNeuron
    participant Membrane as NPNeuronHebbMembrane
    participant LTMembrane as NPLTZoneNeuronMembrane
    participant LTZone as NPLTZone
    participant Synapse as NPulseHebbSynapse
    participant PreNeuron as Пресинаптический нейрон
    
    Storage->>Neuron: New() (из NPHebbNeuron)
    Storage->>Neuron: SetMembraneClassName("NPNeuronHebbMembrane")
    Storage->>Neuron: SetLTMembraneClassName("NPLTZoneNeuronMembrane")
    Storage->>Neuron: SetLTZoneClassName("NPLTZone")
    Storage->>Neuron: SetNumSomaMembraneParts(1)
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    Neuron->>Membrane: CreateComponent("PulseMembrane", "NPNeuronHebbMembrane")
    Neuron->>LTMembrane: CreateComponent("LTMembrane", "NPLTZoneNeuronMembrane")
    Neuron->>LTZone: CreateComponent("LTZone", "NPLTZone")
    Membrane->>Synapse: CreateComponent("ExcSynapse1", "NPulseHebbSynapse")
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        PreNeuron->>Synapse: Input (спайк)
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseNeuronCommon::ACalculate()
        Neuron->>Membrane: ACalculate()
        Membrane->>Synapse: ACalculate()
        Synapse->>Synapse: Обновление Win, Wout, Gd, Gs
        Synapse->>Synapse: G = Gd*GdGain + GsSum*GsGain
        Synapse->>Synapse: Output *= (1.0 + G)
        Synapse-->>Membrane: Output (модифицированный ток)
        Membrane->>Membrane: Расчет мембраны
        Membrane-->>LTMembrane: Выходной сигнал мембраны
        LTMembrane->>LTMembrane: Расчет LT-мембраны
        LTMembrane-->>LTZone: Выходной сигнал LT-мембраны
        LTZone->>Synapse: InputLTZoneFeedbackSignal = Output
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (спайк)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Создание**: `NNewSPHebbNeuron` создается из `NPHebbNeuron` с настройкой параметров
2. **Настройка**: Устанавливается новая архитектура мембраны и LT-зоны, мембрана с поддержкой Хебба, одна часть сомы
3. **Сборка**: Автоматически создается структура нейрона с новой мембраной, LT-мембраной, LT-зоной и синапсами Хебба
4. **Расчет**: На каждом шаге рассчитываются синапсы Хебба (обновление весов), новая мембрана, LT-мембрана и LT-зона

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка New + Hebb архитектуры
    Configuring --> SetNewHebbParams: SetMembraneClassName("NPNeuronHebbMembrane")<br/>SetLTMembraneClassName("NPLTZoneNeuronMembrane")<br/>SetNumSomaMembraneParts(1)
    SetNewHebbParams --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateMembrane: Создание NPNeuronHebbMembrane
    CreateMembrane --> CreateLTMembrane: Создание NPLTZoneNeuronMembrane
    CreateLTMembrane --> CreateLTZone: Создание NPLTZone
    CreateLTZone --> CreateSynapses: Создание NPulseHebbSynapse
    CreateSynapses --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> SynapseCalc: Расчет синапсов Хебба
    SynapseCalc --> UpdateHebbWeights: Обновление весов Хебба
    UpdateHebbWeights --> MembraneCalc: Расчет новой мембраны
    MembraneCalc --> LTMembraneCalc: Расчет LT-мембраны
    LTMembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для новой архитектуры и Хебба
- **SetNewHebbParams** — установка параметров (мембрана с поддержкой Хебба, новая LT-мембрана, одна часть сомы)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateMembrane** — создание мембраны с поддержкой синапсов Хебба
- **CreateLTMembrane** — создание LT-мембраны
- **CreateLTZone** — создание LT-зоны
- **CreateSynapses** — создание синапсов Хебба
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **SynapseCalc** — расчет синапсов Хебба
- **UpdateHebbWeights** — обновление весов по правилу Хебба
- **MembraneCalc** — расчет новой мембраны
- **LTMembraneCalc** — расчет LT-мембраны
- **LTZoneCalc** — расчет LT-зоны
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> LoopSynapses[Цикл по синапсам]
    LoopSynapses --> CalcSynapse[Расчет NPulseHebbSynapse]
    CalcSynapse --> UpdateWin[Win += Kin*input - Min*Win]
    UpdateWin --> UpdateWout[Wout += Kout*ltzoneoutput - Mout*Wout]
    UpdateWout --> UpdateGd[Gd += Win*Wout - Md*Gd]
    UpdateGd --> UpdateGs[Gs[i] += motivation[i]*Gd - Ms[i]*Gs[i]]
    UpdateGs --> CalcG[G = Gd*GdGain + GsSum*GsGain]
    CalcG --> ModifyOutput[Output *= 1.0 + G]
    ModifyOutput --> CheckMoreSynapses{Есть еще синапсы?}
    CheckMoreSynapses -->|Да| LoopSynapses
    CheckMoreSynapses -->|Нет| CalcMembrane[Расчет новой мембраны]
    CalcMembrane --> CalcLTMembrane[Расчет LT-мембраны]
    CalcLTMembrane --> CalcLTZone[Расчет LT-зоны]
    CalcLTZone --> End([End])
```

**Алгоритм расчета:**
1. Вызов базового расчета нейрона
2. Для каждого синапса Хебба:
   - Обновление пресинаптической активности (`Win`)
   - Обновление постсинаптической активности (`Wout`)
   - Обновление корреляции (`Gd`)
   - Обновление мотивационных компонентов (`Gs`)
   - Вычисление общего влияния (`G`)
   - Модификация выходного тока (`Output *= 1.0 + G`)
3. Расчет новой мембраны с модифицированными токами
4. Расчет LT-мембраны
5. Расчет LT-зоны

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NNewSPHebbNeuron["NNewSPHebbNeuron Configuration"]
        Membrane["NPNeuronHebbMembrane<br/>PulseMembrane"]
        LTMembrane["NPLTZoneNeuronMembrane<br/>LTMembrane"]
        LTZone["NPLTZone<br/>LTZone"]
        Synapses["NPulseHebbSynapse<br/>ExcSynapse1..N"]
    end
    
    subgraph External["Внешние компоненты"]
        PreNeurons[Пресинаптические нейроны]
        MotivationSource[Источник мотивации]
    end
    
    BaseNeuron -->|конфигурируется как| NNewSPHebbNeuron
    NNewSPHebbNeuron -->|создает| Membrane
    NNewSPHebbNeuron -->|создает| LTMembrane
    NNewSPHebbNeuron -->|создает| LTZone
    NNewSPHebbNeuron -->|создает| Synapses
    PreNeurons -->|Input| Synapses
    MotivationSource -->|InputMotivation| Synapses
    LTZone -->|InputLTZoneFeedbackSignal| Synapses
    Synapses -->|Output<br/>модифицированный ток| Membrane
    Membrane -->|потенциал| LTMembrane
    LTMembrane -->|потенциал| LTZone
    LTZone -->|Output| NNewSPHebbNeuron
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPNeuronHebbMembrane` (мембрана), `NPLTZoneNeuronMembrane` (LT-мембрана), `NPLTZone` (LT-зона), `NPulseHebbSynapse` (синапсы)
- **Внешние компоненты**: пресинаптические нейроны, источник мотивации

### Свойства

`NNewSPHebbNeuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `MembraneClassName = "NPNeuronHebbMembrane"` — мембрана с поддержкой синапсов Хебба
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — новая LT-мембрана нейрона
- `LTZoneClassName = "NPLTZone"` — стандартная LT-зона
- `NumSomaMembraneParts = 1` — одна часть сомы

**Параметры синапсов Хебба (в NPulseHebbSynapse):**
- `Min` (double) — константа забывания пресинаптической активности
- `Mout` (double) — константа забывания постсинаптической активности
- `Md` (double) — константа забывания корреляции
- `Kin` (double) — коэффициент пресинаптической активности
- `Kout` (double) — коэффициент постсинаптической активности
- `GdGain` (double) — коэффициент усиления корреляционного компонента
- `GsGain` (double) — коэффициент усиления мотивационного компонента

### Методы

`NNewSPHebbNeuron` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание нового Hebb-нейрона в коде C++

```cpp
// Создание нового мелкого нейрона с синапсами Хебба
auto neuron = storage->CreateComponent("NNewSPHebbNeuron");
neuron->SetName("NewSPHebbNeuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NPNeuronHebbMembrane, NPLTZoneNeuronMembrane, NPLTZone, NPulseHebbSynapse)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Веса синапсов Хебба обновляются автоматически
}
```

#### Пример 2: Конфигурация XML

```xml
<NewSPHebbNeuron1 Class="NNewSPHebbNeuron">
    <Parameters>
        <!-- Параметры наследуются от NPulseNeuron -->
    </Parameters>
</NewSPHebbNeuron1>
```

### Использование в конфигурациях

`NNewSPHebbNeuron` используется в экспериментах с новой архитектурой и обучением Хебба:

- **Новая архитектура + Хебб**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где используется комбинация новой архитектуры и обучения Хебба)
- **Улучшенное моделирование**: эксперименты с более точными мембранами и пластичностью синапсов

**Типичные значения параметров:**
- **MembraneClassName**: "NPNeuronHebbMembrane" (мембрана с поддержкой Хебба)
- **LTMembraneClassName**: "NPLTZoneNeuronMembrane" (новая LT-мембрана)
- **LTZoneClassName**: "NPLTZone" (стандартная LT-зона)
- **NumSomaMembraneParts**: 1 (одна часть сомы для мелких нейронов)

**Особенности:**
- Новая архитектура: использует улучшенные мембраны для более точного моделирования
- LT-мембрана: отдельная мембрана для LT-зоны с оптимизированными каналами
- Обучение Хебба: синапсы автоматически обновляют веса на основе корреляции пре- и постсинаптической активности
- Комбинированная функциональность: объединяет возможности новой архитектуры и обучения Хебба

## Источники

См. [Literature-References.md](../Literature-References.md): **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NNewSPNeuron`](NNewSPNeuron.md) — новый мелкий нейрон
- [`NSPHebbNeuron`](NSPHebbNeuron.md) — мелкий нейрон с синапсами Хебба
- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — синапс Хебба
- [`NPNeuronHebbMembrane`](NPNeuronHebbMembrane.md) — мембрана с поддержкой синапсов Хебба
- [`NPLTZoneNeuronMembrane`](NPLTZoneNeuronMembrane.md) — мембрана нейрона с LT-зоной
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewSPHebbNeuron` — configuration variant of new small spiking neuron with Hebbian synapses and improved architecture.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewSPHebbNeuron", ...)`.  
**Instances**: `ClassName = "NNewSPHebbNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NNewSPHebbNeuron` is a configuration variant of the base class `NPulseNeuron` with membrane supporting Hebbian synapses and new architecture. Created from `NPHebbNeuron` with settings:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPNeuronHebbMembrane"` — membrane with Hebbian synapse support
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — new neuron LT-membrane
- `LTZoneClassName = "NPLTZone"` — standard LT-zone

Combines capabilities of new architecture (improved membranes) and Hebbian learning (synaptic plasticity).

**Usage:** Experiments with new small neurons and Hebbian learning

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSPHebbNeuron : configuration variant
    NNewSPHebbNeuron *-- NPNeuronHebbMembrane : PulseMembrane
    NNewSPHebbNeuron *-- NPLTZoneNeuronMembrane : LTMembrane
    NNewSPHebbNeuron *-- NPLTZone : LTZone
    NPNeuronHebbMembrane *-- NPulseHebbSynapse : Synapses
    class NNewSPHebbNeuron {
        +MembraneClassName : string = "NPNeuronHebbMembrane"
        +LTMembraneClassName : string = "NPLTZoneNeuronMembrane"
        +LTZoneClassName : string = "NPLTZone"
        +NumSomaMembraneParts : int = 1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NNewSPHebbNeuron
    participant Membrane as NPNeuronHebbMembrane
    participant LTMembrane as NPLTZoneNeuronMembrane
    participant LTZone as NPLTZone
    participant Synapse as NPulseHebbSynapse
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent()
    Neuron->>LTMembrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    Neuron->>Synapse: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Synapse: ACalculate()
        Synapse->>Synapse: Update Hebb weights
        Synapse-->>Membrane: Modified output
        Neuron->>Membrane: ACalculate()
        Neuron->>LTMembrane: ACalculate()
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
    Calculating --> SynapseCalc: Calculate Hebb synapses
    SynapseCalc --> UpdateWeights: Update Hebb weights
    UpdateWeights --> MembraneCalc: Calculate new membrane
    MembraneCalc --> LTMembraneCalc: Calculate LT-membrane
    LTMembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> LoopSynapses[Loop through synapses]
    LoopSynapses --> UpdateWin[Update Win]
    UpdateWin --> UpdateWout[Update Wout]
    UpdateWout --> UpdateGd[Update Gd]
    UpdateGd --> UpdateGs[Update Gs]
    UpdateGs --> CalcG[Calculate G]
    CalcG --> ModifyOutput[Modify Output]
    ModifyOutput --> CheckMore{More synapses?}
    CheckMore -->|Yes| LoopSynapses
    CheckMore -->|No| CalcMembrane[Calculate new membrane]
    CalcMembrane --> CalcLTMembrane[Calculate LT-membrane]
    CalcLTMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NNewSPHebbNeuron["NNewSPHebbNeuron Configuration"]
        Membrane[NPNeuronHebbMembrane]
        LTMembrane[NPLTZoneNeuronMembrane]
        LTZone[NPLTZone]
        Synapses[NPulseHebbSynapse]
    end
    
    subgraph External["External Components"]
        PreNeurons[Presynaptic Neurons]
        MotivationSource[Motivation Source]
    end
    
    BaseNeuron -->|configured as| NNewSPHebbNeuron
    NNewSPHebbNeuron -->|creates| Membrane
    NNewSPHebbNeuron -->|creates| LTMembrane
    NNewSPHebbNeuron -->|creates| LTZone
    NNewSPHebbNeuron -->|creates| Synapses
    PreNeurons -->|Input| Synapses
    MotivationSource -->|InputMotivation| Synapses
    LTZone -->|InputLTZoneFeedbackSignal| Synapses
    Synapses -->|Output| Membrane
    Membrane -->|potential| LTMembrane
    LTMembrane -->|potential| LTZone
```

### Properties

`NNewSPHebbNeuron` uses all properties of base class `NPulseNeuron` with parameters:
- `MembraneClassName = "NPNeuronHebbMembrane"` — membrane with Hebbian synapse support
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — new neuron LT-membrane
- `LTZoneClassName = "NPLTZone"` — standard LT-zone
- `NumSomaMembraneParts = 1` — one soma part

### Methods

`NNewSPHebbNeuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NNewSPHebbNeuron` is used in experiments with new architecture and Hebbian learning:

- **New architecture + Hebb**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where combination of new architecture and Hebbian learning is used)
- **Improved modeling**: experiments with more accurate membranes and synaptic plasticity

**Typical parameter values:**
- **MembraneClassName**: "NPNeuronHebbMembrane" (membrane with Hebb support)
- **LTMembraneClassName**: "NPLTZoneNeuronMembrane" (new LT-membrane)
- **LTZoneClassName**: "NPLTZone" (standard LT-zone)
- **NumSomaMembraneParts**: 1 (one soma part for small neurons)

**Features:**
- New architecture: uses improved membranes for more accurate modeling
- LT-membrane: separate membrane for LT-zone with optimized channels
- Hebbian learning: synapses automatically update weights based on pre- and postsynaptic activity correlation
- Combined functionality: combines capabilities of new architecture and Hebbian learning

### References

See [Literature-References.md](../Literature-References.md): **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NNewSPNeuron`](NNewSPNeuron.md) — new small neuron
- [`NSPHebbNeuron`](NSPHebbNeuron.md) — small neuron with Hebbian synapses
- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — Hebbian synapse
- [`NPNeuronHebbMembrane`](NPNeuronHebbMembrane.md) — membrane with Hebbian synapse support
- [`NPLTZoneNeuronMembrane`](NPLTZoneNeuronMembrane.md) — neuron membrane with LT-zone
- [Architecture.md](../Architecture.md) — library architecture
