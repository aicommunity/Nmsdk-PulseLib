## NModel — модель SNN/контейнер сценария (Nmsdk-PulseLib)

**Класс**: `NModel` — модель верхнего уровня, которая собирает сеть/сценарий и управляет запуском.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NModel", ...)`.  
**Storage-инстансы**: `ClassName = "NModel"`.

### Lifecycle
- **ADefault**: параметры сценария/модели.
- **ABuild**: построение внутренней сети/подмоделей.
- **AReset**: сброс симуляции.
- **ACalculate**: шаг симуляции/обработки.

### I/O
- Вход: конфигурационные параметры, внешние сигналы.
- Выход: результаты модели (активность, метрики, действия).

```mermaid
classDiagram
    UComponent <|-- NModel
```

Диаграмма фиксирует `NModel` как компонент-обёртку над сетью/сценарием.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant M as NModel
    participant Net as NNet
    Cfg->>M: create + params
    M->>Net: build network
    loop timestep
        M->>Net: step
        Net-->>M: activity
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    cfg[Config] --> m[NModel]
    m --> net[NNet]
    net --> out[Results]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NModel
Name = Model1
```

---

## NModel — top-level model (EN)

Wraps a network/scenario and runs simulation steps.

```mermaid
classDiagram
    UComponent <|-- NModel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant M as NModel
    participant Net as NNet
    M->>Net: step
    Net-->>M: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    cfg[Config] --> m[NModel]
    m --> net[NNet]
    net --> out[Results]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
