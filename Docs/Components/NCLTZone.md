## NCLTZone — LT-зона (классическая) (Nmsdk-PulseLib)

**Класс**: `NCLTZone` — реализация LT-зоны (долговременная пластичность) для классических нейронов/каналов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCLTZone", ...)`.  
**Storage**: `ClassName = "NCLTZone"`.

### Lifecycle
- **ADefault**: параметры LT (пороги/усиление).
- **ABuild**: подключение к мембранам/каналам.
- **AReset**: сброс состояния LT.
- **ACalculate**: обновление LT-состояния по активности.

### I/O
- Вход: активность/потенциалы.
- Выход: модифицированные сигналы/коэффициенты.

```mermaid
classDiagram
    UComponent <|-- NCLTZone
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NCLTZone
    Mem-->>LT: activity
    LT->>LT: ACalculate()
    LT-->>Mem: modulation
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    act[Activity] --> lt[NCLTZone]
    lt --> mod[Modulation]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCLTZone
Name = CLT1
```

---

## NCLTZone — long-term zone (EN)

Long-term plasticity zone for classic neurons/channels.

```mermaid
classDiagram
    UComponent <|-- NCLTZone
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NCLTZone
    Mem-->>LT: activity
    LT-->>Mem: modulation
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    act[Activity] --> lt[NCLTZone]
    lt --> mod[Modulation]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
