## NPainReflexSimple — простой болевой рефлекс

**Класс**: `NPainReflexSimple` — Простой рефлекторный компонент для болевого стимула.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPainReflexSimple", ...)`.
**Storage**: `ClassName = "NPainReflexSimple"`.

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
    UComponent <|-- NPainReflexSimple
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NPainReflexSimple
    In-->>X: signals
    X->>X: ACalculate()
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NPainReflexSimple]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet
```ini
[Component]
ClassName = NPainReflexSimple
Name = NPainReflexSimple1
```

---

## NPainReflexSimple — EN
Class `NPainReflexSimple` — Простой рефлекторный компонент для болевого стимула.

```mermaid
classDiagram
    UComponent <|-- NPainReflexSimple
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NPainReflexSimple
    In-->>X: signals
    X-->>In: outputs
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> x[NPainReflexSimple]
    x --> out[Outputs]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
