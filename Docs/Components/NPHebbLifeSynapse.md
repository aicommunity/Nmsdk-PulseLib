## NPHebbLifeSynapse — Hebb Life синапс

**Класс**: `NPHebbLifeSynapse` — синапс с Hebb-пластичностью и расширением Life.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPHebbLifeSynapse", ...)`.  
**Storage**: `ClassName = "NPHebbLifeSynapse"`.

### Lifecycle
- **ADefault**: параметры Hebb + Life.
- **ABuild**: подключение pre/post.
- **AReset**: сброс веса/метрик.
- **ACalculate**: передача + Hebb-обновление + Life-метрики.

### I/O
- Вход: спайки pre/post.
- Выход: взвешенный импульс + обновлённый вес.

```mermaid
classDiagram
    NPSynapse <|-- NPHebbSynapse
    NPHebbSynapse <|-- NPHebbLifeSynapse
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Syn as NPHebbLifeSynapse
    participant Post as Post
    Pre-->>Syn: spike
    Post-->>Syn: spike
    Syn->>Syn: Hebb + Life update
    Syn-->>Post: weighted spike
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre spike] --> syn[NPHebbLifeSynapse]
    post[Post spike] --> syn
    syn --> out[Weighted spike]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPHebbLifeSynapse
Name = HebbLifeSyn1
```

---

## NPHebbLifeSynapse — Hebb Life synapse (EN)

Synapse with Hebbian plasticity and Life metrics.

```mermaid
classDiagram
    NPSynapse <|-- NPHebbSynapse
    NPHebbSynapse <|-- NPHebbLifeSynapse
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Pre as Pre
    participant Syn as NPHebbLifeSynapse
    participant Post as Post
    Pre-->>Syn: spike
    Post-->>Syn: spike
    Syn-->>Post: weighted spike
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    pre[Pre spike] --> syn[NPHebbLifeSynapse]
    post[Post spike] --> syn
    syn --> out[Weighted spike]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
