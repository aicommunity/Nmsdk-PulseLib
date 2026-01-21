## NPGenerator — генератор импульсов (базовый)

**Класс**: `NPGenerator` — базовый генератор импульсов/спайков.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPGenerator", ...)`.  
**Storage**: `ClassName = "NPGenerator"`.

### Lifecycle
- **ADefault**: параметры генерации (частота/паттерн).
- **ABuild**: подготовка выходов.
- **AReset**: сброс генератора.
- **ACalculate**: выдача следующего импульса.

### I/O
- Вход: (опционально) управление частотой/паттерном.
- Выход: импульс/спайк.

```mermaid
classDiagram
    UComponent <|-- NPGenerator
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant G as NPGenerator
    Cfg-->>G: params
    loop timestep
        G-->>Cfg: spike
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    cfg[Params] --> gen[NPGenerator]
    gen --> spike[Spike]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPGenerator
Name = PGen1
```

---

## NPGenerator — pulse generator (EN)

Base pulse/spike generator.

```mermaid
classDiagram
    UComponent <|-- NPGenerator
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant G as NPGenerator
    Cfg-->>G: params
    G-->>Cfg: spike
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    cfg[Params] --> gen[NPGenerator]
    gen --> spike[Spike]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
