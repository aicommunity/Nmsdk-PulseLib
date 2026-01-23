# NNewMotoneuron — новая версия мотонейрона

## RU

### Назначение

**Класс**: `NNewMotoneuron` — обновлённая модель мотонейрона с улучшенной архитектурой.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewMotoneuron", ...)`.  
**Storage-инстансы**: `ClassName = "NNewMotoneuron"` в `Bin/Configs/*/Model_*.xml`.

`NNewMotoneuron` является обновлённой версией базового класса `NMotoneuron` для моделирования мотонейронов (двигательных нейронов) с улучшенной логикой и архитектурой. Наследуется от `NMotoneuron` и предоставляет обновлённую реализацию мотонейрона с оптимизированными параметрами.

Мотонейроны преобразуют входные сигналы в моторную активность для управления мышцами и движениями.

**Использование:** Моделирование двигательных систем, управление мышцами с улучшенной архитектурой

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuronCommon <|-- NPulseNeuron
    NPulseNeuron <|.. NMotoneuron
    NMotoneuron <|-- NNewMotoneuron : updated version
    NNewMotoneuron *-- NPulseMembrane : PulseMembrane
    NNewMotoneuron *-- NPulseLTZone : LTZone
    class NMotoneuron {
        +MembraneClassName : string = "NPNeuronMembrane"
        +LTZoneClassName : string = "NPLTZone"
    }
    class NNewMotoneuron {
        +New() NNewMotoneuron*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
```

**Иерархия наследования:**
- `NPulseNeuronCommon` — общий импульсный нейрон
- `NPulseNeuron` — импульсный нейрон с параметрами структурирования
- `NMotoneuron` — базовый мотонейрон
- `NNewMotoneuron` — обновлённая версия мотонейрона

**Внутренняя структура:**
- **PulseMembrane** (`NPulseMembrane`) — мембрана нейрона
- **LTZone** (`NPulseLTZone`) — LT-зона для генерации спайков

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NNewMotoneuron
    participant Membrane as NPulseMembrane
    participant LTZone as NPulseLTZone
    participant Muscle as NMuscle
    
    Storage->>Neuron: New()
    Storage->>Neuron: Default()
    Note over Neuron: Новая логика параметров
    Storage->>Neuron: Build()
    Neuron->>Neuron: ABuild()
    Neuron->>Membrane: AddMissingComponent("PulseMembrane")
    Neuron->>LTZone: AddMissingComponent("LTZone")
    Neuron->>Neuron: CreateLinks()
    Neuron-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: ACalculate()
        Neuron->>Membrane: ACalculate()
        Membrane->>Membrane: Интеграция токов
        Membrane-->>LTZone: Выходной сигнал мембраны
        LTZone->>LTZone: Проверка порога
        LTZone-->>Neuron: Output (моторная активность)
        Neuron-->>Muscle: Output (управление мышцей)
    end
```

**Жизненный цикл:**
1. **Инициализация**: `ADefault()` — установка параметров мотонейрона с новой логикой
2. **Сборка**: `ABuild()` — подключение входов/синапсов
3. **Расчет**: `ACalculate()` — расчёт моторной активности
4. **Сброс**: `AReset()` — сброс состояния

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: ADefault()
    Note over Defaulted: Новая логика параметров
    Defaulted --> Building: ABuild()
    Building --> CreatingMembrane: Создание мембраны
    CreatingMembrane --> CreatingLTZone: Создание LT-зоны
    CreatingLTZone --> Linking: Подключение входов/синапсов
    Linking --> Built: Структура построена
    Built --> Ready: Ready = true
    Ready --> Calculating: ACalculate()
    Calculating --> MembraneCalc: Расчет мембраны
    MembraneCalc --> LTZoneCalc: Расчет LT-зоны
    LTZoneCalc --> CheckThreshold: Проверка порога
    CheckThreshold -->|Порог достигнут| MotorSpike: Генерация моторного спайка
    CheckThreshold -->|Порог не достигнут| Ready: Шаг завершен
    MotorSpike --> Ready
    Ready --> Resetting: AReset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены с новой логикой
- **Building** — выполняется сборка структуры
- **CreatingMembrane** — создание мембраны
- **CreatingLTZone** — создание LT-зоны
- **Linking** — подключение входов/синапсов
- **Built** — структура мотонейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет мотонейрона
- **MembraneCalc** — расчет мембраны (интеграция токов)
- **LTZoneCalc** — расчет LT-зоны
- **CheckThreshold** — проверка достижения порога
- **MotorSpike** — генерация моторного спайка
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start ACalculate]) --> CallBase[Вызов базового расчета]
    CallBase --> CalcMembrane[Расчет мембраны]
    CalcMembrane --> AggregateChannels[Агрегация токов от каналов]
    AggregateChannels --> IntegratePotential[Интеграция мембранного потенциала]
    IntegratePotential --> CalcLTZone[Расчет LT-зоны]
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
1. Вызов базового расчета
2. Расчет мембраны: агрегация токов от каналов, интеграция потенциала
3. Расчет LT-зоны: проверка порога генерации спайка
4. Генерация моторного спайка при достижении порога
5. Передача моторной активности мышцам для управления движением

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NMotoneuron["NMotoneuron Base"]
        BaseNeuron[NMotoneuron]
    end
    
    subgraph NNewMotoneuron["NNewMotoneuron"]
        Membrane[NPulseMembrane]
        LTZone[NPulseLTZone]
    end
    
    subgraph External["Внешние компоненты"]
        Channels[Каналы]
        Synapses[Синапсы]
        Muscle[NMuscle]
    end
    
    BaseNeuron -->|наследуется| NNewMotoneuron
    NNewMotoneuron -->|создает| Membrane
    NNewMotoneuron -->|создает| LTZone
    Channels -->|токи| Membrane
    Synapses -->|сигналы| Channels
    Membrane -->|потенциал| LTZone
    LTZone -->|моторная активность| Muscle
```

**Зависимости:**
- **Базовый класс**: `NMotoneuron` (обновлённая версия)
- **Внутренние компоненты**: `NPulseMembrane` (мембрана), `NPulseLTZone` (LT-зона)
- **Внешние компоненты**: каналы (`NPulseChannel*`), синапсы (`NPulseSynapse*`), мышцы (`NMuscle`)

### Свойства

`NNewMotoneuron` наследует все свойства базового класса `NMotoneuron` с обновлённой логикой инициализации.

**Публичные свойства:**
- `ActivePosInputs` (MDMatrix<double>) — матрица активных положительных входов
- `ActiveNegInputs` (MDMatrix<double>) — матрица активных отрицательных входов
- `ActiveOutputs` (MDMatrix<double>) — матрица активных выходов
- `Activity` (double) — активность нейрона
- `Output` (MDMatrix<double>) — выходной сигнал (моторная активность)

### Методы

#### Публичные методы

- **`New()`** → `NNewMotoneuron*` — создает новый экземпляр класса

#### Защищенные методы

- **`ADefault()`** → `bool` — инициализирует параметры мотонейрона с новой логикой
- **`ABuild()`** → `bool` — строит структуру нейрона, подключает входы/синапсы
- **`AReset()`** → `bool` — сбрасывает состояния нейрона
- **`ACalculate()`** → `bool` — выполняет расчёт моторной активности на одном шаге

### Примеры использования

#### Пример 1: Создание нового мотонейрона в коде C++

```cpp
// Создание нового мотонейрона
auto motoneuron = storage->CreateComponent("NNewMotoneuron");
motoneuron->SetName("NewMotoneuron1");

// Инициализация (использует новую логику параметров)
motoneuron->Default();

// Сборка
motoneuron->Build();

// Использование
for (int step = 0; step < 1000; step++) {
    motoneuron->Calculate();
    double motorOutput = motoneuron->Output(0, 0);
    // Передача моторной активности мышце
}
```

#### Пример 2: Конфигурация XML

```xml
<NewMotoneuron1 Class="NNewMotoneuron">
    <Parameters>
        <!-- Параметры наследуются от NMotoneuron -->
    </Parameters>
</NewMotoneuron1>
```

### Использование в конфигурациях

`NNewMotoneuron` используется в экспериментах с двигательными системами:

- **Мотонейроны**: `Bin/Configs/!OldConfigs/OldExperiments/Motoneuron/`
- **Управление движением**: `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`

**Особенности:**
- Обновлённая логика: улучшенная инициализация параметров
- Оптимизированная архитектура: более эффективная обработка сигналов
- Совместимость: наследует функциональность базового `NMotoneuron`

### См. также

- [`NMotoneuron`](NMotoneuron.md) — базовый мотонейрон
- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон с параметрами структурирования
- [`NMuscle`](NMuscle.md) — мышца (получатель моторной активности)
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewMotoneuron` — updated motor neuron model with improved architecture.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewMotoneuron", ...)`.  
**Instances**: `ClassName = "NNewMotoneuron"` in `Bin/Configs/*/Model_*.xml`.

`NNewMotoneuron` is an updated version of the base class `NMotoneuron` for modeling motor neurons with improved logic and architecture. Inherits from `NMotoneuron` and provides an updated implementation of motor neuron with optimized parameters.

Motor neurons convert input signals into motor activity for controlling muscles and movements.

**Usage:** Modeling motor systems, muscle control with improved architecture

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NMotoneuron
    NMotoneuron <|-- NNewMotoneuron : updated version
    NNewMotoneuron *-- NPulseMembrane : PulseMembrane
    NNewMotoneuron *-- NPulseLTZone : LTZone
    class NNewMotoneuron {
        +New() NNewMotoneuron*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NNewMotoneuron
    participant Membrane as NPulseMembrane
    participant LTZone as NPulseLTZone
    participant Muscle as NMuscle
    
    Storage->>Neuron: New() + Default()
    Note over Neuron: New parameter logic
    Storage->>Neuron: Build()
    Neuron->>Membrane: AddMissingComponent()
    Neuron->>LTZone: AddMissingComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate()
        Neuron->>LTZone: ACalculate()
        Neuron-->>Muscle: Motor output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: ADefault()
    Defaulted --> Building: ABuild()
    Building --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: ACalculate()
    Calculating --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> CheckThreshold: Check threshold
    CheckThreshold -->|Reached| MotorSpike: Generate motor spike
    CheckThreshold -->|Not reached| Ready: Step completed
    MotorSpike --> Ready
    Ready --> Resetting: AReset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start ACalculate]) --> CalcMembrane[Calculate membrane]
    CalcMembrane --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> CheckThreshold{Potential >= Threshold?}
    CheckThreshold -->|Yes| GenerateMotorSpike[Generate motor spike]
    CheckThreshold -->|No| NoSpike[No spike]
    GenerateMotorSpike --> SendToMuscle[Send to muscle]
    NoSpike --> SendToMuscle
    SendToMuscle --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NMotoneuron["NMotoneuron Base"]
        BaseNeuron[NMotoneuron]
    end
    
    subgraph NNewMotoneuron["NNewMotoneuron"]
        Membrane[NPulseMembrane]
        LTZone[NPulseLTZone]
    end
    
    subgraph External["External Components"]
        Channels[Channels]
        Synapses[Synapses]
        Muscle[NMuscle]
    end
    
    BaseNeuron -->|inherits| NNewMotoneuron
    NNewMotoneuron -->|creates| Membrane
    NNewMotoneuron -->|creates| LTZone
    Channels -->|currents| Membrane
    Synapses -->|signals| Channels
    Membrane -->|potential| LTZone
    LTZone -->|motor activity| Muscle
```

### Properties

`NNewMotoneuron` inherits all properties of base class `NMotoneuron` with updated initialization logic.

**Public properties:**
- `ActivePosInputs` (MDMatrix<double>) — matrix of active positive inputs
- `ActiveNegInputs` (MDMatrix<double>) — matrix of active negative inputs
- `ActiveOutputs` (MDMatrix<double>) — matrix of active outputs
- `Activity` (double) — neuron activity
- `Output` (MDMatrix<double>) — output signal (motor activity)

### Methods

#### Public methods

- **`New()`** → `NNewMotoneuron*` — creates new instance of the class

#### Protected methods

- **`ADefault()`** → `bool` — initializes motor neuron parameters with new logic
- **`ABuild()`** → `bool` — builds neuron structure, connects inputs/synapses
- **`AReset()`** → `bool` — resets neuron states
- **`ACalculate()`** → `bool` — calculates motor activity for one step

### Usage in configurations

`NNewMotoneuron` is used in motor system experiments:

- **Motor neurons**: `Bin/Configs/!OldConfigs/OldExperiments/Motoneuron/`
- **Motion control**: `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`

**Features:**
- Updated logic: improved parameter initialization
- Optimized architecture: more efficient signal processing
- Compatibility: inherits functionality from base `NMotoneuron`

### See Also

- [`NMotoneuron`](NMotoneuron.md) — base motor neuron
- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron with structuring parameters
- [`NMuscle`](NMuscle.md) — muscle (receiver of motor activity)
- [Architecture.md](../Architecture.md) — library architecture
