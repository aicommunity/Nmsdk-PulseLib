# NClassifier — классификатор

## RU

### Назначение

**Класс**: `NClassifier` — базовый классификатор для распознавания паттернов импульсов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NClassifier", ...)`.  
**Storage-инстансы**: `ClassName = "NClassifier"` в `Bin/Configs/*/Model_*.xml`.

`NClassifier` реализует классификатор, который создает группу обученных нейронов для распознавания заданных паттернов импульсов. Классификатор автоматически строит структуру нейронов, обучает их на заданных паттернах и классифицирует входные паттерны.

**Использование:** Распознавание паттернов импульсов, классификация сигналов

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NClassifier
    NClassifier *-- NPulseGeneratorTransit : Generators
    NClassifier *-- NNeuronTrainer : Trainers
    NClassifier *-- NPulseNeuron : Neurons
    class NClassifier {
        +StructureBuildMode : int
        +CalculateMode : int
        +PulseGeneratorClassName : string
        +NeuronTrainerClassName : string
        +NeuronClassName : string
        +SynapseClassName : string
        +IsNeedToTrain : bool
        +Delay : double
        +SpikesFrequency : double
        +NumInputDendrite : int
        +MaxDendriteLength : int
        +TrainingPatterns : MDMatrix~double~
        +InputPattern : MDMatrix~double~
        +LTZThreshold : double
        +NumClasses : int
        +SizeTrainingSet : int
        +DataFromFile : bool
        +New() NClassifier*
        +ADefault() bool
        +ABuild() bool
        +ACalculate() bool
    }
```

**Иерархия наследования:**
- `UNet` — базовая сеть Rdk Framework
- `NClassifier` — классификатор

**Связи:**
- Создает и управляет генераторами импульсов (`NPulseGeneratorTransit`)
- Создает и управляет тренерами нейронов (`NNeuronTrainer`)
- Создает и управляет нейронами (`NPulseNeuron`)

**Внутренняя структура:**
- **Generators** (`NPulseGeneratorTransit`) — генераторы импульсов для каждого класса
- **Trainers** (`NNeuronTrainer`) — тренеры для обучения нейронов
- **Neurons** (`NPulseNeuron`) — нейроны для каждого класса

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant Storage as UStorage
    participant Classifier as NClassifier
    participant Generator as NPulseGeneratorTransit
    participant Trainer as NNeuronTrainer
    participant Neuron as NPulseNeuron
    participant InputSource as Источник входных паттернов
    
    Storage->>Classifier: New()
    Storage->>Classifier: Default()
    Storage->>Classifier: SetTrainingPatterns(...)
    Storage->>Classifier: SetNumClasses(...)
    Storage->>Classifier: SetIsNeedToTrain(true)
    Storage->>Classifier: Build()
    Classifier->>Classifier: ABuild()
    Classifier->>Classifier: BuildStructure()
    loop Для каждого класса
        Classifier->>Generator: CreateComponent("Generator" + classIndex)
        Classifier->>Neuron: CreateComponent("Neuron" + classIndex)
        Classifier->>Trainer: CreateComponent("Trainer" + classIndex)
        Classifier->>Classifier: CreateLinks()
    end
    Classifier-->>Storage: Ready = true
    
    alt Режим обучения (IsNeedToTrain = true)
        loop Каждый шаг симуляции
            Storage->>Classifier: Calculate()
            Classifier->>Classifier: ACalculate()
            Classifier->>Generator: ACalculate() (для каждого класса)
            Generator-->>Trainer: Генерация обучающих паттернов
            Classifier->>Trainer: ACalculate()
            Trainer->>Neuron: Обучение нейрона
            Neuron-->>Trainer: Обновленные веса
        end
    else Режим классификации (IsNeedToTrain = false)
        loop Каждый шаг симуляции
            InputSource->>Classifier: InputPattern (входной паттерн)
            Storage->>Classifier: Calculate()
            Classifier->>Classifier: ACalculate()
            Classifier->>Neuron: ACalculate() (для каждого класса)
            Neuron-->>Classifier: Активность нейрона
            Classifier->>Classifier: Определение класса с максимальной активностью
            Classifier-->>Storage: Output (класс)
        end
    end
```

**Жизненный цикл:**
1. **Инициализация**: Установка параметров классификатора
2. **Сборка структуры**: Создание генераторов, нейронов и тренеров для каждого класса
3. **Режим обучения**: Обучение нейронов на обучающих паттернах
4. **Режим классификации**: Классификация входных паттернов на основе активности нейронов

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreatingGenerators: Создание генераторов
    CreatingGenerators --> CreatingNeurons: Создание нейронов
    CreatingNeurons --> CreatingTrainers: Создание тренеров
    CreatingTrainers --> Linking: Создание связей
    Linking --> Built: Структура построена
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CheckMode{IsNeedToTrain?}
    CheckMode -->|Да| Training: Режим обучения
    CheckMode -->|Нет| Classification: Режим классификации
    Training --> GeneratePatterns: Генерация обучающих паттернов
    GeneratePatterns --> TrainNeurons: Обучение нейронов
    TrainNeurons --> Ready: Шаг завершен
    Classification --> ProcessInput: Обработка InputPattern
    ProcessInput --> CalcNeurons: Расчет активности нейронов
    CalcNeurons --> FindMax: Поиск класса с максимальной активностью
    FindMax --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Building** — выполняется сборка структуры
- **CreatingGenerators** — создание генераторов импульсов
- **CreatingNeurons** — создание нейронов для классов
- **CreatingTrainers** — создание тренеров нейронов
- **Linking** — создание связей между компонентами
- **Built** — структура классификатора построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет классификатора
- **Training** — режим обучения
- **GeneratePatterns** — генерация обучающих паттернов
- **TrainNeurons** — обучение нейронов
- **Classification** — режим классификации
- **ProcessInput** — обработка входного паттерна
- **CalcNeurons** — расчет активности нейронов
- **FindMax** — поиск класса с максимальной активностью
- **Resetting** — выполняется сброс состояний

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CheckMode{IsNeedToTrain?}
    CheckMode -->|Да| TrainingMode[Режим обучения]
    CheckMode -->|Нет| ClassificationMode[Режим классификации]
    
    TrainingMode --> LoopClasses[Цикл по классам]
    LoopClasses --> GeneratePattern[Генерация обучающего паттерна]
    GeneratePattern --> TrainNeuron[Обучение нейрона класса]
    TrainNeuron --> CheckMoreClasses{Есть еще классы?}
    CheckMoreClasses -->|Да| LoopClasses
    CheckMoreClasses -->|Нет| End([End])
    
    ClassificationMode --> ProcessInput[Обработка InputPattern]
    ProcessInput --> LoopNeurons[Цикл по нейронам классов]
    LoopNeurons --> CalcActivity[Расчет активности нейрона]
    CalcActivity --> CheckMoreNeurons{Есть еще нейроны?}
    CheckMoreNeurons -->|Да| LoopNeurons
    CheckMoreNeurons -->|Нет| FindMaxClass[Поиск класса с максимальной активностью]
    FindMaxClass --> SetOutput[Output = класс с максимальной активностью]
    SetOutput --> End
```

**Алгоритм расчета:**
1. Проверка режима работы: обучение или классификация
2. **Режим обучения**:
   - Генерация обучающих паттернов для каждого класса
   - Обучение нейронов на соответствующих паттернах
3. **Режим классификации**:
   - Обработка входного паттерна
   - Расчет активности нейронов для каждого класса
   - Определение класса с максимальной активностью

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end
    
    subgraph NClassifier["NClassifier"]
        Generators["NPulseGeneratorTransit<br/>Генераторы для классов"]
        Trainers["NNeuronTrainer<br/>Тренеры для классов"]
        Neurons["NPulseNeuron<br/>Нейроны для классов"]
    end
    
    subgraph External["Внешние компоненты"]
        InputSource[Источник входных паттернов]
        OutputTarget[Целевой компонент]
    end
    
    BaseNet -->|наследуется| NClassifier
    NClassifier -->|создает| Generators
    NClassifier -->|создает| Trainers
    NClassifier -->|создает| Neurons
    InputSource -->|InputPattern| NClassifier
    Generators -->|обучающие паттерны| Trainers
    Trainers -->|обучение| Neurons
    Neurons -->|активность| NClassifier
    NClassifier -->|Output| OutputTarget
```

**Зависимости:**
- **Базовый класс**: `UNet`
- **Внутренние компоненты**: генераторы импульсов (`NPulseGeneratorTransit`), тренеры нейронов (`NNeuronTrainer`), нейроны (`NPulseNeuron`)
- **Внешние компоненты**: источник входных паттернов (источник `InputPattern`), целевой компонент (получатель `Output`)

### Свойства

#### Параметры (ptPubParameter)

- **`StructureBuildMode`** (int) — режим сборки структуры:
  - 0 — не пересобирать
  - 1 — пересобрать структуру
  - 2 — пересобрать структуру с обучением
  Значение по умолчанию: зависит от реализации

- **`CalculateMode`** (int) — режим расчета:
  - 0 — обычный
  - 1 — обучение
  - 2 — классификация
  Значение по умолчанию: зависит от реализации

- **`PulseGeneratorClassName`** (string) — имя класса генератора импульсов. Значение по умолчанию: зависит от реализации

- **`NeuronTrainerClassName`** (string) — имя класса тренера нейронов. Значение по умолчанию: зависит от реализации

- **`NeuronClassName`** (string) — имя класса нейронов. Значение по умолчанию: зависит от реализации

- **`SynapseClassName`** (string) — имя класса синапсов. Значение по умолчанию: зависит от реализации

- **`IsNeedToTrain`** (bool) — флаг необходимости обучения. Значение по умолчанию: зависит от реализации

- **`Delay`** (double) — задержка начала обучения (сек). Значение по умолчанию: зависит от реализации

- **`SpikesFrequency`** (double) — частота спайков генераторов (Гц). Значение по умолчанию: зависит от реализации

- **`NumInputDendrite`** (int) — количество входных дендритов. Значение по умолчанию: зависит от реализации

- **`MaxDendriteLength`** (int) — максимальная длина дендрита. Значение по умолчанию: зависит от реализации

- **`TrainingPatterns`** (MDMatrix<double>) — матрица обучающих паттернов. Каждая строка — паттерн для одного класса. Значение по умолчанию: зависит от реализации

- **`InputPattern`** (MDMatrix<double>) — входной паттерн для классификации. Значение по умолчанию: зависит от реализации

- **`LTZThreshold`** (double) — порог LT-зоны. Значение по умолчанию: зависит от реализации

- **`NumClasses`** (int) — количество классов для классификации. Значение по умолчанию: зависит от реализации

- **`SizeTrainingSet`** (int) — размер обучающего набора. Значение по умолчанию: зависит от реализации

- **`DataFromFile`** (bool) — флаг чтения данных из файла. Значение по умолчанию: зависит от реализации

### Методы

- **`ABuild()`** → `bool` — строит структуру классификатора:
  1. Создает генераторы импульсов для каждого класса
  2. Создает нейроны для каждого класса
  3. Создает тренеры для обучения нейронов
  4. Настраивает связи между компонентами

- **`ACalculate()`** → `bool` — выполняет расчет классификатора:
  1. Если `IsNeedToTrain = true`, выполняет обучение
  2. Если `IsNeedToTrain = false`, выполняет классификацию входного паттерна
  3. Определяет класс входного паттерна на основе активности нейронов

### Примеры использования

#### Пример 1: Создание классификатора в коде C++

```cpp
// Создание классификатора
auto classifier = storage->CreateComponent<NClassifier>();
classifier->SetName("Classifier");

// Инициализация
classifier->Default();

// Настройка параметров
classifier->NumClasses = 3;
classifier->NumInputDendrite = 10;
classifier->MaxDendriteLength = 5;
classifier->IsNeedToTrain = true;
classifier->SpikesFrequency = 10.0;

// Загрузка обучающих паттернов
MDMatrix<double> patterns(3, 10);  // 3 класса, 10 признаков
// ... заполнение patterns ...
classifier->TrainingPatterns = patterns;

// Сборка (создает структуру и обучает)
classifier->Build();
```

## Источники

См. [Literature-References.md](../Literature-References.md): **1**, **6**, **8**, **9**, **10**.

### См. также

- [`NSpikeClassifier`](NSpikeClassifier.md) — классификатор по спайкам
- [`NPCAClassifier`](NPCAClassifier.md) — классификатор PCA
- [`NNeuronTrainer`](NNeuronTrainer.md) — тренер нейронов
- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NClassifier` — base classifier for pattern recognition.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NClassifier", ...)`.  
**Instances**: `ClassName = "NClassifier"` in `Bin/Configs/*/Model_*.xml`.

`NClassifier` implements classifier that creates group of trained neurons for recognizing specified pulse patterns. Classifier automatically builds neuron structure, trains them on specified patterns, and classifies input patterns.

**Usage:** Pattern recognition, signal classification

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NClassifier
    NClassifier *-- NPulseGeneratorTransit : Generators
    NClassifier *-- NNeuronTrainer : Trainers
    NClassifier *-- NPulseNeuron : Neurons
    class NClassifier {
        +StructureBuildMode : int
        +CalculateMode : int
        +TrainingPatterns : MDMatrix~double~
        +InputPattern : MDMatrix~double~
        +NumClasses : int
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Classifier as NClassifier
    participant Generator as NPulseGeneratorTransit
    participant Trainer as NNeuronTrainer
    participant Neuron as NPulseNeuron
    
    Storage->>Classifier: New() + Default()
    Storage->>Classifier: Build()
    Classifier->>Generator: CreateComponent()
    Classifier->>Neuron: CreateComponent()
    Classifier->>Trainer: CreateComponent()
    loop Each step
        Storage->>Classifier: Calculate()
        Classifier->>Generator: ACalculate()
        Classifier->>Trainer: ACalculate()
        Classifier->>Neuron: ACalculate()
        Classifier-->>Storage: Output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreatingComponents: Create components
    CreatingComponents --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CheckMode{Need to train?}
    CheckMode -->|Yes| Training: Training mode
    CheckMode -->|No| Classification: Classification mode
    Training --> TrainNeurons: Train neurons
    TrainNeurons --> Ready: Step completed
    Classification --> CalcNeurons: Calculate neuron activity
    CalcNeurons --> FindMax: Find max class
    FindMax --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CheckMode{Need to train?}
    CheckMode -->|Yes| TrainingMode[Training mode]
    CheckMode -->|No| ClassificationMode[Classification mode]
    TrainingMode --> GeneratePatterns[Generate training patterns]
    GeneratePatterns --> TrainNeurons[Train neurons]
    ClassificationMode --> ProcessInput[Process input pattern]
    ProcessInput --> CalcNeurons[Calculate neuron activity]
    CalcNeurons --> FindMax[Find max class]
    TrainNeurons --> End([End])
    FindMax --> End
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end
    
    subgraph NClassifier["NClassifier"]
        Classifier[Classifier]
        Generators["NPulseGeneratorTransit<br/>Generators"]
        Trainers["NNeuronTrainer<br/>Trainers"]
        Neurons["NPulseNeuron<br/>Neurons"]
    end
    
    subgraph External["External Components"]
        InputPattern[Input Pattern]
        TrainingPatterns[Training Patterns]
        OutputTarget[Output Target]
    end
    
    BaseNet -->|inherits| NClassifier
    NClassifier -->|creates| Generators
    NClassifier -->|creates| Trainers
    NClassifier -->|creates| Neurons
    InputPattern -->|InputPattern| NClassifier
    TrainingPatterns -->|TrainingPatterns| NClassifier
    Generators -->|training signals| Trainers
    Trainers -->|trained neurons| Neurons
    Neurons -->|classification result| OutputTarget
```

### Properties

- `StructureBuildMode` — режим пересборки структуры
- `CalculateMode` — режим расчета
- `PulseGeneratorClassName` — имя класса генератора импульсов
- `NeuronTrainerClassName` — имя класса тренера нейронов
- `NeuronClassName` — имя класса нейрона
- `SynapseClassName` — имя класса синапса
- `IsNeedToTrain` — необходимость обучения
- `TrainingPatterns` — паттерны для обучения
- `InputPattern` — входной паттерн для классификации
- `LTZThreshold` — порог LT-зоны
- `NumClasses` — количество классов
- `SizeTrainingSet` — размер обучающего набора
- `DataFromFile` — загрузка данных из файла

### Methods

- `ADefault()` — установка параметров по умолчанию
- `ABuild()` — сборка структуры классификатора
- `ACalculate()` — выполнение шага классификации или обучения
- `BuildStructure()` — построение структуры нейронов и тренеров

### Usage in configurations

`NClassifier` is used for pattern recognition and classification:

- **Pattern recognition**: `Bin/Configs/*/Model_*.xml` (where pattern classification is required)
- **Neuron training**: experiments with training neurons on specific patterns
- **Classification**: real-time classification of input patterns

**Features:**
- Automatic structure building: creates neurons and trainers for each class
- Training mode: trains neurons on provided patterns
- Classification mode: classifies input patterns using trained neurons
- Flexible configuration: supports various neuron and synapse types

**Typical parameter values:**
- **NeuronClassName**: "NPulseNeuron" (spiking neuron)
- **NeuronTrainerClassName**: "NNeuronTrainer" (neuron trainer)
- **PulseGeneratorClassName**: "NPulseGeneratorTransit" (transit pulse generator)
- **SynapseClassName**: "NPulseSynapse" (pulse synapse)

### References

See [Literature-References.md](../Literature-References.md): **1**, **6**, **8**, **9**, **10**.

### See Also

- [`NSpikeClassifier`](NSpikeClassifier.md) — spike-based classifier
- [`NPCAClassifier`](NPCAClassifier.md) — PCA classifier
- [`NNeuronTrainer`](NNeuronTrainer.md) — neuron trainer
- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [Architecture.md](../Architecture.md) — library architecture
