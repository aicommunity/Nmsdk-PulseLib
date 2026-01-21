## NPulseChannelIaF — канал импульсов (Integrate-and-Fire)

**Класс**: `NPulseChannelIaF` — передаёт импульсы с учётом модели IaF.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPulseChannelIaF", ...)`.  
**Storage-инстансы**: `ClassName = "NPulseChannelIaF"`; параметры задержек/масштабов.

```mermaid
classDiagram
    NPulseChannel <|-- NPulseChannelIaF
    class NPulseChannelIaF {
        +delay : double
        +scale : double
    }
```

### Входы/выходы
- Вход: спайки/токи от пресинаптических нейронов.
- Выход: скорректированные импульсы к постнейрону.

```mermaid
flowchart LR
    pre[Pre spikes] --> ch[NPulseChannelIaF]
    ch --> post[Post neuron input]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Pre as PreNeuron
    participant Ch as NPulseChannelIaF
    participant Post as PostNeuron
    Pre-->>Ch: spike
    Ch->>Ch: apply delay/scale
    Ch-->>Post: spike_out
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NPulseChannelIaF — spike channel (IaF)

Routes spikes with delay/scale for integrate-and-fire style processing.
