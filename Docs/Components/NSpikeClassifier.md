## NSpikeClassifier — классификатор по спайкам

**Класс**: `NSpikeClassifier` — определяет класс по спайковым паттернам.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSpikeClassifier", ...)`.  
**Storage-инстансы**: `ClassName = "NSpikeClassifier"`; параметры окон подсчёта, карт классов.

```mermaid
classDiagram
    NClassifier <|-- NSpikeClassifier
    class NSpikeClassifier {
        +window : int
        +classes : map
    }
```

### Входы/выходы
- Вход: активность/спайки слоёв/групп.
- Выход: метка класса/скор.

```mermaid
flowchart LR
    spikes[Spike activity] --> cls[NSpikeClassifier]
    cls --> label[Class/score]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Net as NNet
    participant Cls as NSpikeClassifier
    Net-->>Cls: spike vectors
    Cls->>Cls: accumulate & score
    Cls-->>Net: class result
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NSpikeClassifier — spike-based classifier

Consumes spike activity and outputs class labels/scores using configured windows and mapping.
