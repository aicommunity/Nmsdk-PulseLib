## NPGeneratorDelay — генератор с задержкой

**Класс**: `NPGeneratorDelay` — генератор импульсов с задержкой.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPGeneratorDelay", ...)`.  
**Storage**: `ClassName = "NPGeneratorDelay"`.

### Lifecycle
- **ADefault**: параметры генерации + задержка.
- **ABuild**: подготовка буфера задержки.
- **AReset**: очистка буфера.
- **ACalculate**: генерация + задержка выдачи.

### I/O
- Вход: управление.
- Выход: задержанный импульс.

```mermaid
classDiagram
    NPGenerator <|-- NPGeneratorDelay
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant G as NPGeneratorDelay
    Cfg-->>G: params
    G->>G: generate + delay
    G-->>Cfg: delayed spike
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    cfg[Params] --> gen[NPGeneratorDelay]
    gen --> dspike[Delayed spike]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPGeneratorDelay
Name = PGenDelay1
```

---

## NPGeneratorDelay — delayed pulse generator (EN)

Pulse generator with delay buffer.

```mermaid
classDiagram
    NPGenerator <|-- NPGeneratorDelay
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant G as NPGeneratorDelay
    Cfg-->>G: params
    G-->>Cfg: delayed spike
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    cfg[Params] --> gen[NPGeneratorDelay]
    gen --> dspike[Delayed spike]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
