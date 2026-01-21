## NPLTSynExcChannel — LT синаптический возбуждающий канал

**Класс**: `NPLTSynExcChannel` — синаптический возбуждающий канал с LT.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynExcChannel", ...)`.  
**Storage**: `ClassName = "NPLTSynExcChannel"`.

### Lifecycle
- **ADefault**: параметры LT + синапс + возбуждение.
- **ABuild**: подключение pre/post.
- **AReset**: сброс LT.
- **ACalculate**: передача возбуждающего сигнала + LT.

### I/O
- Вход: импульс от pre.
- Выход: LT-модифицированный возбуждающий синаптический сигнал.

```mermaid
classDiagram
    NPLTSynChannel <|-- NPLTSynExcChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTSynExcChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT excitatory synaptic
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPLTSynExcChannel]
    exc --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTSynExcChannel
Name = LTSynExc1
```

---

## NPLTSynExcChannel — LT excitatory synaptic channel (EN)

Excitatory synaptic channel with long-term plasticity.

```mermaid
classDiagram
    NPLTSynChannel <|-- NPLTSynExcChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTSynExcChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT excitatory synaptic
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPLTSynExcChannel]
    exc --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
