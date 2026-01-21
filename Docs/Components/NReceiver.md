## NReceiver — приёмник выходов сети

**Класс**: `NReceiver` — принимает выходные сигналы/активность сети и преобразует/экспортирует их наружу.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NReceiver", ...)`.  
**Storage-инстансы**: `ClassName = "NReceiver"`.

### Lifecycle
- **ADefault**: параметры приёма/агрегации.
- **ABuild**: подключение к выходам сети/слоёв.
- **AReset**: сброс накопителей.
- **ACalculate**: агрегация активности и выдача наружу.

### I/O
- Вход: активность/спайки (векторы/события).
- Выход: агрегированные признаки/значения (скаляры/векторы), пригодные для логирования/управления.

```mermaid
classDiagram
    UComponent <|-- NReceiver
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Net as NNet
    participant R as NReceiver
    loop timestep
        Net-->>R: activity/spikes
        R->>R: ACalculate()
        R-->>Net: exported output
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    net[NNet activity] --> r[NReceiver]
    r --> out[Exported outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NReceiver
Name = Receiver1
```

---

## NReceiver — network output receiver (EN)

Consumes network outputs and exposes aggregated values for external use.

```mermaid
classDiagram
    UComponent <|-- NReceiver
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Net as NNet
    participant R as NReceiver
    Net-->>R: activity
    R-->>Net: exported output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    net[NNet activity] --> r[NReceiver]
    r --> out[Exported outputs]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
