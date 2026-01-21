## NContinuesSynMotoneuron — непрерывный мотонейрон (syn)

**Класс**: `NContinuesSynMotoneuron` — вариант мотонейрона с непрерывной обработкой, син. семейство.  
**Регистрация**: `UploadClass("NContinuesSynMotoneuron", ...)`.  
**Storage**: `ClassName = "NContinuesSynMotoneuron"`.

### Lifecycle
- **ADefault**: параметры мотонейрона.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: вычисление управляющей активности.

### I/O
- Вход: сигналы/токи.
- Выход: моторная активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynMotoneuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynMotoneuron
    In-->>N: signals
    N-->>In: motor activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynMotoneuron]
    n --> act[Motor activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NContinuesSynMotoneuron
Name = CSMot1
```

---

## NContinuesSynMotoneuron — continuous motor neuron (EN)

Continuous motoneuron variant in synaptic family.

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynMotoneuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynMotoneuron
    In-->>N: signals
    N-->>In: motor activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynMotoneuron]
    n --> act[Motor activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
