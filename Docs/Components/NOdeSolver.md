## NOdeSolver — численный решатель ОДУ

**Класс**: `NOdeSolver` — интегратор ОДУ для нейронных моделей.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NOdeSolver", ...)`.  
**Storage-инстансы**: `ClassName = "NOdeSolver"`; параметры шага интегрирования, метода, точности.

```mermaid
classDiagram
    UComponent <|-- NOdeSolver
    class NOdeSolver {
        +dt : double
        +method : enum
    }
```

### Входы/выходы
- Вход: функции/состояния для интегрирования (обычно мембранные уравнения).
- Выход: обновлённые состояния.

```mermaid
flowchart LR
    state[State variables] --> solver[NOdeSolver]
    solver --> state2[Updated state]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Sol as NOdeSolver
    participant Mem as NPulseMembraneIzhikevich
    Mem-->>Sol: dV/dt inputs
    Sol-->>Mem: integrated V
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NOdeSolver — ODE solver

Numerical integrator for membrane/state equations; configured with step and method.
