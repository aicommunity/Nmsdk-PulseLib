## NPulseLTZoneIzhikevich — LT-зона (Ижикевич)

**Класс**: `NPulseLTZoneIzhikevich` — зона длительной пластичности/порогов для нейронов Ижикевича.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPulseLTZoneIzhikevich", ...)`.  
**Storage-инстансы**: `ClassName = "NPulseLTZoneIzhikevich"`; параметры порогов/усилений.

```mermaid
classDiagram
    NPulseLTZone <|-- NPulseLTZoneIzhikevich
    class NPulseLTZoneIzhikevich {
        +threshold : double
        +gain : double
    }
```

### Входы/выходы
- Вход: мембранные сигналы/активность.
- Выход: модифицированная активность (усиление/затухание), сигналы пластичности.

```mermaid
flowchart LR
    memActivity[Membrane activity] --> lt[NPulseLTZoneIzhikevich]
    lt --> mod[Modulated activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Mem as Membrane
    participant LT as NPulseLTZoneIzhikevich
    Mem-->>LT: activity
    LT-->>Mem: modulation/plasticity signals
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NPulseLTZoneIzhikevich — LT zone

Applies long-term modulation to membrane activity for Izhikevich neurons.
