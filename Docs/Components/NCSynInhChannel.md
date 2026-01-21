## NCSynInhChannel — тормозный синаптический канал (classic)

**Класс**: `NCSynInhChannel` — ингибирующий канал для классических синапсов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCSynInhChannel", ...)`.  
**Storage**: `ClassName = "NCSynInhChannel"`.

### Lifecycle
- **ADefault**: параметры передачи/масштаба.
- **ABuild**: подключение pre/post.
- **AReset**: сброс.
- **ACalculate**: передача тормозного сигнала.

### I/O
- Вход: pre-сигнал.
- Выход: ингибирующий сигнал.

```mermaid
classDiagram
    NCSynChannel <|-- NCSynInhChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NCSynInhChannel
    participant Post as Post
    Pre-->>Ch: signal
    Ch-->>Post: inhibitory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> inh[NCSynInhChannel]
    inh --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCSynInhChannel
Name = CSynInh1
```

---

## NCSynInhChannel — inhibitory channel (EN)

Inhibitory classic synaptic channel.

```mermaid
classDiagram
    NCSynChannel <|-- NCSynInhChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NCSynInhChannel
    participant Post as Post
    Pre-->>Ch: signal
    Ch-->>Post: inhibitory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> inh[NCSynInhChannel]
    inh --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
