## NPLTSynChannel — LT синаптический канал

**Класс**: `NPLTSynChannel` — синаптический канал с LT-пластичностью.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynChannel", ...)`.  
**Storage**: `ClassName = "NPLTSynChannel"`.

### Lifecycle
- **ADefault**: параметры LT + синапс.
- **ABuild**: подключение pre/post.
- **AReset**: сброс LT-состояния.
- **ACalculate**: передача + LT-обновление.

### I/O
- Вход: импульс от pre.
- Выход: LT-модифицированный синаптический сигнал.

```mermaid
classDiagram
    NPLTChannel <|-- NPLTSynChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTSynChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch->>Ch: LT update
    Ch-->>Post: LT synaptic
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> syn[NPLTSynChannel]
    syn --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTSynChannel
Name = LTSyn1
```

---

## NPLTSynChannel — LT synaptic channel (EN)

Synaptic channel with long-term plasticity.

```mermaid
classDiagram
    NPLTChannel <|-- NPLTSynChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTSynChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT synaptic
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> syn[NPLTSynChannel]
    syn --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
