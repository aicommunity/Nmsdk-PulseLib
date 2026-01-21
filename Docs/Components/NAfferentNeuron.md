## NAfferentNeuron — афферентный нейрон (Nmsdk-PulseLib)

**Класс**: `NAfferentNeuron` — базовый афферентный нейрон для приёма внешних стимулов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAfferentNeuron", ...)`.  
**Storage**: `ClassName = "NAfferentNeuron"` в `ClDesc/Configs`.

### Lifecycle
- **ADefault**: инициализация порогов/параметров приёма.
- **ABuild**: подключение входных токов/каналов.
- **AReset**: сброс состояния.
- **ACalculate**: приём стимула, обновление потенциала/активации.

### I/O
- Вход: внешний стимул/ток/событие.
- Выход: активность/спайк афферентного нейрона.

```mermaid
classDiagram
    NNeuron <|-- NAfferentNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Stim as Stimulus
    participant N as NAfferentNeuron
    Stim-->>N: input
    N->>N: ACalculate()
    N-->>Stim: spike/activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    stim[Stimulus] --> n[NAfferentNeuron]
    n --> out[Spike/Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NAfferentNeuron
Name = Afferent1
```

---

## NAfferentNeuron — afferent neuron (EN)

Receives external stimuli and produces spike/activity output.

```mermaid
classDiagram
    NNeuron <|-- NAfferentNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Stim as Stimulus
    participant N as NAfferentNeuron
    Stim-->>N: input
    N-->>Stim: spike/activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    stim[Stimulus] --> n[NAfferentNeuron]
    n --> out[Spike/Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
