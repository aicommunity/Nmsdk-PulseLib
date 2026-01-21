## NPExcChannel — возбуждающий канал

**Класс**: `NPExcChannel` — возбуждающий импульсный канал.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPExcChannel", ...)`.  
**Storage**: `ClassName = "NPExcChannel"`.

### Lifecycle
- **ADefault**: параметры усиления/передачи.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача возбуждающего импульса.

### I/O
- Вход: импульс от pre.
- Выход: возбудительный сигнал к post.

```mermaid
classDiagram
    NPChannel <|-- NPExcChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPExcChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: excitatory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPExcChannel]
    exc --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPExcChannel
Name = ExcCh1
```

---

## NPExcChannel — excitatory channel (EN)

Excitatory pulse channel.

```mermaid
classDiagram
    NPChannel <|-- NPExcChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPExcChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: excitatory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPExcChannel]
    exc --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
