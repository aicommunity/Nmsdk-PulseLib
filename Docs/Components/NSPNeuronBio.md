# NSPNeuronBio — мелкий импульсный нейрон с биологически правдоподобными параметрами

## RU

### Назначение

**Класс**: `NSPNeuronBio` — конфигурационный вариант мелкого импульсного нейрона с биологически правдоподобными параметрами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSPNeuronBio", ...)`.  
**Storage-инстансы**: `ClassName = "NSPNeuronBio"` в `Bin/Configs/*/Model_*.xml`.

`NSPNeuronBio` является конфигурационным вариантом базового класса `NPulseNeuron` с предустановленными биологически правдоподобными параметрами. Создается из `NPulseNeuron` с настройками:
- `LTMembraneClassName = ""` — без LT-мембраны
- `MembraneClassName = "NPMembraneBio"` — биоинспирированная мембрана
- `LTZoneClassName = "NPulseLTZoneThresholdBio"` — биоинспирированная LT-зона с порогом
- `ExcGeneratorClassName = "NPNeuronPosCGeneratorBio"` — биоинспирированный возбуждающий генератор
- `InhGeneratorClassName = "NPNeuronNegCGeneratorBio"` — биоинспирированный тормозной генератор

**Использование:** Моделирование биологически реалистичных мелких нейронов, эксперименты с биоинспирированными параметрами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NSPNeuronBio : configuration variant
    NSPNeuronBio *-- NPMembraneBio : PulseMembrane
    NSPNeuronBio *-- NPulseLTZoneThresholdBio : LTZone
    NSPNeuronBio *-- NPNeuronPosCGeneratorBio : ExcGenerator
    NSPNeuronBio *-- NPNeuronNegCGeneratorBio : InhGenerator
    class NPulseNeuron {
        +MembraneClassName : string
        +LTZoneClassName : string
        +ExcGeneratorClassName : string
        +InhGeneratorClassName : string
    }
    class NSPNeuronBio {
        +MembraneClassName : string = "NPMembraneBio"
        +LTZoneClassName : string = "NPulseLTZoneThresholdBio"
        +ExcGeneratorClassName : string = "NPNeuronPosCGeneratorBio"
        +InhGeneratorClassName : string = "NPNeuronNegCGeneratorBio"
        +LTMembraneClassName : string = ""
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NSPNeuronBio` — конфигурационный вариант с биологически правдоподобными параметрами

**Внутренняя структура:**
- **PulseMembrane** (`NPMembraneBio`) — биоинспирированная мембрана
- **LTZone** (`NPulseLTZoneThresholdBio`) — биоинспирированная LT-зона с порогом (Threshold = -0.055, ThresholdOff = -0.1)
- **ExcGenerator** (`NPNeuronPosCGeneratorBio`) — биоинспирированный возбуждающий генератор
- **InhGenerator** (`NPNeuronNegCGeneratorBio`) — биоинспирированный тормозной генератор

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NSPNeuronBio
    participant Membrane as NPMembraneBio
    participant LTZone as NPulseLTZoneThresholdBio
    participant ExcGen as NPNeuronPosCGeneratorBio
    participant InhGen as NPNeuronNegCGeneratorBio
    
    Storage->>Neuron: New() (из NPulseNeuron)
    Storage->>Neuron: SetMembraneClassName("NPMembraneBio")
    Storage->>Neuron: SetLTZoneClassName("NPulseLTZoneThresholdBio")
    Storage->>Neuron: SetExcGeneratorClassName("NPNeuronPosCGeneratorBio")
    Storage->>Neuron: SetInhGeneratorClassName("NPNeuronNegCGeneratorBio")
    Storage->>Neuron: Build()
    Neuron->>Neuron: NPulseNeuron::ABuild()
    Neuron->>Membrane: CreateComponent("PulseMembrane")
    Neuron->>LTZone: CreateComponent("LTZone")
    Neuron->>ExcGen: CreateComponent("ExcGenerator")
    Neuron->>InhGen: CreateComponent("InhGenerator")
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate()
        Membrane->>Membrane: Расчет биоинспирированной мембраны
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Проверка порога (Threshold = -0.055)
        LTZone-->>Neuron: Output (спайк)
        Neuron-->>Storage: Output
    end
```

**Жизненный цикл:**
1. **Создание**: `NSPNeuronBio` создается из `NPulseNeuron` с настройкой биоинспирированных компонентов
2. **Настройка**: Устанавливаются биоинспирированные мембрана, LT-зона и генераторы
3. **Сборка**: Автоматически создается структура нейрона с биоинспирированными компонентами
4. **Расчет**: На каждом шаге рассчитываются биоинспирированные компоненты

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Configuring: Настройка биоинспирированных компонентов
    Configuring --> Building: Build()
    Building --> CreatingMembrane: Создание NPMembraneBio
    CreatingMembrane --> CreatingLTZone: Создание NPulseLTZoneThresholdBio
    CreatingLTZone --> CreatingExcGen: Создание NPNeuronPosCGeneratorBio
    CreatingExcGen --> CreatingInhGen: Создание NPNeuronNegCGeneratorBio
    CreatingInhGen --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Расчет биоинспирированной мембраны
    MembraneCalc --> LTZoneCalc: Расчет биоинспирированной LT-зоны
    LTZoneCalc --> CheckThreshold: Проверка порога (-0.055)
    CheckThreshold -->|Порог достигнут| GenerateSpike: Генерация спайка
    CheckThreshold -->|Порог не достигнут| Ready: Шаг завершен
    GenerateSpike --> Ready
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Configuring** — настройка биоинспирированных компонентов
- **Building** — выполняется сборка
- **CreatingMembrane** — создание биоинспирированной мембраны
- **CreatingLTZone** — создание биоинспирированной LT-зоны
- **CreatingExcGen** — создание возбуждающего генератора
- **CreatingInhGen** — создание тормозного генератора
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **MembraneCalc** — расчет биоинспирированной мембраны
- **LTZoneCalc** — расчет биоинспирированной LT-зоны
- **CheckThreshold** — проверка порога (-0.055)
- **GenerateSpike** — генерация спайка
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CallBase[NPulseNeuronCommon::ACalculate]
    CallBase --> CalcMembrane[Расчет мембраны NPMembraneBio]
    CalcMembrane --> CalcExcChannel[Расчет возбуждающего канала NPExcChannelBio]
    CalcExcChannel --> CalcInhChannel[Расчет тормозного канала NPInhChannelBio]
    CalcInhChannel --> AggregateMembrane[Агрегация потенциалов мембраны]
    AggregateMembrane --> CalcLTZone[Расчет LT-зоны NPulseLTZoneThresholdBio]
    CalcLTZone --> CheckThreshold{Potential >= -0.055?}
    CheckThreshold -->|Да| GenerateSpike[Генерация спайка]
    CheckThreshold -->|Нет| CheckThresholdOff{Potential < -0.1?}
    CheckThresholdOff -->|Да| StopSpike[Остановка спайка]
    CheckThresholdOff -->|Нет| UpdateOutput[Обновление Output]
    GenerateSpike --> UpdateOutput
    StopSpike --> UpdateOutput
    UpdateOutput --> End([End])
```

**Алгоритм расчета:**
1. Вызов базового расчета (`NPulseNeuronCommon::ACalculate()`)
2. Расчет биоинспирированной мембраны: расчет возбуждающих и тормозных каналов
3. Агрегация потенциалов от всех каналов
4. Расчет биоинспирированной LT-зоны: проверка порогов (-0.055 для генерации, -0.1 для остановки)
5. Генерация спайка при достижении порога
6. Обновление выходного сигнала нейрона

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSPNeuronBio["NSPNeuronBio Configuration"]
        Membrane[NPMembraneBio]
        LTZone[NPulseLTZoneThresholdBio]
        ExcGen[NPNeuronPosCGeneratorBio]
        InhGen[NPNeuronNegCGeneratorBio]
    end
    
    subgraph External["Внешние компоненты"]
        Synapses[Синапсы]
        PreNeurons[Пресинаптические нейроны]
    end
    
    BaseNeuron -->|конфигурируется как| NSPNeuronBio
    NSPNeuronBio -->|создает| Membrane
    NSPNeuronBio -->|создает| LTZone
    NSPNeuronBio -->|создает| ExcGen
    NSPNeuronBio -->|создает| InhGen
    Membrane -->|содержит| Synapses
    PreNeurons -->|Input| Synapses
    Synapses -->|ток| Membrane
    Membrane -->|потенциал| LTZone
    LTZone -->|Output| NSPNeuronBio
```

**Зависимости:**
- **Базовый класс**: `NPulseNeuron` (конфигурационный вариант)
- **Внутренние компоненты**: `NPMembraneBio` (мембрана), `NPulseLTZoneThresholdBio` (LT-зона), `NPNeuronPosCGeneratorBio` (возбуждающий генератор), `NPNeuronNegCGeneratorBio` (тормозной генератор)
- **Внешние компоненты**: синапсы, пресинаптические нейроны

### Свойства

`NSPNeuronBio` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `MembraneClassName = "NPMembraneBio"`
- `LTZoneClassName = "NPulseLTZoneThresholdBio"`
- `ExcGeneratorClassName = "NPNeuronPosCGeneratorBio"`
- `InhGeneratorClassName = "NPNeuronNegCGeneratorBio"`
- `LTMembraneClassName = ""`

### Методы

`NSPNeuronBio` использует все методы базового класса `NPulseNeuron`.

### Примеры использования

#### Пример 1: Создание биоинспирированного SP-нейрона в коде C++

```cpp
// Создание биоинспирированного SP-нейрона
auto neuron = storage->CreateComponent<NPulseNeuron>();
neuron->SetName("SPNeuronBio");

// Инициализация
neuron->Default();

// Настройка параметров
neuron->LTMembraneClassName = "";
neuron->MembraneClassName = "NPMembraneBio";
neuron->LTZoneClassName = "NPulseLTZoneThresholdBio";
neuron->ExcGeneratorClassName = "NPNeuronPosCGeneratorBio";
neuron->InhGeneratorClassName = "NPNeuronNegCGeneratorBio";

// Сборка
neuron->Build();
```

### Использование в конфигурациях

`NSPNeuronBio` используется в экспериментах с биологически реалистичными нейронами:

- **Биоинспирированные модели**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (где требуется биологическая реалистичность)

**Типичные значения параметров:**
- **MembraneClassName**: "NPMembraneBio" (биоинспирированная мембрана)
- **LTZoneClassName**: "NPulseLTZoneThresholdBio" (LT-зона с порогом -0.055)
- **ExcGeneratorClassName**: "NPNeuronPosCGeneratorBio" (возбуждающий генератор)
- **InhGeneratorClassName**: "NPNeuronNegCGeneratorBio" (тормозной генератор)
- **LTMembraneClassName**: "" (без LT-мембраны)

## Источники

См. [Literature-References.md](../Literature-References.md): **26**, **29**, **30**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NSPNeuron`](NSPNeuron.md) — базовый SP-нейрон
- [`NSPNeuronBio2`](NSPNeuronBio2.md) — SP-нейрон с биологически правдоподобными параметрами (версия 2)
- [`NPMembraneBio`](NPMembraneBio.md) — биоинспирированная мембрана
- [`NPulseLTZoneThresholdBio`](NPulseLTZoneThresholdBio.md) — биоинспирированная LT-зона с порогом
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSPNeuronBio` — configuration variant of small spiking neuron with biologically plausible parameters.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSPNeuronBio", ...)`.  
**Instances**: `ClassName = "NSPNeuronBio"` in `Bin/Configs/*/Model_*.xml`.

`NSPNeuronBio` is a configuration variant of the base class `NPulseNeuron` with preset biologically plausible parameters. Created from `NPulseNeuron` with settings:
- `LTMembraneClassName = ""` — without LT-membrane
- `MembraneClassName = "NPMembraneBio"` — bio-inspired membrane
- `LTZoneClassName = "NPulseLTZoneThresholdBio"` — bio-inspired LT-zone with threshold
- `ExcGeneratorClassName = "NPNeuronPosCGeneratorBio"` — bio-inspired excitatory generator
- `InhGeneratorClassName = "NPNeuronNegCGeneratorBio"` — bio-inspired inhibitory generator

**Usage:** Modeling biologically realistic small neurons, experiments with bio-inspired parameters

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NSPNeuronBio : configuration variant
    class NSPNeuronBio {
        +MembraneClassName : string = "NPMembraneBio"
        +LTZoneClassName : string = "NPulseLTZoneThresholdBio"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSPNeuronBio
    participant Membrane as NPMembraneBio
    participant LTZone as NPulseLTZoneThresholdBio
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
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
    Building --> CreatingMembrane: Create bio membrane
    CreatingMembrane --> CreatingLTZone: Create bio LT-zone
    CreatingLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> CheckThreshold: Check threshold
    CheckThreshold -->|Reached| GenerateSpike: Generate spike
    CheckThreshold -->|Not reached| Ready: Step completed
    GenerateSpike --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcMembrane[Calculate bio membrane]
    CalcMembrane --> CalcLTZone[Calculate bio LT-zone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateSpike[Generate spike]
    CheckThreshold -->|No| UpdateOutput[Update Output]
    GenerateSpike --> UpdateOutput
    UpdateOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NSPNeuronBio["NSPNeuronBio Configuration"]
        Membrane[NPMembraneBio]
        LTZone[NPulseLTZoneThresholdBio<br/>Threshold = -0.055]
        ExcGen[NPNeuronPosCGeneratorBio]
        InhGen[NPNeuronNegCGeneratorBio]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses]
        PreNeurons[Presynaptic neurons]
    end
    
    BaseNeuron -->|configured as| NSPNeuronBio
    NSPNeuronBio -->|creates| Membrane
    NSPNeuronBio -->|creates| LTZone
    NSPNeuronBio -->|creates| ExcGen
    NSPNeuronBio -->|creates| InhGen
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|Output| NSPNeuronBio
```

### Properties

`NSPNeuronBio` uses all properties of base class `NPulseNeuron` with parameters:
- `MembraneClassName = "NPMembraneBio"` — bio-inspired membrane
- `LTZoneClassName = "NPulseLTZoneThresholdBio"` — bio-inspired LT-zone with threshold
- `ExcGeneratorClassName = "NPNeuronPosCGeneratorBio"` — bio-inspired excitatory generator
- `InhGeneratorClassName = "NPNeuronNegCGeneratorBio"` — bio-inspired inhibitory generator
- `LTMembraneClassName = ""` — without LT-membrane

### Methods

`NSPNeuronBio` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NSPNeuronBio` is used in experiments with biologically realistic neurons:

- **Bio-inspired models**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where biological realism is required)

**Typical parameter values:**
- **MembraneClassName**: "NPMembraneBio" (bio-inspired membrane)
- **LTZoneClassName**: "NPulseLTZoneThresholdBio" (LT-zone with threshold -0.055)
- **ExcGeneratorClassName**: "NPNeuronPosCGeneratorBio" (excitatory generator)
- **InhGeneratorClassName**: "NPNeuronNegCGeneratorBio" (inhibitory generator)
- **LTMembraneClassName**: "" (without LT-membrane)

**Features:**
- Biologically plausible: all components use bio-inspired parameters
- Realistic thresholds: LT-zone uses biological threshold values (-0.055 for spike, -0.1 for stop)
- Complete bio setup: includes both excitatory and inhibitory generators

### References

See [Literature-References.md](../Literature-References.md): **26**, **29**, **30**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NSPNeuron`](NSPNeuron.md) — base SP-neuron
- [`NSPNeuronBio2`](NSPNeuronBio2.md) — SP-neuron with bio-inspired parameters (version 2)
- [`NPMembraneBio`](NPMembraneBio.md) — bio-inspired membrane
- [`NPulseLTZoneThresholdBio`](NPulseLTZoneThresholdBio.md) — bio-inspired LT-zone with threshold
- [Architecture.md](../Architecture.md) — library architecture
