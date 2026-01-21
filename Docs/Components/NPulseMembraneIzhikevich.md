## NPulseMembraneIzhikevich — мембрана (модель Ижикевича)

**Класс**: `NPulseMembraneIzhikevich` — мембранная модель для нейронов Ижикевича.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPulseMembraneIzhikevich", ...)`.  
**Storage-инстансы**: `ClassName = "NPulseMembraneIzhikevich"`; параметры a,b,c,d, timestep.

```mermaid
classDiagram
    NPulseMembrane <|-- NPulseMembraneIzhikevich
    class NPulseMembraneIzhikevich {
        +a : double
        +b : double
        +c : double
        +d : double
    }
```

### Входы/выходы
- Вход: ток/спайк воздействия.
- Выход: мембранный потенциал, признак спайка.

```mermaid
flowchart LR
    current[Input current] --> mem[NPulseMembraneIzhikevich]
    mem --> v[V membrane]
    mem --> spike[Spike flag]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Mem as NPulseMembraneIzhikevich
    participant Chan as NPulseChannelIaF
    Chan-->>Mem: current/spike
    Mem->>Mem: integrate & check threshold
    Mem-->>Chan: spike flag
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NPulseMembraneIzhikevich — membrane (Izhikevich)

Integrates input current per Izhikevich equations; outputs membrane potential and spike event.
