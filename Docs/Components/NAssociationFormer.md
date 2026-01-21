## NAssociationFormer — формирование ассоциаций (Nmsdk-PulseLib)

**Класс**: `NAssociationFormer` — создает/обновляет ассоциативные связи на основе активности.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAssociationFormer", ...)`.  
**Storage**: `ClassName = "NAssociationFormer"`.

### Lifecycle
- **ADefault**: параметры формирования ассоциаций.
- **ABuild**: связывание входных паттернов/активности.
- **AReset**: сброс ассоциаций.
- **ACalculate**: обновление/вывод ассоциативных результатов.

### I/O
- Вход: паттерны/активность (векторы/спайки).
- Выход: ассоциативный отклик/связи.

```mermaid
classDiagram
    UComponent <|-- NAssociationFormer
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant A as InputA
    participant B as InputB
    participant F as NAssociationFormer
    A-->>F: pattern A
    B-->>F: pattern B
    F->>F: ACalculate()
    F-->>A: association result
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    a[Pattern A] --> f[NAssociationFormer]
    b[Pattern B] --> f
    f --> assoc[Association/Link]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NAssociationFormer
Name = Assoc1
```

---

## NAssociationFormer — association former (EN)

Forms associations between input patterns/activities and outputs association signals.

```mermaid
classDiagram
    UComponent <|-- NAssociationFormer
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant A as InputA
    participant B as InputB
    participant F as NAssociationFormer
    A-->>F: pattern A
    B-->>F: pattern B
    F-->>A: association result
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    a[Pattern A] --> f[NAssociationFormer]
    b[Pattern B] --> f
    f --> assoc[Association]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
