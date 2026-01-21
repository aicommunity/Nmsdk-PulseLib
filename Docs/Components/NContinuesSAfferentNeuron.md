## NContinuesSAfferentNeuron — непрерывный афферент (Nmsdk-PulseLib)

**Класс**: `NContinuesSAfferentNeuron` — непрерывный вариант афферентного нейрона.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSAfferentNeuron", ...)`.  
**Storage**: `ClassName = "NContinuesSAfferentNeuron"`.

### Lifecycle
- **ADefault**: параметры непрерывной обработки.
- **ABuild**: подключение входа.
- **AReset**: сброс состояния.
- **ACalculate**: интеграция непрерывного стимула.

### I/O
- Вход: непрерывный сигнал/ток.
- Выход: активность/спайк.

```mermaid
classDiagram
    NAfferentNeuron <|-- NContinuesSAfferentNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Stim as Stimulus
    participant N as NContinuesSAfferentNeuron
    Stim-->>N: signal
    N-->>Stim: activity/spike
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    stim[Signal] --> n[NContinuesSAfferentNeuron]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NContinuesSAfferentNeuron
Name = CAfferent1
```

---

## NContinuesSAfferentNeuron — continuous afferent neuron (EN)

Continuous afferent neuron variant handling continuous stimuli.

```mermaid
classDiagram
    NAfferentNeuron <|-- NContinuesSAfferentNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Stim as Stimulus
    participant N as NContinuesSAfferentNeuron
    Stim-->>N: signal
    N-->>Stim: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    stim[Signal] --> n[NContinuesSAfferentNeuron]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
