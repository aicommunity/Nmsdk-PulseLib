## NSource — источник сигналов для сети

**Класс**: `NSource` — источник внешних сигналов/стимулов для сети.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSource", ...)`.  
**Storage-инстансы**: `ClassName = "NSource"`.

### Lifecycle
- **ADefault**: параметры источника.
- **ABuild**: подключение к генераторам/датасетам.
- **AReset**: сброс позиции/состояния.
- **ACalculate**: выдача следующего стимула.

### I/O
- Вход: (опционально) управление режимом/индексом.
- Выход: стимул (ток/спайки/вектор) в сеть.

```mermaid
classDiagram
    UComponent <|-- NSource
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Src as NSource
    participant Net as NNet
    loop timestep
        Src->>Src: ACalculate()
        Src-->>Net: stimulus
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    src[NSource] --> net[NNet]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NSource
Name = Source1
```

---

## NSource — network input source (EN)

Produces external stimuli for the network.

```mermaid
classDiagram
    UComponent <|-- NSource
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Src as NSource
    participant Net as NNet
    Src-->>Net: stimulus
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    src[NSource] --> net[NNet]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
