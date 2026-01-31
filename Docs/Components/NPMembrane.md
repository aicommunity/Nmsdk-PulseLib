# NPMembrane — импульсная мембрана (алиас)

## RU

### Назначение

**Класс**: `NPMembrane` — алиас для класса `NPulseMembrane`.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPMembrane", ...)`.  
**Storage-инстансы**: `ClassName = "NPMembrane"` в `Bin/Configs/*/Model_*.xml`.

`NPMembrane` является алиасом (синонимом) для класса `NPulseMembrane`. При создании компонента с `ClassName = "NPMembrane"` фактически создается экземпляр класса `NPulseMembrane` с параметрами по умолчанию.

`NPulseMembrane` реализует базовую импульсную мембрану с управлением возбуждающими и тормозными каналами и синапсами.

**Использование:** Упрощенное именование при конфигурации, обратная совместимость

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseMembraneCommon <|-- NPulseMembrane
    NPulseMembrane <|.. NPMembrane : alias
    class NPulseMembrane {
        +FeedbackGain : double
        +SynapseClassName : string
        +ExcChannelClassName : string
        +InhChannelClassName : string
    }
    class NPMembrane {
        +Type : string = "NPulseMembrane"
    }
```

**Иерархия наследования:**
- `NPulseMembraneCommon` — общая импульсная мембрана
- `NPulseMembrane` — базовая импульсная мембрана
- `NPMembrane` — алиас для `NPulseMembrane`

### Свойства

`NPMembrane` использует все свойства базового класса `NPulseMembrane` с параметрами по умолчанию.

### Методы

`NPMembrane` использует все методы базового класса `NPulseMembrane`.

### Примеры использования

#### Пример 1: Создание мембраны в коде C++

```cpp
// Создание импульсной мембраны через алиас
auto membrane = storage->CreateComponent("NPMembrane");
membrane->SetName("PMembrane");

// Инициализация (использует параметры по умолчанию)
membrane->Default();

// Использование
membrane->Build();
```

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseMembrane`](NPulseMembrane.md) — базовая импульсная мембрана (базовый класс)
- [`NPulseMembraneCommon`](NPulseMembraneCommon.md) — общая импульсная мембрана
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPMembrane` — alias for `NPulseMembrane` class.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPMembrane", ...)`.  
**Instances**: `ClassName = "NPMembrane"` in `Bin/Configs/*/Model_*.xml`.

`NPMembrane` is an alias (synonym) for the `NPulseMembrane` class. When creating a component with `ClassName = "NPMembrane"`, an instance of `NPulseMembrane` with default parameters is actually created.

**Usage:** Simplified naming in configurations, backward compatibility

### UML Class Diagram

```mermaid
classDiagram
    NPulseMembraneCommon <|-- NPulseMembrane
    NPulseMembrane <|.. NPMembrane : alias
    class NPulseMembrane {
        +FeedbackGain : double
        +SynapseClassName : string
        +ExcChannelClassName : string
        +InhChannelClassName : string
    }
    class NPMembrane {
        +Type : string = "NPulseMembrane"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Alias as NPMembrane
    participant Membrane as NPulseMembrane
    
    Storage->>Alias: CreateComponent("NPMembrane")
    Note over Alias,Membrane: NPMembrane creates NPulseMembrane instance
    Storage->>Membrane: New() (NPulseMembrane)
    Storage->>Membrane: Default()
    Storage->>Membrane: Build()
    loop Each step
        Storage->>Membrane: Calculate()
        Membrane->>Membrane: NPulseMembrane::ACalculate()
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> CreateAlias: CreateComponent("NPMembrane")
    CreateAlias --> CreateInstance: Create NPulseMembrane
    CreateInstance --> Defaulted: Default()
    Defaulted --> Built: Build()
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> Ready: NPulseMembrane::ACalculate()
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Create NPMembrane]) --> CreateInstance[Create NPulseMembrane]
    CreateInstance --> UseDefault[Use default parameters]
    UseDefault --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseMembrane["NPulseMembrane Base"]
        BaseMembrane[NPulseMembrane]
    end
    
    subgraph NPMembrane["NPMembrane Alias"]
        Alias[NPMembrane<br/>alias]
    end
    
    BaseMembrane -->|aliased as| Alias
```

### Properties

`NPMembrane` uses all properties of base class `NPulseMembrane` with default parameters.

### Methods

`NPMembrane` uses all methods of base class `NPulseMembrane`.

### Usage in configurations

`NPMembrane` is used as a simplified name for `NPulseMembrane`:

- **Simplified naming**: Used for simplified naming in configurations
- **Backward compatibility**: Maintains compatibility with old configurations
- **Default parameters**: Uses standard default parameters

**Features:**
- Alias: creates `NPulseMembrane` instance when used
- Simplified configuration: easier to use in XML configs
- Backward compatibility: supports legacy configurations

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseMembrane`](NPulseMembrane.md) — base spiking membrane (base class)
- [`NPulseMembraneCommon`](NPulseMembraneCommon.md) — common spiking membrane
- [Architecture.md](../Architecture.md) — library architecture
