# NSPHebbNeuron — мелкий импульсный нейрон с синапсами Хебба

## RU

### Назначение

**Класс**: `NSPHebbNeuron` — конфигурационный вариант мелкого импульсного нейрона с синапсами Хебба.  
**Аббревиатура**: `Hebb` — **Hebb**ian (геббовская пластичность, обучение по правилу Хебба).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSPHebbNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NSPHebbNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NSPHebbNeuron` является конфигурационным вариантом базового класса `NPulseNeuron` с мембраной, поддерживающей синапсы Хебба. Создается из `NPHebbNeuron` с настройками:
- `LTMembraneClassName = ""` — без LT-мембраны
- `NumSomaMembraneParts = 1` — одна часть сомы
- `MembraneClassName = "NPNeuronHebbMembrane"` — мембрана с поддержкой синапсов Хебба

Синапсы Хебба (`NPulseHebbSynapse`) реализуют механизм обучения Хебба с множественными выходами и мотивационными сигналами.

**Использование:** Моделирование нейронов с обучением Хебба, эксперименты с пластичностью синапсов

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NSPHebbNeuron : configuration variant
    NSPHebbNeuron *-- NPNeuronHebbMembrane : PulseMembrane
    NPNeuronHebbMembrane *-- NPulseHebbSynapse : Synapses
    NSPHebbNeuron *-- NPulseLTZoneCommon : LTZone
    class NPulseNeuron {
        +MembraneClassName : string
        +LTMembraneClassName : string
        +NumSomaMembraneParts : int
    }
    class NSPHebbNeuron {
        +MembraneClassName : string = "NPNeuronHebbMembrane"
        +LTMembraneClassName : string = ""
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
        +InputLTZoneFeedbackSignal : MDMatrix~double~
        +InputMotivation : MDMatrix~double~
        +Output1..Output6 : MDMatrix~double~
        -G : double
        -Gd : double
        -Gs : vector~double~
        -Win : double
        -Wout : double
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NSPHebbNeuron` — конфигурационный вариант с синапсами Хебба

**Внутренняя структура:**
- **PulseMembrane** (`NPNeuronHebbMembrane`) — мембрана с поддержкой синапсов Хебба
- **Synapses** (`NPulseHebbSynapse[]`) — синапсы Хебба с механизмом обучения
- **LTZone** (`NPulseLTZoneCommon`) — стандартная LT-зона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NSPHebbNeuron
    participant Membrane as NPNeuronHebbMembrane
    participant Synapse as NPulseHebbSynapse
    participant LTZone as NPulseLTZoneCommon
    participant PreNeuron as Пресинаптический нейрон
    
    Storage->>Neuron: New() (из NPHebbNeuron)
    Storage->>Neuron: SetMembraneClassName("NPNeuronHebbMembrane")
    Storage->>Neuron: SetNumSomaMembraneParts(1)
    Storage->>Neuron: SetLTMembraneClassName("")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    Neuron->>Membrane: CreateComponent("PulseMembrane", "NPNeuronHebbMembrane")
    Neuron->>LTZone: CreateComponent("LTZone")
    Neuron->>Synapse: CreateComponent("ExcSynapse1", "NPulseHebbSynapse")
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        PreNeuron->>Synapse: Input (спайк)
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseNeuronCommon::ACalculate()
        Neuron->>Membrane: ACalculate()
        Membrane->>Synapse: ACalculate()
        Synapse->>Synapse: Обновление Win (пресинаптическая активность)
        Synapse->>Synapse: Обновление Wout (постсинаптическая активность)
        Synapse->>Synapse: Обновление Gd (корреляция)
        Synapse->>Synapse: Обновление Gs (мотивационные компоненты)
        Synapse->>Synapse: G = Gd*GdGain + GsSum*GsGain
        Synapse->>Synapse: Output *= (1.0 + G)
        Synapse-->>Membrane: Output (модифицированный ток)
        Membrane->>Membrane: Расчет мембраны
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>Synapse: InputLTZoneFeedbackSignal = Output
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (спайк)
    end
```

**Жизненный цикл:**
1. **Создание**: `NSPHebbNeuron` создается из `NPHebbNeuron` с настройкой параметров
2. **Настройка**: Устанавливается мембрана с поддержкой синапсов Хебба, одна часть сомы
3. **Сборка**: Автоматически создается структура нейрона с мембраной и синапсами Хебба
4. **Расчет**: На каждом шаге рассчитываются синапсы Хебба (обновление весов), мембрана и LT-зона

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка Hebb параметров
    Configuring --> SetHebbParams: SetMembraneClassName("NPNeuronHebbMembrane")<br/>SetNumSomaMembraneParts(1)
    SetHebbParams --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateMembrane: Создание NPNeuronHebbMembrane
    CreateMembrane --> CreateLTZone: Создание NPulseLTZoneCommon
    CreateLTZone --> CreateSynapses: Создание NPulseHebbSynapse
    CreateSynapses --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> SynapseCalc: Расчет синапсов Хебба
    SynapseCalc --> UpdateHebbWeights: Обновление весов Хебба
    UpdateHebbWeights --> MembraneCalc: Расчет мембраны
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для Hebb-нейрона
- **SetHebbParams** — установка параметров (мембрана с поддержкой Хебба, одна часть сомы)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateMembrane** — создание мембраны с поддержкой синапсов Хебба
- **CreateLTZone** — создание LT-зоны
- **CreateSynapses** — создание синапсов Хебба
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **SynapseCalc** — расчет синапсов Хебба
- **UpdateHebbWeights** — обновление весов по правилу Хебба
- **MembraneCalc** — расчет мембраны
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
    CheckMoreSynapses -->|Нет| CalcMembrane[Расчет мембраны]
    CalcMembrane --> CalcLTZone[Расчет LT-зоны]
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
3. Расчет мембраны с модифицированными токами
4. Расчет LT-зоны

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSPHebbNeuron["NSPHebbNeuron Configuration"]
        Membrane["NPNeuronHebbMembrane<br/>PulseMembrane"]
        LTZone["NPulseLTZoneCommon<br/>LTZone"]
        Synapses["NPulseHebbSynapse<br/>ExcSynapse1..N"]
    end
    
    subgraph External["Внешние компоненты"]
        PreNeurons[Пресинаптические нейроны]
        MotivationSource[Источник мотивации]
    end
    
    BaseNeuron -->|конфигурируется как| NSPHebbNeuron
    NSPHebbNeuron -->|создает| Membrane
    NSPHebbNeuron -->|создает| LTZone
    NSPHebbNeuron -->|создает| Synapses
    PreNeurons -->|Input| Synapses
    MotivationSource -->|InputMotivation| Synapses
    LTZone -->|InputLTZoneFeedbackSignal| Synapses
    Synapses -->|Output<br/>модифицированный ток| Membrane
    Membrane -->|потенциал| LTZone
    LTZone -->|Output| NSPHebbNeuron
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPNeuronHebbMembrane` (мембрана), `NPulseLTZoneCommon` (LT-зона), `NPulseHebbSynapse` (синапсы)
- **Внешние компоненты**: пресинаптические нейроны, источник мотивации

### Свойства

`NSPHebbNeuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `MembraneClassName = "NPNeuronHebbMembrane"` — мембрана с поддержкой синапсов Хебба
- `LTMembraneClassName = ""` — без LT-мембраны
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

`NSPHebbNeuron` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание Hebb-нейрона в коде C++

```cpp
// Создание мелкого нейрона с синапсами Хебба
auto neuron = storage->CreateComponent("NSPHebbNeuron");
neuron->SetName("SPHebbNeuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NPNeuronHebbMembrane, NPulseLTZoneCommon, NPulseHebbSynapse)
neuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Веса синапсов Хебба обновляются автоматически
}
```

#### Пример 2: Конфигурация XML

```xml
<SPHebbNeuron1 Class="NSPHebbNeuron">
    <Parameters>
        <!-- Параметры наследуются от NPulseNeuron -->
    </Parameters>
</SPHebbNeuron1>
```

### Использование в конфигурациях

`NSPHebbNeuron` используется в экспериментах с обучением Хебба:

- **Обучение Хебба**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где используется обучение Хебба)
- **Пластичность синапсов**: эксперименты с адаптацией синаптических весов

**Типичные значения параметров:**
- **MembraneClassName**: "NPNeuronHebbMembrane" (мембрана с поддержкой Хебба)
- **NumSomaMembraneParts**: 1 (одна часть сомы для мелких нейронов)
- **LTMembraneClassName**: "" (без LT-мембраны)

**Особенности:**
- Обучение Хебба: синапсы автоматически обновляют веса на основе корреляции пре- и постсинаптической активности
- Мотивационные сигналы: поддержка мотивационных входов для модуляции обучения
- Множественные выходы: синапсы Хебба имеют 6 выходов для различных компонентов

## Источники

См. [Literature-References.md](../Literature-References.md): **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NSPNeuron`](NSPNeuron.md) — базовый SP-нейрон
- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — синапс Хебба
- [`NPNeuronHebbMembrane`](NPNeuronHebbMembrane.md) — мембрана с поддержкой синапсов Хебба
- [`NLPHebbNeuron`](NLPHebbNeuron.md) — крупный нейрон с синапсами Хебба
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSPHebbNeuron` — configuration variant of small spiking neuron with Hebbian synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSPHebbNeuron", ...)`.  
**Instances**: `ClassName = "NSPHebbNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NSPHebbNeuron` is a configuration variant of the base class `NPulseNeuron` with membrane supporting Hebbian synapses. Created from `NPHebbNeuron` with settings:
- `LTMembraneClassName = ""` — without LT-membrane
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPNeuronHebbMembrane"` — membrane with Hebbian synapse support

Hebbian synapses (`NPulseHebbSynapse`) implement Hebbian learning mechanism with multiple outputs and motivational signals.

**Usage:** Modeling neurons with Hebbian learning, experiments with synaptic plasticity

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NSPHebbNeuron : configuration variant
    NSPHebbNeuron *-- NPNeuronHebbMembrane : PulseMembrane
    NPNeuronHebbMembrane *-- NPulseHebbSynapse : Synapses
    NSPHebbNeuron *-- NPulseLTZoneCommon : LTZone
    class NSPHebbNeuron {
        +MembraneClassName : string = "NPNeuronHebbMembrane"
        +LTMembraneClassName : string = ""
        +NumSomaMembraneParts : int = 1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSPHebbNeuron
    participant Membrane as NPNeuronHebbMembrane
    participant Synapse as NPulseHebbSynapse
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    Neuron->>Synapse: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Synapse: ACalculate()
        Synapse->>Synapse: Update Hebb weights
        Synapse-->>Membrane: Modified output
        Neuron->>Membrane: ACalculate()
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
    UpdateWeights --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
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
    CheckMore -->|No| CalcMembrane[Calculate membrane]
    CalcMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSPHebbNeuron["NSPHebbNeuron Configuration"]
        Membrane[NPNeuronHebbMembrane]
        LTZone[NPulseLTZoneCommon]
        Synapses[NPulseHebbSynapse]
    end
    
    subgraph External["External Components"]
        PreNeurons[Presynaptic Neurons]
        MotivationSource[Motivation Source]
    end
    
    BaseNeuron -->|configured as| NSPHebbNeuron
    NSPHebbNeuron -->|creates| Membrane
    NSPHebbNeuron -->|creates| LTZone
    NSPHebbNeuron -->|creates| Synapses
    PreNeurons -->|Input| Synapses
    MotivationSource -->|InputMotivation| Synapses
    LTZone -->|InputLTZoneFeedbackSignal| Synapses
    Synapses -->|Output| Membrane
    Membrane -->|potential| LTZone
```

### Properties

`NSPHebbNeuron` uses all properties of base class `NPulseNeuron` with parameters:
- `MembraneClassName = "NPNeuronHebbMembrane"` — membrane with Hebbian synapse support
- `LTMembraneClassName = ""` — without LT-membrane
- `NumSomaMembraneParts = 1` — one soma part

### Methods

`NSPHebbNeuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NSPHebbNeuron` is used in Hebbian learning experiments:

- **Hebbian learning**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where Hebbian learning is used)
- **Synaptic plasticity**: experiments with synaptic weight adaptation

**Typical parameter values:**
- **MembraneClassName**: "NPNeuronHebbMembrane" (membrane with Hebb support)
- **NumSomaMembraneParts**: 1 (one soma part for small neurons)

**Features:**
- Hebbian learning: synapses automatically update weights based on pre- and postsynaptic activity correlation
- Motivational signals: support for motivational inputs to modulate learning
- Multiple outputs: Hebbian synapses have 6 outputs for various components

### References

See [Literature-References.md](../Literature-References.md): **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NSPNeuron`](NSPNeuron.md) — base SP-neuron
- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — Hebbian synapse
- [`NPNeuronHebbMembrane`](NPNeuronHebbMembrane.md) — membrane with Hebbian synapse support
- [`NLPHebbNeuron`](NLPHebbNeuron.md) — large neuron with Hebbian synapses
- [Architecture.md](../Architecture.md) — library architecture
