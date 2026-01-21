## NPSimpleLTZone — простая LT-зона (pulse)

**Класс**: `NPSimpleLTZone` — Простая LT-зона для пульсовых нейронов.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPSimpleLTZone", ...)`.
**Storage**: `ClassName = "NPSimpleLTZone"`.

### Lifecycle
- **ADefault**: параметры по умолчанию.
- **ABuild**: подключение входов/синапсов/каналов.
- **AReset**: сброс состояния/потенциалов.
- **ACalculate**: шаг расчёта (интеграция/передача/модуляция).

### I/O
- Вход: сигналы/токи/спайки (по назначению класса).
- Выход: потенциал/спайк/модулированный сигнал.

```mermaid
classDiagram
    UComponent <|-- NPSimpleLTZone
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NPSimpleLTZone
    In-->>X: signals
    X->>X: ACalculate()
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NPSimpleLTZone]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet
```ini
[Component]
ClassName = NPSimpleLTZone
Name = NPSimpleLTZone1
```

---

## NPSimpleLTZone — EN
Class `NPSimpleLTZone` — Простая LT-зона для пульсовых нейронов.

```mermaid
classDiagram
    UComponent <|-- NPSimpleLTZone
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NPSimpleLTZone
    In-->>X: signals
    X-->>In: outputs
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> x[NPSimpleLTZone]
    x --> out[Outputs]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
