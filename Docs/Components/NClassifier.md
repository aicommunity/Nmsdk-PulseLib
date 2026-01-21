## NClassifier — классификатор (Nmsdk-PulseLib)

**Класс**: `NClassifier` — базовый классификатор по активности/признакам сети.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NClassifier", ...)`.  
**Storage**: `ClassName = "NClassifier"`.

### Lifecycle
- **ADefault**: инициализация параметров классификации.
- **ABuild**: подключение входных признаков.
- **AReset**: сброс состояния/статистик.
- **ACalculate**: вычисление класса/скор.

### I/O
- Вход: признаки/активность.
- Выход: метка класса/скор.

```mermaid
classDiagram
    UComponent <|-- NClassifier
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Feat as Features
    participant C as NClassifier
    Feat-->>C: features
    C->>C: ACalculate()
    C-->>Feat: class/score
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    feats[Features] --> cls[NClassifier]
    cls --> label[Class/Score]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NClassifier
Name = Classifier1
```

---

## NClassifier — classifier (EN)

Base classifier consuming features and producing class/score.

```mermaid
classDiagram
    UComponent <|-- NClassifier
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Feat as Features
    participant C as NClassifier
    Feat-->>C: features
    C-->>Feat: class/score
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    feats[Features] --> cls[NClassifier]
    cls --> label[Class/Score]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
