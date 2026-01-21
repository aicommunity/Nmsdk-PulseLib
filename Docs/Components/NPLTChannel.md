## NPLTChannel — LT-канал

**Класс**: `NPLTChannel` — канал с долговременной пластичностью (LT).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTChannel", ...)`.  
**Storage**: `ClassName = "NPLTChannel"`.

### Lifecycle
- **ADefault**: параметры LT.
- **ABuild**: подключение pre/post.
- **AReset**: сброс LT-состояния.
- **ACalculate**: передача + обновление LT.

### I/O
- Вход: импульс от pre.
- Выход: модифицированный сигнал (LT-эффект).

```mermaid
classDiagram
    NPChannel <|-- NPLTChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch->>Ch: LT update
    Ch-->>Post: modulated signal
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> lt[NPLTChannel]
    lt --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTChannel
Name = LTCh1
```

---

## NPLTChannel — long-term channel (EN)

Channel with long-term plasticity modulation.

```mermaid
classDiagram
    NPChannel <|-- NPLTChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: modulated signal
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> lt[NPLTChannel]
    lt --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
