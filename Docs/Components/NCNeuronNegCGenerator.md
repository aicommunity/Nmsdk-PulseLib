## NCNeuronNegCGenerator — генератор отрицательных токов (Nmsdk-PulseLib)

**Класс**: `NCNeuronNegCGenerator` — генерирует отрицательные токи/сигналы для классических нейронов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCNeuronNegCGenerator", ...)`.  
**Storage**: `ClassName = "NCNeuronNegCGenerator"`.

### Lifecycle
- **ADefault**: параметры амплитуды/частоты.
- **ABuild**: подготовка выходов.
- **AReset**: сброс.
- **ACalculate**: формирование отрицательного тока на шаге.

### I/O
- Вход: (опционально) управление.
- Выход: отрицательный ток/сигнал.

```mermaid
classDiagram
    UComponent <|-- NCNeuronNegCGenerator
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Ctrl as Control
    participant G as NCNeuronNegCGenerator
    Ctrl-->>G: params
    G->>G: ACalculate()
    G-->>Ctrl: negative current
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    ctrl[Params] --> gen[NCNeuronNegCGenerator]
    gen --> out[Negative current]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCNeuronNegCGenerator
Name = NegCGen1
```

---

## NCNeuronNegCGenerator — negative current generator (EN)

Produces negative currents for classic neurons.

```mermaid
classDiagram
    UComponent <|-- NCNeuronNegCGenerator
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Ctrl as Control
    participant G as NCNeuronNegCGenerator
    Ctrl-->>G: params
    G-->>Ctrl: negative current
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    ctrl[Params] --> gen[NCNeuronNegCGenerator]
    gen --> out[Negative current]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
