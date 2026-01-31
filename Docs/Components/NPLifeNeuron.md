# NPLifeNeuron — нейрон Life (pulse)

## RU

### Назначение

**Класс**: `NPLifeNeuron` — пульсовый нейрон с метриками Life.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLifeNeuron", ...)`.
**Storage-инстансы**: `ClassName = "NPLifeNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NPLifeNeuron` является пульсовым нейроном с метриками жизнеобеспечения (Life). Наследуется от `UComponent` и интегрирует модель жизнеобеспечения для отслеживания энергии, износа и других метрик жизнедеятельности нейрона.

**Использование:** Моделирование нейронов с метриками жизнеобеспечения, эксперименты с энергией и износом

### UML-диаграмма классов

```mermaid
classDiagram
    UComponent <|-- NPLifeNeuron
    NPLifeNeuron *-- NNeuronLife : LifeModel
    class UComponent {
        +New() UComponent*
    }
    class NPLifeNeuron {
        +New() NPLifeNeuron*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
    class NNeuronLife {
        +Energy : double
        +Threshold : double
        +CriticalEnergy : double
        +WearOut : double
    }
```

**Иерархия наследования:**
- `UComponent` — базовый компонент Rdk Framework
- `NPLifeNeuron` — пульсовый нейрон с метриками Life

**Внутренняя структура:**
- **LifeModel** (`NNeuronLife`) — модель жизнеобеспечения нейрона

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Neuron as NPLifeNeuron
    participant LifeModel as NNeuronLife
    participant InputSource as Источник входных сигналов
    participant OutputTarget as Целевой компонент

    Storage->>Neuron: New()
    Storage->>Neuron: Default()
    Neuron->>Neuron: ADefault()
    Note over Neuron: Инициализация параметров по умолчанию
    Storage->>Neuron: Build()
    Neuron->>Neuron: ABuild()
    Neuron->>LifeModel: CreateComponent("NeuronLife")
    Neuron->>Neuron: Подключение входов/синапсов/каналов
    Neuron-->>Storage: Ready = true

    loop Каждый шаг симуляции
        InputSource->>Neuron: Входные сигналы/токи/спайки
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: ACalculate()
        Neuron->>Neuron: Интеграция/передача/модуляция
        Neuron->>LifeModel: Обновление метрик Life
        LifeModel->>LifeModel: Обновление Energy, WearOut
        LifeModel-->>Neuron: Метрики жизнеобеспечения
        Neuron->>Neuron: Обновление выходного сигнала
        Neuron-->>OutputTarget: Потенциал/спайк/модулированный сигнал
    end
```

**Жизненный цикл:**
1. **Инициализация**: Установка параметров по умолчанию
2. **Сборка**: Подключение входов, синапсов, каналов и создание модели Life
3. **Расчет**: Интеграция сигналов, передача, модуляция и обновление метрик Life
4. **Сброс**: Сброс состояния/потенциалов и метрик Life

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateLifeModel: Создание NNeuronLife
    CreateLifeModel --> ConnectComponents: Подключение входов/синапсов/каналов
    ConnectComponents --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Resetting: Reset()
    Resetting --> ResetLifeModel: Сброс метрик Life
    ResetLifeModel --> Ready: Состояния сброшены
    Ready --> Calculating: Calculate()
    Calculating --> Integration: Интеграция сигналов
    Integration --> Transmission: Передача сигналов
    Transmission --> Modulation: Модуляция сигналов
    Modulation --> UpdateLife: Обновление метрик Life
    UpdateLife --> Ready: Шаг завершен
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Building** — выполняется сборка структуры
- **CreateLifeModel** — создание модели жизнеобеспечения
- **ConnectComponents** — подключение входов, синапсов, каналов
- **Built** — структура нейрона построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет нейрона
- **Integration** — интеграция сигналов
- **Transmission** — передача сигналов
- **Modulation** — модуляция сигналов
- **UpdateLife** — обновление метрик Life
- **Resetting** — выполняется сброс состояний
- **ResetLifeModel** — сброс метрик Life

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> ProcessInputs[Обработка входных сигналов/токов/спайков]
    ProcessInputs --> Integration[Интеграция сигналов]
    Integration --> Transmission[Передача сигналов]
    Transmission --> Modulation[Модуляция сигналов]
    Modulation --> UpdateLifeMetrics[Обновление метрик Life]
    UpdateLifeMetrics --> CheckEnergy{Энергия критична?}
    CheckEnergy -->|Да| AdjustActivity[Корректировка активности]
    CheckEnergy -->|Нет| UpdateOutput[Обновление выходного сигнала]
    AdjustActivity --> UpdateOutput
    UpdateOutput --> End([End])
```

**Алгоритм расчета:**
1. Обработка входных сигналов/токов/спайков
2. Интеграция сигналов
3. Передача сигналов
4. Модуляция сигналов
5. Обновление метрик Life (энергия, износ)
6. Проверка критичности энергии
7. Корректировка активности при необходимости
8. Обновление выходного сигнала

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph UComponent["UComponent Base"]
        BaseComponent[UComponent]
    end

    subgraph NPLifeNeuron["NPLifeNeuron"]
        LifeNeuron[Нейрон Life]
        LifeModel[NNeuronLife<br/>Life Model]
        Membrane[Мембрана]
        Channels[Каналы]
        Synapses[Синапсы]
    end

    subgraph External["Внешние компоненты"]
        InputSource[Источник входных сигналов]
        EnergySource[Источник энергии]
        OutputTarget[Целевой компонент]
    end

    BaseComponent -->|наследуется| NPLifeNeuron
    NPLifeNeuron -->|создает| LifeModel
    NPLifeNeuron -->|создает| Membrane
    NPLifeNeuron -->|создает| Channels
    NPLifeNeuron -->|создает| Synapses
    InputSource -->|сигналы/токи/спайки| Synapses
    Synapses -->|ток| Membrane
    Channels -->|ток| Membrane
    Membrane -->|потенциал| NPLifeNeuron
    EnergySource -->|энергия| LifeModel
    LifeModel -->|метрики| NPLifeNeuron
    NPLifeNeuron -->|потенциал/спайк/модулированный сигнал| OutputTarget
```

**Зависимости:**
- **Базовый класс**: `UComponent`
- **Внутренние компоненты**: `NNeuronLife` (модель жизнеобеспечения), мембрана, каналы, синапсы
- **Внешние компоненты**: источники входных сигналов, источник энергии, целевые компоненты

### Свойства

`NPLifeNeuron` использует свойства модели жизнеобеспечения (`NNeuronLife`):

**Параметры жизнеобеспечения (в NNeuronLife):**
- `Energy` (double) — текущая энергия нейрона
- `Threshold` (double) — порог жизнедеятельности
- `CriticalEnergy` (double) — критический уровень энергии
- `WearOut` (double) — износ нейрона

### Методы

- `New()` → `NPLifeNeuron*` — создает новый экземпляр нейрона
- `ADefault()` → `bool` — установка параметров по умолчанию
- `ABuild()` → `bool` — сборка структуры нейрона (подключение входов/синапсов/каналов)
- `AReset()` → `bool` — сброс состояния/потенциалов и метрик Life
- `ACalculate()` → `bool` — выполнение шага расчета (интеграция/передача/модуляция)
- `GetNeuronLife()` → `NNeuronLife*` — получение модели жизнеобеспечения

### Примеры использования

#### Пример 1: Создание нейрона Life в коде C++

```cpp
// Создание пульсового нейрона с метриками Life
auto neuron = storage->CreateComponent("NPLifeNeuron");
neuron->SetName("NPLifeNeuron1");

// Инициализация
neuron->Default();

// Использование
neuron->Build();
for (int step = 0; step < 1000; step++) {
    neuron->Calculate();
}
```

#### Пример 2: Конфигурация XML

```xml
<Neuron1 Class="NPLifeNeuron">
    <Parameters>
        <!-- Параметры жизнеобеспечения -->
    </Parameters>
</Neuron1>
```

### Использование в конфигурациях

`NPLifeNeuron` используется для моделирования нейронов с метриками жизнеобеспечения:

- **Life-модели**: `Bin/Configs/*/Model_*.xml` (где требуется отслеживание метрик жизнедеятельности)
- **Энергетические модели**: эксперименты с энергией и износом нейронов

**Особенности:**
- Метрики жизнеобеспечения: отслеживание энергии, износа и других метрик
- Энергетическая модель: интеграция модели жизнеобеспечения
- Корректировка активности: автоматическая корректировка при критических уровнях энергии

## Источники

См. [Literature-References.md](../Literature-References.md): **neuromodeler.ru** (жизнеобеспечение), **15**.

### См. также

- [`NNeuronLife`](NNeuronLife.md) — модель жизнеобеспечения нейрона
- [`NSPLifeNeuron`](NSPLifeNeuron.md) — SP-нейрон с метриками Life
- [`NLPLifeNeuron`](NLPLifeNeuron.md) — LP-нейрон с метриками Life
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPLifeNeuron` — spiking neuron with Life metrics.
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPLifeNeuron", ...)`.
**Instances**: `ClassName = "NPLifeNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NPLifeNeuron` is a spiking neuron with life support metrics. It inherits from `UComponent` and integrates a life support model for tracking energy, wear out, and other neuron vitality metrics.

**Usage:** Modeling neurons with life support metrics, experiments with energy and wear out

### UML Class Diagram

```mermaid
classDiagram
    UComponent <|-- NPLifeNeuron
    NPLifeNeuron *-- NNeuronLife : LifeModel
    class UComponent {
        +New() UComponent*
    }
    class NPLifeNeuron {
        +New() NPLifeNeuron*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
    class NNeuronLife {
        +Energy : double
        +Threshold : double
        +CriticalEnergy : double
        +WearOut : double
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NPLifeNeuron
    participant LifeModel as NNeuronLife
    participant InputSource
    participant OutputTarget

    Storage->>Neuron: New()
    Storage->>Neuron: Default()
    Neuron->>Neuron: ADefault()
    Note over Neuron: Initialize default parameters
    Storage->>Neuron: Build()
    Neuron->>Neuron: ABuild()
    Neuron->>LifeModel: CreateComponent("NeuronLife")
    Neuron->>Neuron: Connect inputs/synapses/channels
    Neuron-->>Storage: Ready = true

    loop Each simulation step
        InputSource->>Neuron: Input signals/currents/spikes
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: ACalculate()
        Neuron->>Neuron: Integration/transmission/modulation
        Neuron->>LifeModel: Update Life metrics
        LifeModel->>LifeModel: Update Energy, WearOut
        LifeModel-->>Neuron: Life support metrics
        Neuron->>Neuron: Update output signal
        Neuron-->>OutputTarget: Potential/spike/modulated signal
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateLifeModel: Create NNeuronLife
    CreateLifeModel --> ConnectComponents: Connect inputs/synapses/channels
    ConnectComponents --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Resetting: Reset()
    Resetting --> ResetLifeModel: Reset Life metrics
    ResetLifeModel --> Ready
    Ready --> Calculating: Calculate()
    Calculating --> Integration: Integrate signals
    Integration --> Transmission: Transmit signals
    Transmission --> Modulation: Modulate signals
    Modulation --> UpdateLife: Update Life metrics
    UpdateLife --> Ready: Step completed
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ProcessInputs[Process input signals/currents/spikes]
    ProcessInputs --> Integration[Integrate signals]
    Integration --> Transmission[Transmit signals]
    Transmission --> Modulation[Modulate signals]
    Modulation --> UpdateLifeMetrics[Update Life metrics]
    UpdateLifeMetrics --> CheckEnergy{Energy critical?}
    CheckEnergy -->|Yes| AdjustActivity[Adjust activity]
    CheckEnergy -->|No| UpdateOutput[Update output signal]
    AdjustActivity --> UpdateOutput
    UpdateOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UComponent["UComponent Base"]
        BaseComponent[UComponent]
    end

    subgraph NPLifeNeuron["NPLifeNeuron"]
        LifeNeuron[Life Neuron]
        LifeModel[NNeuronLife<br/>Life Model]
        Membrane[Membrane]
        Channels[Channels]
        Synapses[Synapses]
    end

    subgraph External["External Components"]
        InputSource[Input source]
        EnergySource[Energy source]
        OutputTarget[Output target]
    end

    BaseComponent -->|inherits| NPLifeNeuron
    NPLifeNeuron -->|creates| LifeModel
    NPLifeNeuron -->|creates| Membrane
    NPLifeNeuron -->|creates| Channels
    NPLifeNeuron -->|creates| Synapses
    InputSource -->|signals/currents/spikes| Synapses
    Synapses -->|current| Membrane
    Channels -->|current| Membrane
    Membrane -->|potential| NPLifeNeuron
    EnergySource -->|energy| LifeModel
    LifeModel -->|metrics| NPLifeNeuron
    NPLifeNeuron -->|potential/spike/modulated signal| OutputTarget
```

### Properties

`NPLifeNeuron` uses properties of the life support model (`NNeuronLife`):

**Life support parameters (in NNeuronLife):**
- `Energy` (double) — current neuron energy
- `Threshold` (double) — life threshold
- `CriticalEnergy` (double) — critical energy level
- `WearOut` (double) — neuron wear out

### Methods

- `New()` → `NPLifeNeuron*` — creates new neuron instance
- `ADefault()` → `bool` — sets default parameters
- `ABuild()` → `bool` — builds neuron structure (connects inputs/synapses/channels)
- `AReset()` → `bool` — resets state/potentials and Life metrics
- `ACalculate()` → `bool` — performs calculation step (integration/transmission/modulation)
- `GetNeuronLife()` → `NNeuronLife*` — gets life support model

### Usage in configurations

`NPLifeNeuron` is used for modeling neurons with life support metrics:

- **Life models**: `Bin/Configs/*/Model_*.xml` (where life support metrics tracking is required)
- **Energy models**: experiments with energy and wear out for neurons

**Features:**
- Life support metrics: tracks energy, wear out, and other metrics
- Energy model: integrates life support model
- Activity adjustment: automatically adjusts activity at critical energy levels

### References

See [Literature-References.md](../Literature-References.md): **neuromodeler.ru** (life support), **15**.

### See Also

- [`NNeuronLife`](NNeuronLife.md) — neuron life support model
- [`NSPLifeNeuron`](NSPLifeNeuron.md) — SP neuron with Life metrics
- [`NLPLifeNeuron`](NLPLifeNeuron.md) — LP neuron with Life metrics
- [Architecture.md](../Architecture.md) — library architecture
