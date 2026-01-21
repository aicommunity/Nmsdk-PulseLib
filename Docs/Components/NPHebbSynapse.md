## NPHebbSynapse — Hebb синапс

**Класс**: `NPHebbSynapse` — синапс с Hebb-пластичностью.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPHebbSynapse", ...)`.  
**Storage**: `ClassName = "NPHebbSynapse"`.

### Lifecycle
- **ADefault**: параметры Hebb.
- **ABuild**: подключение pre/post.
- **AReset**: сброс веса.
- **ACalculate**: передача + Hebb-обновление веса.

### I/O
- Вход: спайки pre/post.
- Выход: взвешенный импульс + обновлённый вес.

```mermaid
classDiagram
    NPSynapse <|-- NPHebbSynapse
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Syn as NPHebbSynapse
    participant Post as Post
    Pre-->>Syn: spike
    Post-->>Syn: spike
    Syn->>Syn: Hebb update
    Syn-->>Post: weighted spike
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre spike] --> syn[NPHebbSynapse]
    post[Post spike] --> syn
    syn --> out[Weighted spike]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPHebbSynapse
Name = HebbSyn1
```

---

## NPHebbSynapse — Hebbian synapse (EN)

Synapse with Hebbian plasticity updating weight.

```mermaid
classDiagram
    NPSynapse <|-- NPHebbSynapse
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Syn as NPHebbSynapse
    participant Post as Post
    Pre-->>Syn: spike
    Post-->>Syn: spike
    Syn-->>Post: weighted spike
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre spike] --> syn[NPHebbSynapse]
    post[Post spike] --> syn
    syn --> out[Weighted spike]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
