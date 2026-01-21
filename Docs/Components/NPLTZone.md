## NPLTZone — LT-зона (базовая)

**Класс**: `NPLTZone` — базовая зона долговременной пластичности.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTZone", ...)`.  
**Storage**: `ClassName = "NPLTZone"`.

### Lifecycle
- **ADefault**: параметры LT (пороги/усиление).
- **ABuild**: подключение к мембранам/каналам.
- **AReset**: сброс LT-состояния.
- **ACalculate**: обновление LT по активности.

### I/O
- Вход: активность/потенциалы.
- Выход: модифицированные сигналы/коэффициенты.

```mermaid
classDiagram
    UComponent <|-- NPLTZone
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NPLTZone
    Mem-->>LT: activity
    LT->>LT: ACalculate()
    LT-->>Mem: modulation
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    act[Activity] --> lt[NPLTZone]
    lt --> mod[Modulated]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTZone
Name = LTZone1
```

---

## NPLTZone — long-term zone (EN)

Base long-term plasticity zone.

```mermaid
classDiagram
    UComponent <|-- NPLTZone
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NPLTZone
    Mem-->>LT: activity
    LT-->>Mem: modulation
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    act[Activity] --> lt[NPLTZone]
    lt --> mod[Modulated]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
