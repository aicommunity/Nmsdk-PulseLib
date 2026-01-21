## NNewSynRenshowCell — новая син. клетка Реншоу

**Класс**: `NNewSynRenshowCell` — новая клетка Реншоу в син. семействе.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynRenshowCell", ...)`.  
**Storage**: `ClassName = "NNewSynRenshowCell"`.

### Lifecycle
- **ADefault**: параметры клетки Реншоу.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: расчёт тормозного ответа.

### I/O
- Вход: сигналы/токи.
- Выход: ингибирующая активность.

```mermaid
classDiagram
    NNeuron <|-- NNewSynRenshowCell
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSynRenshowCell
    In-->>N: signals
    N-->>In: inhibitory output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSynRenshowCell]
    n --> inh[Inhibitory output]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewSynRenshowCell
Name = NewSynRenshow1
```

---

## NNewSynRenshowCell — new synaptic Renshaw cell (EN)

New Renshaw cell variant in synaptic family.

```mermaid
classDiagram
    NNeuron <|-- NNewSynRenshowCell
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSynRenshowCell
    In-->>N: signals
    N-->>In: inhibitory output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSynRenshowCell]
    n --> inh[Inhibitory output]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
