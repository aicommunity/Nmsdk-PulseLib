## NNewMotoneuron — новая версия мотонейрона

**Класс**: `NNewMotoneuron` — обновлённая модель мотонейрона.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewMotoneuron", ...)`.  
**Storage**: `ClassName = "NNewMotoneuron"`.

### Lifecycle
- **ADefault**: параметры мотонейрона (новая логика).
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния.
- **ACalculate**: расчёт моторной активности.

### I/O
- Вход: сигналы/токи.
- Выход: моторная активность/спайк.

```mermaid
classDiagram
    NMotoneuron <|-- NNewMotoneuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewMotoneuron
    In-->>N: signals
    N-->>In: motor output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewMotoneuron]
    n --> act[Motor output]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewMotoneuron
Name = NewMoto1
```

---

## NNewMotoneuron — new motor neuron (EN)

Updated motor neuron variant.

```mermaid
classDiagram
    NMotoneuron <|-- NNewMotoneuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewMotoneuron
    In-->>N: signals
    N-->>In: motor output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewMotoneuron]
    n --> act[Motor output]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
