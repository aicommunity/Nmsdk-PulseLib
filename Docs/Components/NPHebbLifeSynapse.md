# NPHebbLifeSynapse — импульсный синапс Хебба с поддержкой жизнеобеспечения

## RU

### Назначение

**Класс**: `NPHebbLifeSynapse` — импульсный синапс с механизмом Хебба и интеграцией с моделью жизнеобеспечения нейрона.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPHebbLifeSynapse", ...)`.  
**Storage-инстансы**: `ClassName = "NPHebbLifeSynapse"` в `Bin/Configs/*/Model_*.xml`.

`NPHebbLifeSynapse` является расширением класса `NPulseHebbSynapse` с дополнительной интеграцией с моделью жизнеобеспечения нейрона (`NPulseLifeNeuron`). Наследуется от `NPulseHebbSynapse` и добавляет автоматическое подключение к системе жизнеобеспечения нейрона через метод `InstallLifeConnection()`.

**Использование:** Моделирование синаптической пластичности с учетом жизнеобеспечения нейрона, эксперименты с жизненными циклами нейронов

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseSynapseCommon <|-- NPulseSynapse
    NPulseSynapse <|-- NPulseHebbSynapse
    NPulseHebbSynapse <|-- NPulseHebbLifeSynapse
    NPulseHebbLifeSynapse *-- NPulseLifeNeuron : MainOwner
    NPulseHebbLifeSynapse *-- NNeuronLife : NeuronLife
    class NPulseHebbSynapse {
        +Min : double
        +Mout : double
        +Md : double
        +Kin : double
        +Kout : double
        +GdGain : double
        +GsGain : double
        +InputLTZoneFeedbackSignal : MDMatrix~double~
        +InputMotivation : MDMatrix~double~
        +Output1-Output6 : MDMatrix~double~
        +InstallHebbianConnection() bool
    }
    class NPulseHebbLifeSynapse {
        +New() NPulseHebbLifeSynapse*
        +InstallLifeConnection() bool
        +ADefault() bool
        +ABuild() bool
    }
```

**Иерархия наследования:**
- `NPulseSynapseCommon` — общий импульсный синапс
- `NPulseSynapse` — импульсный синапс с моделью медиатора
- `NPulseHebbSynapse` — импульсный синапс с механизмом Хебба
- `NPulseHebbLifeSynapse` — импульсный синапс Хебба с поддержкой жизнеобеспечения

**Ключевые особенности:**
- Автоматическое подключение к системе жизнеобеспечения нейрона
- Интеграция с `NPulseLifeNeuron` и `NNeuronLife`
- Использование сигналов жизнеобеспечения для модуляции механизма Хебба

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant PreNeuron as Пресинаптический нейрон
    participant Synapse as NPulseHebbLifeSynapse
    participant PostNeuron as NPulseLifeNeuron
    participant LTZone as LT-зона
    participant LifeSystem as NNeuronLife
    participant Channel as Канал
    
    PreNeuron->>Synapse: Входной сигнал (Input)
    PostNeuron->>LTZone: Генерация спайка
    LTZone->>Synapse: InputLTZoneFeedbackSignal
    PostNeuron->>LifeSystem: Сигналы жизнеобеспечения
    LifeSystem->>Synapse: Сигналы жизнеобеспечения (через InstallLifeConnection)
    Synapse->>Synapse: ACalculate2()
    Synapse->>Synapse: NPulseHebbSynapse::ACalculate2()
    Synapse->>Synapse: Обновление механизма Хебба
    Note over Synapse: Учет сигналов жизнеобеспечения
    Synapse-->>Channel: Output (модифицированный ток)
```

**Жизненный цикл:**
1. **Инициализация**: Установка параметров по умолчанию
2. **Сборка**: Вызов `NPulseHebbSynapse::ABuild()`
3. **Подключение**: Автоматическое подключение к LT-зоне и системе жизнеобеспечения
4. **Расчет**: Обновление компонентов механизма Хебба с учетом сигналов жизнеобеспечения
5. **Сброс**: Обнуление состояний

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Built: Build()
    Built --> ConnectingHebb: InstallHebbianConnection()
    ConnectingHebb --> ConnectingLife: InstallLifeConnection()
    ConnectingLife --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CalcBase: Расчет базового синапса
    CalcBase --> UpdateHebb: Обновление механизма Хебба
    UpdateHebb --> ApplyLife: Применение сигналов жизнеобеспечения
    ApplyLife --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Build]) --> CallBaseBuild[NPulseHebbSynapse::ABuild]
    CallBaseBuild --> CheckOwner{MainOwner - NPulseLifeNeuron?}
    CheckOwner -->|Да| GetLifeSystem[Получение NNeuronLife]
    CheckOwner -->|Нет| End([End])
    GetLifeSystem --> CheckLifeSystem{NeuronLife существует?}
    CheckLifeSystem -->|Нет| End
    CheckLifeSystem -->|Да| InstallLife[InstallLifeConnection]
    InstallLife --> CreateLink{Создание связи}
    CreateLink -->|Успешно| End
    CreateLink -->|Ошибка| End
```

**Алгоритм подключения к жизнеобеспечению:**
1. Проверка, что владелец синапса является `NPulseLifeNeuron`
2. Получение системы жизнеобеспечения (`NNeuronLife`)
3. Создание связи между системой жизнеобеспечения и синапсом
4. Использование сигналов жизнеобеспечения в расчетах механизма Хебба

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseHebbSynapse["NPulseHebbSynapse Base"]
        BaseHebbSynapse[NPulseHebbSynapse]
    end
    
    subgraph NPulseHebbLifeSynapse["NPulseHebbLifeSynapse"]
        LifeIntegration[Интеграция с жизнеобеспечением]
        HebbMechanism[Механизм Хебба]
    end
    
    subgraph External["Внешние компоненты"]
        PreNeuron[Пресинаптический нейрон]
        PostNeuron[NPulseLifeNeuron]
        LTZone[LT-зона]
        LifeSystem[NNeuronLife]
        Channel[Канал]
    end
    
    BaseHebbSynapse -->|наследуется| NPulseHebbLifeSynapse
    NPulseHebbLifeSynapse -->|вычисляет| LifeIntegration
    NPulseHebbLifeSynapse -->|вычисляет| HebbMechanism
    PreNeuron -->|Input| NPulseHebbLifeSynapse
    LTZone -->|InputLTZoneFeedbackSignal| NPulseHebbLifeSynapse
    LifeSystem -->|сигналы жизнеобеспечения| NPulseHebbLifeSynapse
    NPulseHebbLifeSynapse -->|Output| Channel
```

### Свойства

`NPHebbLifeSynapse` использует все свойства базового класса `NPulseHebbSynapse`:
- Параметры механизма Хебба: `Min`, `Mout`, `Md`, `Kin`, `Kout`, `GdGain`, `GsGain`
- Мотивационные параметры: `ActiveMs`, `PassiveMs`, `Kmot`
- Входы: `Input`, `InputLTZoneFeedbackSignal`, `InputMotivation`
- Выходы: `Output`, `Output1`-`Output6`
- Состояния: `Win`, `Wout`, `Gd`, `Gs`, `GsSum`, `G`

### Методы

#### Публичные методы

- **`New()`** → `NPulseHebbLifeSynapse*` — создает новый экземпляр класса.

#### Защищенные методы жизненного цикла

- **`ADefault()`** → `bool` — инициализирует параметры по умолчанию. Вызывает `NPulseHebbSynapse::ADefault()`.

- **`ABuild()`** → `bool` — строит структуру синапса. Вызывает `NPulseHebbSynapse::ABuild()`, затем автоматически подключается к системе жизнеобеспечения через `InstallLifeConnection()`.

#### Защищенные методы подключения

- **`InstallLifeConnection()`** → `bool` — подключает синапс к системе жизнеобеспечения нейрона (`NNeuronLife`). Проверяет, что владелец синапса является `NPulseLifeNeuron`, получает систему жизнеобеспечения и создает необходимые связи. Возвращает `false` только при ошибке установки связи.

### Примеры использования

#### Пример 1: Создание синапса в коде C++

```cpp
// Создание синапса Хебба с поддержкой жизнеобеспечения
auto synapse = storage->CreateComponent<NPulseHebbLifeSynapse>();
synapse->SetName("HebbLifeSynapse");

// Инициализация
synapse->Default();

// Настройка параметров механизма Хебба
synapse->Min = 10.0;
synapse->Mout = 10.0;
synapse->Md = 0.001;
synapse->Kin = 100.0;
synapse->Kout = 100.0;

// Сборка (автоматически подключается к жизнеобеспечению)
synapse->Build();

// Использование
for (int step = 0; step < 10000; step++) {
    synapse->Calculate();
    double output = synapse->Output(0, 0);
    double g = synapse->Output2(0, 0);
    
    if (step % 1000 == 0) {
        std::cout << "Step " << step << ": Output = " << output 
                  << ", G = " << g << std::endl;
    }
}
```

#### Пример 2: Конфигурация XML

```xml
<Synapse1 Class="NPHebbLifeSynapse">
    <Parameters>
        <Type>1</Type>
        <PulseAmplitude>1.0</PulseAmplitude>
        <Resistance>1.0e10</Resistance>
        <Weight>1.0</Weight>
        <SecretionTC>0.001</SecretionTC>
        <DissociationTC>0.01</DissociationTC>
        <Min>10</Min>
        <Mout>10</Mout>
        <Md>0.001</Md>
        <Kin>100</Kin>
        <Kout>100</Kout>
        <GdGain>1</GdGain>
        <GsGain>10</GsGain>
    </Parameters>
</Synapse1>
```

### Использование в конфигурациях

`NPHebbLifeSynapse` используется в экспериментах с жизнеобеспечением нейронов:

- Моделирование синаптической пластичности с учетом жизнеобеспечения
- Эксперименты с жизненными циклами нейронов
- Изучение влияния жизнеобеспечения на обучение

**Особенности:**
- Автоматически подключается к системе жизнеобеспечения нейрона
- Интегрируется с `NPulseLifeNeuron` и `NNeuronLife`
- Использует сигналы жизнеобеспечения для модуляции механизма Хебба

### См. также

- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — импульсный синапс с механизмом Хебба (базовый класс)
- [`NPHebbSynapse`](NPHebbSynapse.md) — алиас для NPulseHebbSynapse
- [`NPulseLifeNeuron`](NPulseLifeNeuron.md) — импульсный нейрон с жизнеобеспечением
- [`NNeuronLife`](NNeuronLife.md) — система жизнеобеспечения нейрона
- [`NPulseSynapse`](NPulseSynapse.md) — импульсный синапс с моделью медиатора
- [Architecture.md](../Architecture.md) — архитектура библиотеки
- [Scientific-Background.md](../Scientific-Background.md) — научный фон (правило Хебба, жизнеобеспечение нейронов)

---

## EN

### Purpose

**Class**: `NPHebbLifeSynapse` — spiking synapse with Hebbian mechanism and neuron life support integration.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPHebbLifeSynapse", ...)`.  
**Instances**: `ClassName = "NPHebbLifeSynapse"` in `Bin/Configs/*/Model_*.xml`.

`NPHebbLifeSynapse` is an extension of `NPulseHebbSynapse` class with additional integration with neuron life support model (`NPulseLifeNeuron`). Inherits from `NPulseHebbSynapse` and adds automatic connection to neuron life support system through `InstallLifeConnection()` method.

**Usage:** Modeling synaptic plasticity with neuron life support, experiments with neuron life cycles

### UML Class Diagram

```mermaid
classDiagram
    NPulseHebbSynapse <|-- NPulseHebbLifeSynapse
    NPulseHebbLifeSynapse *-- NPulseLifeNeuron : MainOwner
    NPulseHebbLifeSynapse *-- NNeuronLife : NeuronLife
    class NPulseHebbLifeSynapse {
        +InstallLifeConnection() bool
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant PreNeuron
    participant Synapse as NPulseHebbLifeSynapse
    participant PostNeuron as NPulseLifeNeuron
    participant LifeSystem as NNeuronLife
    
    PreNeuron->>Synapse: Input
    PostNeuron->>LifeSystem: Life signals
    LifeSystem->>Synapse: Life support signals
    Synapse->>Synapse: Calculate Hebbian mechanism
    Synapse-->>Synapse: Output
```

### See Also

- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — spiking synapse with Hebbian mechanism (base class)
- [`NPHebbSynapse`](NPHebbSynapse.md) — alias for NPulseHebbSynapse
- [`NPulseLifeNeuron`](NPulseLifeNeuron.md) — spiking neuron with life support
- [`NNeuronLife`](NNeuronLife.md) — neuron life support system
- [`NPulseSynapse`](NPulseSynapse.md) — spiking synapse with neurotransmitter model
- [Architecture.md](../Architecture.md) — library architecture
- [Scientific-Background.md](../Scientific-Background.md) — scientific background (Hebb's rule, neuron life support)
