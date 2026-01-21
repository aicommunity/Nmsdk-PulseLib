## NPLTInhChannel — LT тормозной канал

**Класс**: `NPLTInhChannel` — ингибирующий канал с LT-пластичностью.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTInhChannel", ...)`.  
**Storage**: `ClassName = "NPLTInhChannel"`.

### Lifecycle
- **ADefault**: параметры LT + торможение.
- **ABuild**: подключение pre/post.
- **AReset**: сброс LT-состояния.
- **ACalculate**: передача ингибирующего сигнала + LT-обновление.

### I/O
- Вход: импульс от pre.
- Выход: LT-модифицированный ингибирующий сигнал.

```mermaid
classDiagram
    NPLTChannel <|-- NPLTInhChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTInhChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch->>Ch: LT + inhibitory
    Ch-->>Post: LT inhibitory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPLTInhChannel]
    inh --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTInhChannel
Name = LTInh1
```

---

## NPLTInhChannel — LT inhibitory channel (EN)

Inhibitory channel with long-term plasticity.

```mermaid
classDiagram
    NPLTChannel <|-- NPLTInhChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTInhChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT inhibitory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPLTInhChannel]
    inh --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
