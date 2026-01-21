## NPExcChannelBio — возбуждающий канал (био)

**Класс**: `NPExcChannelBio` — возбуждающий канал с биологическими параметрами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPExcChannelBio", ...)`.  
**Storage**: `ClassName = "NPExcChannelBio"`.

### Lifecycle
- **ADefault**: био-параметры передачи.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача с учётом био-модели.

### I/O
- Вход: импульс от pre.
- Выход: био-моделированный возбуждающий сигнал.

```mermaid
classDiagram
    NPExcChannel <|-- NPExcChannelBio
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPExcChannelBio
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio excitatory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPExcChannelBio]
    exc --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPExcChannelBio
Name = ExcBio1
```

---

## NPExcChannelBio — excitatory bio channel (EN)

Excitatory channel with biological parameters.

```mermaid
classDiagram
    NPExcChannel <|-- NPExcChannelBio
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPExcChannelBio
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio excitatory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPExcChannelBio]
    exc --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
