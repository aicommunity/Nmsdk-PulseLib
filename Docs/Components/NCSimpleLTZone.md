## NCSimpleLTZone — LT-зона (простая, classic)

**Класс**: `NCSimpleLTZone` — простая зона долговременной пластичности для классических нейронов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCSimpleLTZone", ...)`.  
**Storage**: `ClassName = "NCSimpleLTZone"`.

### Lifecycle
- **ADefault**: установка порогов/усилений LT.
- **ABuild**: подключение к мембранам/каналам.
- **AReset**: сброс состояния LT.
- **ACalculate**: обновление LT-состояния по активности.

### I/O
- Вход: активность/потенциал.
- Выход: модифицированный сигнал/модуляция.

```mermaid
classDiagram
    UComponent <|-- NCSimpleLTZone
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NCSimpleLTZone
    Mem-->>LT: activity
    LT->>LT: ACalculate()
    LT-->>Mem: modulation
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    act[Activity] --> lt[NCSimpleLTZone]
    lt --> mod[Modulated]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCSimpleLTZone
Name = CLTZone1
```

---

## NCSimpleLTZone — simple LT zone (EN)

Simple long-term plasticity zone for classic neurons.

```mermaid
classDiagram
    UComponent <|-- NCSimpleLTZone
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NCSimpleLTZone
    Mem-->>LT: activity
    LT-->>Mem: modulation
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    act[Activity] --> lt[NCSimpleLTZone]
    lt --> mod[Modulated]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
