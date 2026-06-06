# NLPLifeNeuron — крупный живой импульсный нейрон

## RU

### Назначение

**Класс**: `NLPLifeNeuron` — конфигурационный вариант крупного живого импульсного нейрона с поддержкой жизнеобеспечения.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NLPLifeNeuron", ...)`.
**Storage-инстансы**: `ClassName = "NLPLifeNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NLPLifeNeuron` является конфигурационным вариантом базового класса `NPulseLifeNeuron` с предустановленными параметрами для крупных живых нейронов. Создается из `NPulseLifeNeuron` с настройками:
- `NumSomaMembraneParts = 3` — три части сомы
- `MembraneClassName = "NPMembrane"` — стандартная мембрана
- `LTMembraneClassName = ""` — без LT-мембраны

Живые нейроны (`NPulseLifeNeuron`) включают модель жизнеобеспечения (`NNeuronLife`), которая отслеживает энергетическое состояние нейрона, износ и другие параметры жизнедеятельности.

**Использование:** Моделирование крупных нейронов с жизнеобеспечением, эксперименты с энергетическими моделями

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|-- NPulseLifeNeuron
    NPulseLifeNeuron <|.. NLPLifeNeuron : configuration variant
    NLPLifeNeuron *-- NPMembrane : PulseMembrane
    NLPLifeNeuron *-- NNeuronLife : NeuronLife
    NLPLifeNeuron *-- NPulseLTZoneCommon : LTZone
    class NPulseLifeNeuron {
        +SummaryPosGd : double
        +SummaryPosGs : double
        +SummaryPosG : double
        +SummaryNegGd : double
        +SummaryNegGs : double
        +SummaryNegG : double
        +OutputSummaryPosGd : MDMatrix~double~
        +GetNeuronLife() NNeuronLife*
    }
    class NLPLifeNeuron {
        +NumSomaMembraneParts : int = 3
        +MembraneClassName : string = "NPMembrane"
        +LTMembraneClassName : string = ""
    }
    class NNeuronLife {
        +Energy : double
        +Threshold : double
        +CriticalEnergy : double
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NPulseLifeNeuron` — живой импульсный нейрон с поддержкой жизнеобеспечения
- `NLPLifeNeuron` — конфигурационный вариант для крупных живых нейронов

**Внутренняя структура:**
- **PulseMembrane** (`NPMembrane`) — стандартная мембрана (3 части сомы)
- **NeuronLife** (`NNeuronLife`) — модель жизнеобеспечения
- **LTZone** (`NPulseLTZoneCommon`) — стандартная LT-зона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NLPLifeNeuron
    participant Membrane as NPMembrane
    participant LTZone as NPulseLTZoneCommon
    participant NeuronLife as NNeuronLife

    Storage->>Neuron: New() (из NPulseLifeNeuron)
    Storage->>Neuron: SetNumSomaMembraneParts(3)
    Storage->>Neuron: SetMembraneClassName("NPMembrane")
    Storage->>Neuron: SetLTMembraneClassName("")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseLifeNeuron::ABuild()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    Neuron->>Membrane: CreateComponent("PulseMembrane", "NPMembrane")
    Note over Membrane: Создание 3 частей сомы
    Neuron->>LTZone: CreateComponent("LTZone")
    Neuron->>NeuronLife: AddMissingComponent("NeuronLife", "NNeuronLife")
    Neuron->>Neuron: CreateLink(LTZone->Output, NeuronLife->Input1)
    Neuron-->>Storage: Ready = true

    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: NPulseLifeNeuron::ACalculate()
        Neuron->>Membrane: ACalculate() (3 части сомы)
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Проверка порога
        LTZone->>NeuronLife: Input1 = потенциал
        NeuronLife->>NeuronLife: ACalculate()
        NeuronLife->>NeuronLife: Вычисление метрик жизнеобеспечения
        NeuronLife-->>Neuron: Output1..Output8 (метрики)
        Neuron->>Neuron: Обновление Summary весов
        LTZone-->>Neuron: Output (спайк)
        Neuron-->>Storage: Output (спайк + метрики)
    end
```

**Жизненный цикл:**
1. **Создание**: `NLPLifeNeuron` создается из `NPulseLifeNeuron` с настройкой параметров
2. **Настройка**: Устанавливается три части сомы, стандартная мембрана, отсутствие LT-мембраны
3. **Сборка**: Автоматически создается структура нейрона с мембраной (3 части), LT-зоной и моделью жизнеобеспечения
4. **Расчет**: На каждом шаге рассчитываются мембрана (3 части), LT-зона и модель жизнеобеспечения

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка LP параметров
    Configuring --> SetLPParams: SetNumSomaMembraneParts(3)<br/>SetMembraneClassName("NPMembrane")
    SetLPParams --> Building: Build()
    Building --> BuildBase: NPulseNeuron::ABuild()
    BuildBase --> CreateMembrane: Создание NPMembrane (3 части)
    CreateMembrane --> CreateLTZone: Создание NPulseLTZoneCommon
    CreateLTZone --> CreateLife: Создание NNeuronLife
    CreateLife --> LinkLife: CreateLink(LTZone->Output, NeuronLife->Input1)
    LinkLife --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Расчет мембраны (3 части)
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> LifeCalc: Расчет жизнеобеспечения
    LifeCalc --> UpdateSummary: Обновление Summary весов
    UpdateSummary --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка параметров для LP-нейрона
- **SetLPParams** — установка параметров (три части сомы, стандартная мембрана)
- **Building** — выполняется сборка структуры
- **BuildBase** — сборка базовой структуры нейрона
- **CreateMembrane** — создание мембраны с тремя частями сомы
- **CreateLTZone** — создание LT-зоны
- **CreateLife** — создание модели жизнеобеспечения
- **LinkLife** — связывание LT-зоны с моделью жизнеобеспечения
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **MembraneCalc** — расчет мембраны (три части сомы)
- **LTZoneCalc** — расчет LT-зоны
- **LifeCalc** — расчет жизнеобеспечения
- **UpdateSummary** — обновление суммарных весов
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> CalcMembrane[Расчет мембраны (3 части сомы)]
    CalcMembrane --> CalcChannels[Расчет каналов]
    CalcChannels --> CalcSynapses[Расчет синапсов]
    CalcSynapses --> CalcLTZone[Расчет LT-зоны]
    CalcLTZone --> CheckThreshold{Порог достигнут?}
    CheckThreshold -->|Да| GenerateSpike[Генерация спайка]
    CheckThreshold -->|Нет| UpdateLifeInput
    GenerateSpike --> UpdateLifeInput[Обновление NeuronLife->Input1]
    UpdateLifeInput --> CalcLife[Расчет жизнеобеспечения]
    CalcLife --> CalcWearOut[ACalcWearOut]
    CalcWearOut --> CalcEnergy[ACalcEnergy]
    CalcEnergy --> CalcFeel[ACalcFeel]
    CalcFeel --> CalcThresholds[ACalcThresholdLife]
    CalcThresholds --> UpdateSummary[Обновление Summary весов]
    UpdateSummary --> UpdateOutputs[Обновление OutputSummary]
    UpdateOutputs --> End([End])
```

**Алгоритм расчета:**
1. Вызов базового расчета нейрона (`NPulseNeuronCommon::ACalculate`)
2. Расчет мембраны с тремя частями сомы, каналов, синапсов
3. Расчет LT-зоны и проверка порога
4. Обновление входов модели жизнеобеспечения
5. Расчет метрик жизнеобеспечения (износ, энергия, чувство, пороги)
6. Обновление суммарных весов и выходов

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end

    subgraph NPulseLifeNeuron["NPulseLifeNeuron"]
        LifeNeuronCore[Ядро живого нейрона]
    end

    subgraph NLPLifeNeuron["NLPLifeNeuron Configuration"]
        Membrane["NPMembrane<br/>PulseMembrane<br/>3 части сомы"]
        LTZone["NPulseLTZoneCommon<br/>LTZone"]
        NeuronLife["NNeuronLife<br/>NeuronLife"]
    end

    subgraph External["Внешние компоненты"]
        Synapses[Синапсы]
        PreNeurons[Пресинаптические нейроны]
        EnergySource[Источник энергии]
    end

    BaseNeuron -->|наследуется| NPulseLifeNeuron
    NPulseLifeNeuron -->|конфигурируется как| NLPLifeNeuron
    NLPLifeNeuron -->|создает| Membrane
    NLPLifeNeuron -->|создает| LTZone
    NLPLifeNeuron -->|создает| NeuronLife
    Membrane -->|содержит| Synapses
    PreNeurons -->|Input| Synapses
    Synapses -->|ток| Membrane
    Membrane -->|потенциал| LTZone
    LTZone -->|Output| NeuronLife
    LTZone -->|Output| NLPLifeNeuron
    EnergySource -->|энергия| NeuronLife
    NeuronLife -->|метрики| NLPLifeNeuron
```

**Зависимости:**
- **Базовый класс**: `NPulseLifeNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPMembrane` (мембрана с 3 частями сомы), `NPulseLTZoneCommon` (LT-зона), `NNeuronLife` (модель жизнеобеспечения)
- **Внешние компоненты**: синапсы, пресинаптические нейроны, источник энергии

### Свойства

`NLPLifeNeuron` использует все свойства базового класса `NPulseLifeNeuron` с параметрами:
- `NumSomaMembraneParts = 3` — три части сомы
- `MembraneClassName = "NPMembrane"` — стандартная мембрана
- `LTMembraneClassName = ""` — без LT-мембраны

**Наследуемые свойства от NPulseLifeNeuron:**
- `SummaryPosGd`, `SummaryPosGs`, `SummaryPosG` (double) — суммарные положительные веса
- `SummaryNegGd`, `SummaryNegGs`, `SummaryNegG` (double) — суммарные отрицательные веса
- `OutputSummaryPosGd`, `OutputSummaryPosGs`, `OutputSummaryPosG` (MDMatrix<double>) — выходные сигналы положительных весов
- `OutputSummaryNegGd`, `OutputSummaryNegGs`, `OutputSummaryNegG` (MDMatrix<double>) — выходные сигналы отрицательных весов
- Все нормализованные выходы (`*Norm`)

**Параметры жизнеобеспечения (в NeuronLife):**
- `Energy` (double) — текущая энергия нейрона
- `Threshold` (double) — порог жизнедеятельности
- `CriticalEnergy` (double) — критический уровень энергии
- `WearOut` (double) — износ нейрона
- `Feel` (double) — чувство нейрона

### Методы

`NLPLifeNeuron` использует все методы базового класса `NPulseLifeNeuron`:
- `GetNeuronLife()` → `NNeuronLife*` — получение модели жизнеобеспечения

### Примеры использования

#### Пример 1: Создание живого LP-нейрона в коде C++

```cpp
// Создание крупного живого нейрона
auto neuron = storage->CreateComponent("NLPLifeNeuron");
neuron->SetName("LPLifeNeuron");

// Инициализация (использует параметры по умолчанию)
neuron->Default();

// Сборка (автоматически создает NPMembrane с 3 частями сомы, NPulseLTZoneCommon, NNeuronLife)
neuron->Build();

// Получение модели жизнеобеспечения
auto neuronLife = neuron->GetNeuronLife();
if (neuronLife) {
    // Настройка параметров жизнеобеспечения
    neuronLife->Threshold = 0.001;
    neuronLife->CriticalEnergy = 0.5;
}

// Использование
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
    // Получение метрик жизнеобеспечения
    auto neuronLife = neuron->GetNeuronLife();
    if (neuronLife) {
        double energy = neuronLife->Output5(0, 0);
        double wearOut = neuronLife->Output3(0, 0);
        // Использование метрик для адаптации
    }
}
```

#### Пример 2: Конфигурация XML

```xml
<LPLifeNeuron1 Class="NLPLifeNeuron">
    <Parameters>
        <!-- Параметры наследуются от NPulseLifeNeuron -->
    </Parameters>
</LPLifeNeuron1>
```

### Использование в конфигурациях

`NLPLifeNeuron` используется в экспериментах с крупными живыми нейронами:

- **Моделирование жизнеобеспечения**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где используются крупные живые нейроны)
- **Энергетические модели**: эксперименты с метриками энергии и износа для крупных нейронов

**Типичные значения параметров:**
- **NumSomaMembraneParts**: 3 (три части сомы для крупных нейронов)
- **MembraneClassName**: "NPMembrane" (стандартная мембрана)
- **LTMembraneClassName**: "" (без LT-мембраны)

**Особенности:**
- Три части сомы: крупные нейроны имеют более сложную структуру с тремя частями сомы
- Модель жизнеобеспечения: автоматически создается и связывается с LT-зоной
- Метрики жизнедеятельности: отслеживание энергии, износа, чувства для крупных нейронов

## Источники

См. [Literature-References.md](../Literature-References.md): **neuromodeler.ru** (жизнеобеспечение), **15**.

### См. также

- [`NPulseLifeNeuron`](NPLifeNeuron.md) — живой импульсный нейрон (базовый класс)
- [`NLPNeuron`](NLPNeuron.md) — базовый LP-нейрон
- [`NSPLifeNeuron`](NSPLifeNeuron.md) — мелкий живой нейрон
- [`NLPLifeHebbNeuron`](NLPLifeHebbNeuron.md) — крупный живой нейрон с синапсами Хебба
- [`NNeuronLife`](NNeuronLife.md) — модель жизнеобеспечения
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NLPLifeNeuron` — configuration variant of large living spiking neuron with life support.
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NLPLifeNeuron", ...)`.
**Instances**: `ClassName = "NLPLifeNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NLPLifeNeuron` is a configuration variant of the base class `NPulseLifeNeuron` with preset parameters for large living neurons. Created from `NPulseLifeNeuron` with settings:
- `NumSomaMembraneParts = 3` — three soma parts
- `MembraneClassName = "NPMembrane"` — standard membrane
- `LTMembraneClassName = ""` — without LT-membrane

Living neurons (`NPulseLifeNeuron`) include a life support model (`NNeuronLife`) that tracks the neuron's energy state, wear, and other life parameters.

**Usage:** Modeling large neurons with life support, experiments with energy models

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|-- NPulseLifeNeuron
    NPulseLifeNeuron <|.. NLPLifeNeuron : configuration variant
    NLPLifeNeuron *-- NPMembrane : PulseMembrane
    NLPLifeNeuron *-- NNeuronLife : NeuronLife
    NLPLifeNeuron *-- NPulseLTZoneCommon : LTZone
    class NLPLifeNeuron {
        +NumSomaMembraneParts : int = 3
        +MembraneClassName : string = "NPMembrane"
        +LTMembraneClassName : string = ""
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NLPLifeNeuron
    participant Membrane as NPMembrane
    participant LTZone as NPulseLTZoneCommon
    participant NeuronLife as NNeuronLife

    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent() (3 soma parts)
    Neuron->>LTZone: CreateComponent()
    Neuron->>NeuronLife: AddMissingComponent()
    Neuron->>Neuron: CreateLink(LTZone->Output, NeuronLife->Input1)
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate() (3 parts)
        Neuron->>LTZone: ACalculate()
        LTZone->>NeuronLife: Input1 = potential
        NeuronLife->>NeuronLife: Calculate life metrics
        NeuronLife-->>Neuron: Output metrics
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateComponents: Create components (3 soma parts)
    CreateComponents --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Calculate membrane (3 parts)
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> LifeCalc: Calculate life support
    LifeCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcMembrane[Calculate membrane (3 soma parts)]
    CalcMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> UpdateLifeInput[Update NeuronLife inputs]
    UpdateLifeInput --> CalcLife[Calculate life support]
    CalcLife --> UpdateSummary[Update summary weights]
    UpdateSummary --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseLifeNeuron["NPulseLifeNeuron Base"]
        BaseNeuron[NPulseLifeNeuron]
    end

    subgraph NLPLifeNeuron["NLPLifeNeuron Configuration"]
        Membrane["NPMembrane<br/>3 soma parts"]
        LTZone[NPulseLTZoneCommon]
        NeuronLife[NNeuronLife]
    end

    subgraph External["External Components"]
        Synapses[Synapses]
        EnergySource[Energy Source]
    end

    BaseNeuron -->|configured as| NLPLifeNeuron
    NLPLifeNeuron -->|creates| Membrane
    NLPLifeNeuron -->|creates| LTZone
    NLPLifeNeuron -->|creates| NeuronLife
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|Output| NeuronLife
    EnergySource -->|energy| NeuronLife
```

### Properties

`NLPLifeNeuron` uses all properties of base class `NPulseLifeNeuron` with parameters:
- `NumSomaMembraneParts = 3` — three soma parts
- `MembraneClassName = "NPMembrane"` — standard membrane
- `LTMembraneClassName = ""` — without LT-membrane

**Life support parameters (in NeuronLife):**
- `Energy` (double) — current neuron energy
- `Threshold` (double) — life threshold
- `CriticalEnergy` (double) — critical energy level
- `WearOut` (double) — neuron wear out

### Methods

`NLPLifeNeuron` uses all methods of base class `NPulseLifeNeuron`:
- `GetNeuronLife()` → `NNeuronLife*` — get life support model

### Usage in configurations

`NLPLifeNeuron` is used in large living neuron experiments:

- **Life support modeling**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where large living neurons are used)
- **Energy models**: experiments with energy and wear out metrics for large neurons

**Typical parameter values:**
- **NumSomaMembraneParts**: 3 (three soma parts for large neurons)
- **MembraneClassName**: "NPMembrane" (standard membrane)
- **LTMembraneClassName**: "" (without LT-membrane)

**Features:**
- Three soma parts: large neurons have more complex structure with three soma parts
- Life support model: automatically created and linked with LT-zone
- Life metrics: tracking energy, wear out, feel for large neurons

### References

See [Literature-References.md](../Literature-References.md): **neuromodeler.ru** (life support), **15**.

### See Also

- [`NPulseLifeNeuron`](NPLifeNeuron.md) — living spiking neuron (base class)
- [`NLPNeuron`](NLPNeuron.md) — base LP-neuron
- [`NSPLifeNeuron`](NSPLifeNeuron.md) — small living neuron
- [`NLPLifeHebbNeuron`](NLPLifeHebbNeuron.md) — large living neuron with Hebbian synapses
- [`NNeuronLife`](NNeuronLife.md) — life support model
- [Architecture.md](../Architecture.md) — library architecture
