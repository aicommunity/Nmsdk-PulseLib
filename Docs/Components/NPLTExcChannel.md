## NPLTExcChannel — LT возбуждающий канал

**Класс**: `NPLTExcChannel` — возбуждающий канал с LT-пластичностью.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTExcChannel", ...)`.  
**Storage**: `ClassName = "NPLTExcChannel"`.

### Lifecycle
- **ADefault**: параметры LT + возбуждение.
- **ABuild**: подключение pre/post.
- **AReset**: сброс LT-состояния.
- **ACalculate**: передача возбуждающего сигнала + LT-обновление.

### I/O
- Вход: импульс от pre.
- Выход: LT-модифицированный возбуждающий сигнал.

```mermaid
classDiagram
    NPLTChannel <|-- NPLTExcChannel
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTExcChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch->>Ch: LT + excitatory
    Ch-->>Post: LT excitatory
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPLTExcChannel]
    exc --> post[Post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPLTExcChannel
Name = LTExc1
```

---

## NPLTExcChannel — LT excitatory channel (EN)

Excitatory channel with long-term plasticity.

```mermaid
classDiagram
    NPLTChannel <|-- NPLTExcChannel
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Ch as NPLTExcChannel
    participant Post as Post
    Pre-->>Ch: spike
    Ch-->>Post: LT excitatory
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre] --> exc[NPLTExcChannel]
    exc --> post[Post]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
