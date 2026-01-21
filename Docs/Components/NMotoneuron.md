## NMotoneuron — мотонейрон

**Класс**: `NMotoneuron` — мотонейрон (классический/базовый).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NMotoneuron", ...)`.  
**Storage**: `ClassName = "NMotoneuron"`.

### Lifecycle
- **ADefault**: параметры мотонейрона.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния.
- **ACalculate**: генерация моторной активности.

### I/O
- Вход: сигналы/токи.
- Выход: моторная активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NMotoneuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NMotoneuron
    In-->>N: signals
    N-->>In: motor output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NMotoneuron]
    n --> act[Motor output]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NMotoneuron
Name = Motoneuron1
```

---

## NMotoneuron — motor neuron (EN)

Motor neuron producing motor output from input signals.

```mermaid
classDiagram
    NNeuron <|-- NMotoneuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NMotoneuron
    In-->>N: signals
    N-->>In: motor output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NMotoneuron]
    n --> act[Motor output]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
