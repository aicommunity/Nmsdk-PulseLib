## NCSynChannel — синаптический канал (classic)

**Класс**: `NCSynChannel` — канал передачи для классических синапсов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCSynChannel", ...)`.  
**Storage**: `ClassName = "NCSynChannel"`.

### Lifecycle
- **ADefault**: параметры передачи.
- **ABuild**: подключение pre/post.
- **AReset**: сброс буферов.
- **ACalculate**: передача сигнала/веса.

### I/O
- Вход: pre-сигнал.
- Выход: переданный/взвешенный сигнал.

```mermaid
classDiagram
    UComponent <|-- NCSynChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NCSynChannel
    participant Post as Post
    Pre-->>Ch: signal
    Ch->>Ch: ACalculate()
    Ch-->>Post: weighted signal
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> ch[NCSynChannel]
    ch --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCSynChannel
Name = CSynCh1
```

---

## NCSynChannel — classic synaptic channel (EN)

Transfers weighted signal from pre to post.

```mermaid
classDiagram
    UComponent <|-- NCSynChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NCSynChannel
    participant Post as Post
    Pre-->>Ch: signal
    Ch-->>Post: weighted
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> ch[NCSynChannel]
    ch --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
