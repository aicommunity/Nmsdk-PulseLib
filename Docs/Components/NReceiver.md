# NReceiver — приемник сигналов

## RU

### Назначение

**Класс**: `NReceiver` — компонент для приема выходных сигналов/активности сети и преобразования/экспорта их наружу.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NReceiver", ...)`.  
**Storage-инстансы**: `ClassName = "NReceiver"` в `Bin/Configs/*/Model_*.xml`.

`NReceiver` реализует приемник, который принимает выходные сигналы сети, агрегирует их и экспортирует для внешнего использования (логирование, управление, визуализация). Компонент является базовым классом для приема и обработки выходных сигналов нейронных сетей.

**Использование:** Прием выходных сигналов сети, экспорт данных

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NReceiver
    class NReceiver {
        +New() NReceiver*
    }
```

**Иерархия наследования:**
- `UNet` — базовая сеть Rdk Framework
- `NReceiver` — приемник сигналов

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Receiver as NReceiver
    participant Network as Нейронная сеть
    participant ExternalSystem as Внешняя система
    
    Storage->>Receiver: New()
    Storage->>Receiver: Default()
    Receiver->>Receiver: ADefault()
    Storage->>Receiver: Build()
    Receiver->>Receiver: ABuild()
    Receiver-->>Storage: Ready = true
    
    loop Каждый шаг симуляции
        Network->>Receiver: Выходные сигналы сети
        Storage->>Receiver: Calculate()
        Receiver->>Receiver: ACalculate()
        Receiver->>Receiver: Агрегация входных сигналов
        Receiver->>Receiver: Обработка данных
        Receiver->>Receiver: Экспорт данных
        Receiver-->>ExternalSystem: Экспортированные данные
        Note over ExternalSystem: Логирование, визуализация,<br/>управление, анализ
    end
```

**Жизненный цикл:**
1. **Инициализация**: Установка параметров по умолчанию
2. **Сборка**: Инициализация структуры приемника
3. **Расчет**: Прием выходных сигналов сети, агрегация, обработка, экспорт

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceiveSignals: Прием выходных сигналов
    ReceiveSignals --> AggregateData: Агрегация данных
    AggregateData --> ProcessData: Обработка данных
    ProcessData --> ExportData: Экспорт данных
    ExportData --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Building** — выполняется сборка
- **Built** — структура приемника построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет приемника
- **ReceiveSignals** — прием выходных сигналов сети
- **AggregateData** — агрегация данных
- **ProcessData** — обработка данных
- **ExportData** — экспорт данных
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveSignals[Прием выходных сигналов сети]
    ReceiveSignals --> AggregateData[Агрегация данных]
    AggregateData --> ProcessData[Обработка данных]
    ProcessData --> ExportData[Экспорт данных]
    ExportData --> End([End])
```

**Алгоритм расчета:**
1. Прием выходных сигналов сети
2. Агрегация данных от различных компонентов
3. Обработка данных (нормализация, фильтрация, преобразование)
4. Экспорт данных для внешнего использования (логирование, визуализация, управление)

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end
    
    subgraph NReceiver["NReceiver"]
        Receiver[Приемник сигналов]
        DataAggregator[Агрегатор данных]
        DataExporter[Экспортер данных]
    end
    
    subgraph External["Внешние компоненты"]
        Network[Нейронная сеть]
        Logger[Система логирования]
        Visualizer[Визуализатор]
        Controller[Контроллер]
    end
    
    BaseNet -->|наследуется| NReceiver
    NReceiver -->|реализует| Receiver
    NReceiver -->|использует| DataAggregator
    NReceiver -->|использует| DataExporter
    Network -->|выходные сигналы| NReceiver
    NReceiver -->|экспортированные данные| Logger
    NReceiver -->|экспортированные данные| Visualizer
    NReceiver -->|экспортированные данные| Controller
```

**Зависимости:**
- **Базовый класс**: `UNet`
- **Внутренние компоненты**: приемник сигналов, агрегатор данных, экспортер данных
- **Внешние компоненты**: нейронная сеть (источник выходных сигналов), системы логирования, визуализации, управления (получатели экспортированных данных)

### Свойства

`NReceiver` является базовым классом и не имеет собственных публичных свойств. Производные классы могут добавлять входные и выходные свойства для приема и экспорта данных.

### Методы

#### Публичные методы

- **`New()`** → `NReceiver*` — создает новый экземпляр класса.

### Примеры использования

#### Пример 1: Создание приемника в коде C++

```cpp
// Создание приемника
auto receiver = storage->CreateComponent<NReceiver>();
receiver->SetName("Receiver");

// Инициализация
receiver->Default();

// Использование
receiver->Build();
```

### Использование в конфигурациях

`NReceiver` используется как базовый класс для приемников сигналов:

- **Приемники сигналов**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (производные классы)

**Особенности:**
- Базовый класс: не используется напрямую, только через производные классы
- Прием сигналов: получает выходные сигналы нейронной сети
- Экспорт данных: экспортирует данные для внешнего использования (логирование, визуализация, управление)

### См. также

- [`NSource`](NSource.md) — источник сигналов
- [`NReceptor`](NReceptor.md) — рецептор
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NReceiver` — component for receiving network output signals/activity and transforming/exporting them externally.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NReceiver", ...)`.  
**Instances**: `ClassName = "NReceiver"` in `Bin/Configs/*/Model_*.xml`.

`NReceiver` implements receiver that receives network output signals, aggregates them, and exports for external use (logging, control, visualization). Component is base class for receiving and processing neural network output signals.

**Usage:** Receiving network output signals, data export

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NReceiver
    class NReceiver {
        +New() NReceiver*
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Receiver as NReceiver
    participant Network
    participant ExternalSystem
    
    Storage->>Receiver: New() + Default()
    Storage->>Receiver: Build()
    loop Each step
        Network->>Receiver: Output signals
        Storage->>Receiver: Calculate()
        Receiver->>Receiver: Aggregate data
        Receiver->>Receiver: Export data
        Receiver-->>ExternalSystem: Exported data
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Built: Build()
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceiveSignals: Receive signals
    ReceiveSignals --> AggregateData: Aggregate data
    AggregateData --> ExportData: Export data
    ExportData --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveSignals[Receive signals]
    ReceiveSignals --> AggregateData[Aggregate data]
    AggregateData --> ProcessData[Process data]
    ProcessData --> ExportData[Export data]
    ExportData --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end
    
    subgraph NReceiver["NReceiver"]
        Receiver[Signal Receiver]
        DataAggregator[Data Aggregator]
        DataExporter[Data Exporter]
    end
    
    subgraph External["External Components"]
        Network[Neural Network]
        Logger[Logging System]
        Visualizer[Visualizer]
        Controller[Controller]
    end
    
    BaseNet -->|inherits| NReceiver
    NReceiver -->|implements| Receiver
    NReceiver -->|uses| DataAggregator
    NReceiver -->|uses| DataExporter
    Network -->|output signals| NReceiver
    NReceiver -->|exported data| Logger
    NReceiver -->|exported data| Visualizer
    NReceiver -->|exported data| Controller
```

### Properties

`NReceiver` does not have its own public properties (UProperty). All properties are inherited from the base class `UNet`. Derived classes may add specific properties for data reception and export.

### Methods

- `New()` — creates new receiver instance
- `ADefault()` — sets default parameters (inherited from UNet)
- `ABuild()` — builds receiver structure (inherited from UNet)
- `AReset()` — resets receiver state (inherited from UNet)
- `ACalculate()` — performs one calculation step (receives signals, aggregates data, exports)

**Inherited methods from UNet:**
- `AddComponent()` — add component to receiver
- `CreateLink()` — create link between components
- `Default()` / `Build()` / `Reset()` / `Calculate()` — lifecycle methods

### Usage in configurations

`NReceiver` is used as base class for signal receivers:

- **Signal receivers**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (derived classes)

**Features:**
- Base class: not used directly, only through derived classes
- Signal reception: receives network output signals
- Data export: exports data for external use (logging, visualization, control)

### See Also

- [`NSource`](NSource.md) — signal source
- [`NReceptor`](NReceptor.md) — receptor
- [Architecture.md](../Architecture.md) — library architecture
