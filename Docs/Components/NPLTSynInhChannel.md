## NPLTSynInhChannel — LT синаптический тормозной канал

**Класс**: `NPLTSynInhChannel` — синаптический ингибирующий канал с LT.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynInhChannel", ...)`.  
**Storage**: `ClassName = "NPLTSynInhChannel"`.

### Lifecycle
- **ADefault**: параметры LT + синапс + торможение.
- **ABuild**: подключение pre/post.
- **AReset**: сброс LT.
- **ACalculate**: передача ингибирующего сигнала + LT.

### I/O
- Вход: импульс от pre.
- Выход: LT-модифицированный ингибирующий синаптический сигнал.

```mermaid
classDiagram
    NPLTSynChannel <|-- NPLTSynInhChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTSynInhChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT inhibitory synaptic
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPLTSynInhChannel]
    inh --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTSynInhChannel
Name = LTSynInh1
```

---

## NPLTSynInhChannel — LT inhibitory synaptic channel (EN)

Inhibitory synaptic channel with long-term plasticity.

```mermaid
classDiagram
    NPLTSynChannel <|-- NPLTSynInhChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTSynInhChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT inhibitory synaptic
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> inh[NPLTSynInhChannel]
    inh --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
