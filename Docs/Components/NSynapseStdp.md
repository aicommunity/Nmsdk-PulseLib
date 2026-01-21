## NSynapseStdp — синапс с STDP (Nmsdk-PulseLib)

**Класс**: `NSynapseStdp` — импульсный синапс с правилом Spike-Timing Dependent Plasticity.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynapseStdp", ...)`.  
**Storage-инстансы**: `ClassName = "NSynapseStdp"`; параметры: tau_pre/post, A_pre/post, w_min/max, задержка.

```mermaid
classDiagram
    NPSynapse <|-- NSynapseStdp
    class NSynapseStdp {
        +w : double
        +tauPre : double
        +tauPost : double
        +aPre : double
        +aPost : double
        +delay : double
    }
```

### Жизненный цикл
- **ADefault**: инициализация веса и временных констант.
- **ABuild**: подключение к пресинаптическому/постсинаптическому нейрону.
- **ACalculate**: обновление веса по временному различию спайков.

### Входы/выходы
- Вход: спайки pre/post.
- Выход: модифицированный вес w и переданный импульс.

```mermaid
sequenceDiagram
    participant Pre as PreNeuron
    participant Syn as NSynapseStdp
    participant Post as PostNeuron
    Pre-->>Syn: spike_pre
    Post-->>Syn: spike_post
    Syn->>Syn: update w (STDP rule)
    Syn-->>Post: weighted spike
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    pre[Pre spike] --> syn[NSynapseStdp]
    post[Post spike] --> syn
    syn --> weight[Updated weight]
    syn --> out[Spike to post]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

---

## NSynapseStdp — STDP synapse

Implements spike-timing dependent plasticity; inputs pre/post spikes, outputs weighted spike and updated weight.
