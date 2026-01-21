## NCNeuronPosCGenerator — генератор положительных токов (Nmsdk-PulseLib)

**Класс**: `NCNeuronPosCGenerator` — генерирует положительные токи/сигналы для классических нейронов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCNeuronPosCGenerator", ...)`.  
**Storage**: `ClassName = "NCNeuronPosCGenerator"`.

### Lifecycle
- **ADefault**: параметры амплитуды/частоты.
- **ABuild**: подготовка выходов.
- **AReset**: сброс.
- **ACalculate**: формирование положительного тока на шаге.

### I/O
- Вход: (опционально) управление.
- Выход: положительный ток/сигнал.

```mermaid
classDiagram
    UComponent <|-- NCNeuronPosCGenerator
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Ctrl as Control
    participant G as NCNeuronPosCGenerator
    Ctrl-->>G: params
    G->>G: ACalculate()
    G-->>Ctrl: positive current
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    ctrl[Params] --> gen[NCNeuronPosCGenerator]
    gen --> out[Positive current]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCNeuronPosCGenerator
Name = PosCGen1
```

---

## NCNeuronPosCGenerator — positive current generator (EN)

Produces positive currents for classic neurons.

```mermaid
classDiagram
    UComponent <|-- NCNeuronPosCGenerator
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Ctrl as Control
    participant G as NCNeuronPosCGenerator
    Ctrl-->>G: params
    G-->>Ctrl: positive current
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    ctrl[Params] --> gen[NCNeuronPosCGenerator]
    gen --> out[Positive current]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
