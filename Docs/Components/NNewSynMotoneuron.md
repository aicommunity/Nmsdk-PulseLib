## NNewSynMotoneuron — новая син. мотонейрон

**Класс**: `NNewSynMotoneuron` — новая модель мотонейрона в син. семействе.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynMotoneuron", ...)`.  
**Storage**: `ClassName = "NNewSynMotoneuron"`.

### Lifecycle
- **ADefault**: параметры мотонейрона.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: расчёт моторной активности.

### I/O
- Вход: сигналы/токи.
- Выход: моторная активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NNewSynMotoneuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSynMotoneuron
    In-->>N: signals
    N-->>In: motor output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSynMotoneuron]
    n --> act[Motor output]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewSynMotoneuron
Name = NewSynMoto1
```

---

## NNewSynMotoneuron — new synaptic motor neuron (EN)

New motor neuron variant in synaptic family.

```mermaid
classDiagram
    NNeuron <|-- NNewSynMotoneuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSynMotoneuron
    In-->>N: signals
    N-->>In: motor output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSynMotoneuron]
    n --> act[Motor output]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
