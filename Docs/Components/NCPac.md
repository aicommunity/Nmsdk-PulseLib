## NCPac — счётчик/активатор (Nmsdk-PulseLib)

**Класс**: `NCPac` — компонент, связанный с подсчётом/активностью (Pac).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCPac", ...)`.  
**Storage**: `ClassName = "NCPac"`.

### Lifecycle
- **ADefault**: начальные параметры.
- **ABuild**: подключение входов.
- **AReset**: сброс счётчиков.
- **ACalculate**: обновление состояния/счёта.

### I/O
- Вход: активность/события.
- Выход: счёт/сигнал активности.

```mermaid
classDiagram
    UComponent <|-- NCPac
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Input
    participant P as NCPac
    In-->>P: events
    P->>P: ACalculate()
    P-->>In: count/signal
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    ev[Events] --> p[NCPac]
    p --> c[Count]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCPac
Name = CPac1
```

---

## NCPac — count/activation component (EN)

Counts or accumulates activity/events.

```mermaid
classDiagram
    UComponent <|-- NCPac
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Input
    participant P as NCPac
    In-->>P: events
    P-->>In: count/signal
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    ev[Events] --> p[NCPac]
    p --> c[Count]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
