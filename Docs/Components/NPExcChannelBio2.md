## NPExcChannelBio2 — возбуждающий канал (био2)

**Класс**: `NPExcChannelBio2` — возбуждающий канал с расширенными био-параметрами (вариант 2).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPExcChannelBio2", ...)`.  
**Storage**: `ClassName = "NPExcChannelBio2"`.

### Lifecycle
- **ADefault**: био2-параметры.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача с био2-моделью.

### I/O
- Вход: импульс от pre.
- Выход: био2-возбуждающий сигнал.

```mermaid
classDiagram
    NPExcChannel <|-- NPExcChannelBio
    NPExcChannelBio <|-- NPExcChannelBio2
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPExcChannelBio2
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio2 excitatory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPExcChannelBio2]
    exc --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPExcChannelBio2
Name = ExcBio2_1
```

---

## NPExcChannelBio2 — excitatory bio2 channel (EN)

Excitatory channel with extended bio parameters (variant 2).

```mermaid
classDiagram
    NPExcChannel <|-- NPExcChannelBio
    NPExcChannelBio <|-- NPExcChannelBio2
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPExcChannelBio2
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio2 excitatory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPExcChannelBio2]
    exc --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
