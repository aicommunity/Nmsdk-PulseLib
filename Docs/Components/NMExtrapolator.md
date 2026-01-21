## NMExtrapolator — экстраполятор (Nmsdk-PulseLib)

**Класс**: `NMExtrapolator` — экстраполирует сигнал/состояние вперёд во времени.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NMExtrapolator", ...)`.  
**Storage**: `ClassName = "NMExtrapolator"`.

### Lifecycle
- **ADefault**: параметры окна/метода.
- **ABuild**: подключение входа.
- **AReset**: сброс накопителей.
- **ACalculate**: вычисление прогноза/экстраполяции.

### I/O
- Вход: временной ряд/сигнал.
- Выход: прогноз/экстраполированное значение.

```mermaid
classDiagram
    UComponent <|-- NMExtrapolator
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Input
    participant E as NMExtrapolator
    In-->>E: signal
    E->>E: ACalculate()
    E-->>In: extrapolated
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signal] --> ex[NMExtrapolator]
    ex --> pred[Prediction]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NMExtrapolator
Name = Extrap1
```

---

## NMExtrapolator — extrapolator (EN)

Extrapolates input signal forward in time.

```mermaid
classDiagram
    UComponent <|-- NMExtrapolator
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Input
    participant E as NMExtrapolator
    In-->>E: signal
    E-->>In: extrapolated
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signal] --> ex[NMExtrapolator]
    ex --> pred[Prediction]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
