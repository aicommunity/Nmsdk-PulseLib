## NContinuesSynRenshowCell — непрерывная клетка Реншоу (syn)

**Класс**: `NContinuesSynRenshowCell` — вариант клетки Реншоу с непрерывной обработкой.  
**Регистрация**: `UploadClass("NContinuesSynRenshowCell", ...)`.  
**Storage**: `ClassName = "NContinuesSynRenshowCell"`.

### Lifecycle
- **ADefault**: параметры/пороги.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: расчёт тормозного ответа.

### I/O
- Вход: сигналы/токи.
- Выход: тормозная активность.

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynRenshowCell
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynRenshowCell
    In-->>N: signals
    N-->>In: inhibitory output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynRenshowCell]
    n --> inh[Inhibitory output]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NContinuesSynRenshowCell
Name = CSRenshow1
```

---

## NContinuesSynRenshowCell — continuous Renshaw cell (EN)

Continuous inhibitory Renshaw cell variant.

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynRenshowCell
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynRenshowCell
    In-->>N: signals
    N-->>In: inhibitory output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynRenshowCell]
    n --> inh[Inhibitory output]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
