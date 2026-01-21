## NCGenerator — генератор токов (Nmsdk-PulseLib)

**Класс**: `NCGenerator` — генерирует токи/сигналы для нейросети.  
**Регистрация**: `UploadClass("NCGenerator", ...)` в `NPulseLibrary.cpp`.  
**Storage**: `ClassName = "NCGenerator"`.

### Lifecycle
- **ADefault**: параметры генерации (амплитуда/частота/форма).
- **ABuild**: подготовка выходов/привязка к потребителям.
- **AReset**: сброс генератора.
- **ACalculate**: выдача следующего значения.

### I/O
- Вход: (опционально) управление параметрами.
- Выход: ток/сигнал (скаляр/вектор).

```mermaid
classDiagram
    UComponent <|-- NCGenerator
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant G as NCGenerator
    Cfg-->>G: params
    loop timestep
        G->>G: ACalculate()
        G-->>Cfg: current
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    cfg[Params] --> gen[NCGenerator]
    gen --> out[Current/Signal]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCGenerator
Name = CGen1
```

---

## NCGenerator — current generator (EN)

Generates current/signal according to configured parameters.

```mermaid
classDiagram
    UComponent <|-- NCGenerator
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant G as NCGenerator
    Cfg-->>G: params
    G-->>Cfg: current
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    cfg[Params] --> gen[NCGenerator]
    gen --> out[Current/Signal]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
