## NPInhChannelBio — тормозной канал (био)

**Класс**: `NPInhChannelBio` — ингибирующий канал с биологическими параметрами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPInhChannelBio", ...)`.  
**Storage**: `ClassName = "NPInhChannelBio"`.

### Lifecycle
- **ADefault**: био-параметры передачи.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача с учётом био-модели.

### I/O
- Вход: импульс от pre.
- Выход: био-моделированный ингибирующий сигнал.

```mermaid
classDiagram
    NPInhChannel <|-- NPInhChannelBio
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPInhChannelBio
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio inhibitory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPInhChannelBio]
    inh --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPInhChannelBio
Name = InhBio1
```

---

## NPInhChannelBio — inhibitory bio channel (EN)

Inhibitory channel with biological parameters.

```mermaid
classDiagram
    NPInhChannel <|-- NPInhChannelBio
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPInhChannelBio
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: bio inhibitory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPInhChannelBio]
    inh --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
