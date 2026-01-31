# NDataset — датасет

## RU

### Назначение

**Класс**: `NDataset` — компонент для управления датасетами (выборками данных) для обучения и тестирования.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NDataset", ...)`.  
**Storage-инстансы**: `ClassName = "NDataset"` в `Bin/Configs/*/Model_*.xml`.

`NDataset` реализует компонент для управления датасетами, который создает генераторы импульсов для каждого признака данных и управляет их работой на основе матрицы данных (`MatrixData`) и классов (`MatrixClasses`). Компонент может загружать данные из файла и генерировать паттерны импульсов для обучения и тестирования нейронных сетей.

**Использование:** Управление датасетами, генерация паттернов для обучения

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NDataset
    NDataset *-- NPulseGeneratorTransit : Generators
    class NDataset {
        +PulseGeneratorClassName : string
        +NumGenerators : int
        +NumFeatures : int
        +NumSamples : int
        +MatrixData : MDMatrix~double~
        +MatrixClasses : MDMatrix~int~
        +ReloadDataset : bool
        +MatrixDelay : MDMatrix~double~
        +Iteration : int
        +Tay : float
        +Delay : double
        +SpikesFrequency : double
        +NumClasses : int
        +FileName : string
        +StateGeneration : int
        +TimeGeneration : double
        +OperatingTime : double
        +ResetDelay : bool
        -Generators : vector~NPulseGeneratorTransit*~
        +New() NDataset*
        +ADefault() bool
        +ABuild() bool
        +ACalculate() bool
    }
```

**Иерархия наследования:**
- `UNet` — базовая сеть Rdk Framework
- `NDataset` — датасет

**Связи:**
- Создает и управляет генераторами импульсов (`NPulseGeneratorTransit`)

### Свойства

#### Параметры (ptPubParameter)

- **`NumFeatures`** (int) — количество признаков (измерений) в датасете. Значение по умолчанию: зависит от реализации

- **`NumSamples`** (int) — количество образцов (примеров) в датасете. Значение по умолчанию: зависит от реализации

- **`MatrixData`** (MDMatrix<double>) — матрица данных. Строки — образцы, столбцы — признаки. Значение по умолчанию: зависит от реализации

- **`MatrixClasses`** (MDMatrix<int>) — матрица классов. Содержит метки классов для каждого образца. Значение по умолчанию: зависит от реализации

- **`ReloadDataset`** (bool) — флаг перезагрузки датасета. Значение по умолчанию: зависит от реализации

- **`MatrixDelay`** (MDMatrix<double>) — матрица временных сдвигов запусков генераторов. Вычисляется на основе `MatrixData` и `Tay`. Значение по умолчанию: зависит от реализации

- **`Iteration`** (int) — текущая итерация (индекс образца). Значение по умолчанию: зависит от реализации

- **`Tay`** (float) — параметр для расчета задержек. Значение по умолчанию: зависит от реализации

- **`NumClasses`** (int) — количество классов в датасете. Вычисляется автоматически из `MatrixClasses`. Значение по умолчанию: зависит от реализации

- **`FileName`** (string) — путь к файлу с данными. Значение по умолчанию: зависит от реализации

**Остальные параметры аналогичны `NPattern`.**

### Методы

- **`ABuild()`** → `bool` — строит структуру датасета:
  1. Загружает данные из файла (если указан `FileName`)
  2. Вычисляет `NumClasses` из `MatrixClasses`
  3. Вычисляет `MatrixDelay` на основе `MatrixData` и `Tay`
  4. Создает генераторы импульсов для каждого признака

- **`ACalculate()`** → `bool` — выполняет расчет датасета:
  1. Управляет генераторами для текущего образца (`Iteration`)
  2. Устанавливает задержки генераторов на основе `MatrixDelay`
  3. Обновляет состояние генерации

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**.

### См. также

- [`NPattern`](NPattern.md) — паттерн данных
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md) — генератор с транзитным сигналом
- [`NClassifier`](NClassifier.md) — классификатор
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NDataset` — component for managing datasets (data samples) for training and testing.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NDataset", ...)`.  
**Instances**: `ClassName = "NDataset"` in `Bin/Configs/*/Model_*.xml`.

`NDataset` implements dataset management component that creates pulse generators for each data feature and manages their operation based on data matrix (`MatrixData`) and classes (`MatrixClasses`). Component can load data from file and generate pulse patterns for training and testing neural networks.

**Usage:** Dataset management, pattern generation for training

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NDataset
    NDataset *-- NPulseGeneratorTransit : Generators
    class NDataset {
        +PulseGeneratorClassName : string
        +NumGenerators : int
        +NumFeatures : int
        +NumSamples : int
        +MatrixData : MDMatrix~double~
        +MatrixClasses : MDMatrix~int~
        +ReloadDataset : bool
        +MatrixDelay : MDMatrix~double~
        +Iteration : int
        +Tay : float
        +Delay : double
        +SpikesFrequency : double
        +NumClasses : int
        +FileName : string
        +StateGeneration : int
        +TimeGeneration : double
        +OperatingTime : double
        +ResetDelay : bool
        -Generators : vector~NPulseGeneratorTransit*~
        +New() NDataset*
        +ADefault() bool
        +ABuild() bool
        +ACalculate() bool
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Dataset as NDataset
    participant Generators as NPulseGeneratorTransit
    participant FileSystem as File System
    participant OutputTarget
    
    Storage->>Dataset: New()
    Storage->>Dataset: Default()
    Storage->>Dataset: SetFileName("data.ini")
    Storage->>Dataset: SetMatrixData(...)
    Storage->>Dataset: SetMatrixClasses(...)
    Storage->>Dataset: Build()
    Dataset->>Dataset: ABuild()
    alt FileName specified
        Dataset->>FileSystem: Load data from file
        FileSystem-->>Dataset: MatrixData, MatrixClasses
    end
    Dataset->>Dataset: Calculate NumClasses from MatrixClasses
    Dataset->>Dataset: Calculate MatrixDelay from MatrixData and Tay
    loop For each feature (i = 0..NumFeatures-1)
        Dataset->>Generators: CreateComponent("Generator" + i)
        Dataset->>Generators: SetDelay(MatrixDelay(Iteration, i))
    end
    Dataset-->>Storage: Ready = true
    
    loop Each simulation step
        Storage->>Dataset: Calculate()
        Dataset->>Dataset: ACalculate()
        Dataset->>Dataset: Update Iteration
        loop For each generator
            Dataset->>Generators: SetDelay(MatrixDelay(Iteration, i))
            Dataset->>Generators: ACalculate()
            Generators-->>Dataset: Output (pulses)
        end
        Dataset-->>OutputTarget: Output (pattern for current sample)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> LoadData{FileName specified?}
    LoadData -->|Yes| LoadFromFile: Load from file
    LoadData -->|No| CalcNumClasses: Calculate NumClasses
    LoadFromFile --> CalcNumClasses
    CalcNumClasses --> CalcMatrixDelay: Calculate MatrixDelay
    CalcMatrixDelay --> CreateGenerators: Create generators
    CreateGenerators --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> UpdateIteration: Update Iteration
    UpdateIteration --> UpdateGenerators: Update generator delays
    UpdateGenerators --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> ResetIteration: Iteration = 0
    ResetIteration --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> UpdateIteration[Update Iteration]
    UpdateIteration --> LoopGenerators[Loop through generators]
    LoopGenerators --> SetDelay[Set generator delay from MatrixDelay]
    SetDelay --> CalcGenerator[Calculate generator]
    CalcGenerator --> CheckMore{More generators?}
    CheckMore -->|Yes| LoopGenerators
    CheckMore -->|No| AggregateOutputs[Aggregate generator outputs]
    AggregateOutputs --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end
    
    subgraph NDataset["NDataset"]
        Dataset[Dataset]
        Generators[NPulseGeneratorTransit<br/>Generators<br/>NumFeatures]
    end
    
    subgraph External["External Components"]
        DataFile[Data File]
        MatrixData[Matrix Data]
        MatrixClasses[Matrix Classes]
        OutputTarget[Output Target]
    end
    
    BaseNet -->|inherits| NDataset
    NDataset -->|creates| Generators
    DataFile -->|FileName| NDataset
    MatrixData -->|MatrixData| NDataset
    MatrixClasses -->|MatrixClasses| NDataset
    NDataset -->|calculates| MatrixDelay
    MatrixDelay -->|delays| Generators
    Generators -->|pulse patterns| NDataset
    NDataset -->|Output| OutputTarget
```

### Properties

- `PulseGeneratorClassName` — имя класса генератора импульсов
- `NumGenerators` — количество генераторов (соответствует NumFeatures)
- `NumFeatures` — количество признаков (измерений) в датасете
- `NumSamples` — количество образцов (примеров) в датасете
- `MatrixData` — матрица данных (строки — образцы, столбцы — признаки)
- `MatrixClasses` — матрица классов (метки классов для каждого образца)
- `ReloadDataset` — флаг перезагрузки датасета
- `MatrixDelay` — матрица временных сдвигов запусков генераторов
- `Iteration` — текущая итерация (индекс образца)
- `Tay` — параметр для расчета задержек
- `Delay` — базовая задержка
- `SpikesFrequency` — частота спайков
- `NumClasses` — количество классов в датасете
- `FileName` — путь к файлу с данными
- `StateGeneration` — состояние генерации
- `TimeGeneration` — время генерации
- `OperatingTime` — рабочее время
- `ResetDelay` — флаг сброса задержки

### Methods

- `SetPulseGeneratorClassName(value)` — установка имени класса генератора
- `SetNumGenerators(value)` — установка количества генераторов
- `SetNumFeatures(value)` — установка количества признаков
- `SetMatrixData(value)` — установка матрицы данных
- `SetMatrixClasses(value)` — установка матрицы классов
- `ADefault()` — установка параметров по умолчанию
- `ABuild()` — сборка структуры датасета (загрузка данных, создание генераторов)
- `ACalculate()` — выполнение шага генерации паттерна для текущего образца

### Usage in configurations

`NDataset` is used for dataset management:

- **Dataset management**: `Bin/Configs/*/Model_*.xml` (where dataset management is required)
- **Pattern generation**: generating pulse patterns for training and testing
- **Data loading**: loading data from files for neural network training

**Features:**
- Automatic structure building: creates generators for each feature
- Data loading: supports loading data from files
- Class management: automatically calculates number of classes
- Delay calculation: calculates generator delays from data matrix
- Iteration management: manages current sample iteration

**Typical parameter values:**
- **PulseGeneratorClassName**: "NPulseGeneratorTransit" (transit pulse generator)
- **NumFeatures**: 5-50 (number of features in dataset)
- **NumSamples**: 10-1000 (number of samples in dataset)
- **Tay**: 0.1-1.0 (parameter for delay calculation)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**.

### See Also

- [`NPattern`](NPattern.md) — data pattern
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md) — generator with transit signal
- [`NClassifier`](NClassifier.md) — classifier
- [Architecture.md](../Architecture.md) — library architecture
