## NPChannel — импульсный канал (Nmsdk-PulseLib)

**Класс**: `NPChannel` — базовый канал передачи импульсов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPChannel", ...)`.  
**Storage**: `ClassName = "NPChannel"`.

### Lifecycle
- **ADefault**: параметры передачи/задержки.
- **ABuild**: подключение pre/post.
- **AReset**: сброс буферов.
- **ACalculate**: передача импульса/сигнала.

### I/O
- Вход: импульс/сигнал от pre.
- Выход: переданный/обработанный сигнал к post.

```mermaid
classDiagram
    UComponent <|-- NPChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as PreNeuron
    participant Ch as NPChannel
    participant Post as PostNeuron
    Pre-->>Ch: spike/signal
    Ch->>Ch: ACalculate()
    Ch-->>Post: output
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> ch[NPChannel]
    ch --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPChannel
Name = PCh1
```

---

## NPChannel — pulse channel (EN)

Base pulse/spike transmission channel.

```mermaid
classDiagram
    UComponent <|-- NPChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as PreNeuron
    participant Ch as NPChannel
    participant Post as PostNeuron
    Pre-->>Ch: spike
    Ch-->>Post: output
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> ch[NPChannel]
    ch --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
