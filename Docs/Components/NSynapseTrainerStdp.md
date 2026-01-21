## NSynapseTrainerStdp — тренер STDP (Nmsdk-PulseLib)

**Класс**: `NSynapseTrainerStdp` (и варианты TD/WD/Triplet/Probabilistic и др.) — обучает веса синапсов по правилам STDP.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynapseTrainerStdp", ...)` и специализированные варианты.  
**Storage-инстансы**: `ClassName = "NSynapseTrainerStdp"` с параметрами коэффициентов обучения.

```mermaid
classDiagram
    NSynapseTrainer <|-- NSynapseTrainerStdp
    class NSynapseTrainerStdp {
        +aPlus : double
        +aMinus : double
        +tauPlus : double
        +tauMinus : double
    }
```

### Жизненный цикл
- **ADefault**: коэффициенты STDP.
- **ABuild**: связывание с целевыми синапсами.
- **ACalculate**: шаг обучения по журналу спайков pre/post.

### Входы/выходы
- Вход: события спайков pre/post (может брать из связанных синапсов).
- Выход: обновлённые веса в целевых синапсах.

```mermaid
sequenceDiagram
    participant Trainer as NSynapseTrainerStdp
    participant Syn as NSynapseStdp
    Syn-->>Trainer: spike log
    Trainer->>Syn: update weights
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    spikes[Pre/Post spikes] --> trainer[NSynapseTrainerStdp]
    trainer --> syns[Target synapses weights]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

---

## NSynapseTrainerStdp — STDP trainer

Processes spike timings and updates target synapse weights according to STDP learning rule.
