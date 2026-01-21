## NPInhChannel — тормозной канал

**Класс**: `NPInhChannel` — ингибирующий импульсный канал.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPInhChannel", ...)`.  
**Storage**: `ClassName = "NPInhChannel"`.

### Lifecycle
- **ADefault**: параметры передачи/масштаба.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача тормозного импульса.

### I/O
- Вход: импульс от pre.
- Выход: ингибирующий сигнал к post.

```mermaid
classDiagram
    NPChannel <|-- NPInhChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPInhChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: inhibitory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPInhChannel]
    inh --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPInhChannel
Name = InhCh1
```

---

## NPInhChannel — inhibitory channel (EN)

Inhibitory pulse channel.

```mermaid
classDiagram
    NPChannel <|-- NPInhChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPInhChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: inhibitory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPInhChannel]
    inh --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
