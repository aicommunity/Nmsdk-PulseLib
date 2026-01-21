## NNewRenshowCell — новая клетка Реншоу

**Класс**: `NNewRenshowCell` — обновлённая модель клетки Реншоу (ингибирующий интернейрон).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewRenshowCell", ...)`.  
**Storage**: `ClassName = "NNewRenshowCell"`.

### Lifecycle
- **ADefault**: параметры клетки Реншоу (новая реализация).
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния.
- **ACalculate**: расчёт тормозного ответа.

### I/O
- Вход: сигналы/токи.
- Выход: ингибирующая активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NNewRenshowCell
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewRenshowCell
    In-->>N: signals
    N-->>In: inhibitory output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewRenshowCell]
    n --> inh[Inhibitory output]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewRenshowCell
Name = NewRenshow1
```

---

## NNewRenshowCell — new Renshaw cell (EN)

Updated Renshaw cell variant producing inhibitory output.

```mermaid
classDiagram
    NNeuron <|-- NNewRenshowCell
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewRenshowCell
    In-->>N: signals
    N-->>In: inhibitory output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewRenshowCell]
    n --> inh[Inhibitory output]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
