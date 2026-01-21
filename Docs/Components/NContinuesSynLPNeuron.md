## NContinuesSynLPNeuron — непрерывный LP нейрон (syn)

**Класс**: `NContinuesSynLPNeuron` — непрерывный LP-нейрон в син. семействе.  
**Регистрация**: `UploadClass("NContinuesSynLPNeuron", ...)` в `NPulseLibrary.cpp`.  
**Storage**: `ClassName = "NContinuesSynLPNeuron"`.

### Lifecycle
- **ADefault**: параметры LP.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: непрерывный LP-расчёт.

### I/O
- Вход: сигналы/токи.
- Выход: активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynLPNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynLPNeuron
    In-->>N: signals
    N-->>In: activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynLPNeuron]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NContinuesSynLPNeuron
Name = CSLP1
```

---

## NContinuesSynLPNeuron — continuous LP neuron (EN)

Continuous LP neuron variant (synaptic family).

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynLPNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynLPNeuron
    In-->>N: signals
    N-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynLPNeuron]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
