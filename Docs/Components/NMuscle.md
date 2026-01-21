## NMuscle / NEyeMuscle — эффекторные компоненты

**Классы**: `NMuscle`, `NEyeMuscle` — эффекторные узлы, преобразующие активность сети в действие/движение.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NMuscle", ...)`, `"NEyeMuscle"`.  
**Storage-инстансы**: `ClassName = "NMuscle"` / `"NEyeMuscle"`; параметры усиления/масштабов.

```mermaid
classDiagram
    UComponent <|-- NMuscle
    UComponent <|-- NEyeMuscle
```

### Входы/выходы
- Вход: суммарная активность/спайки управляющих нейронов.
- Выход: управляющее воздействие (сила/угол/поворот).

```mermaid
flowchart LR
    spikes[Control spikes] --> musc[NMuscle]
    musc --> action[Actuation]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Net as NNet
    participant M as NMuscle
    Net-->>M: control signal
    M-->>Net: feedback (optional)
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NMuscle / NEyeMuscle — effectors

Effectors translating spike activity into actions (force/position); optional feedback to the network.
