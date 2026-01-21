## NPInhChannelBio2 — тормозной канал (био2)

**Класс**: `NPInhChannelBio2` — ингибирующий канал с расширенными био-параметрами (вариант 2).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPInhChannelBio2", ...)`.  
**Storage**: `ClassName = "NPInhChannelBio2"`.

### Lifecycle
- **ADefault**: био2-параметры.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача с био2-моделью.

### I/O
- Вход: импульс от pre.
- Выход: био2-ингибирующий сигнал.

```mermaid
classDiagram
    NPInhChannel <|-- NPInhChannelBio
    NPInhChannelBio <|-- NPInhChannelBio2
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPInhChannelBio2
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio2 inhibitory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPInhChannelBio2]
    inh --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPInhChannelBio2
Name = InhBio2_1
```

---

## NPInhChannelBio2 — inhibitory bio2 channel (EN)

Inhibitory channel with extended bio parameters (variant 2).

```mermaid
classDiagram
    NPInhChannel <|-- NPInhChannelBio
    NPInhChannelBio <|-- NPInhChannelBio2
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPInhChannelBio2
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio2 inhibitory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPInhChannelBio2]
    inh --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
