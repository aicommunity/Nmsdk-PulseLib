# NSynapseStdp — базовый STDP-синапс

**Каталог компонентов:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NSynapseStdp` — базовый импульсный синапс с поддержкой STDP-обучения (Spike-Timing Dependent Plasticity).
**Аббревиатура**: `STDP` — **S**pike-**T**iming **D**ependent **P**lasticity (пластичность, зависящая от времени спайков).
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynapseStdp", ...)`.
**Storage-инстансы**: `ClassName = "NSynapseStdp"` в `Bin/Configs/*/Model_*.xml`.

`NSynapseStdp` реализует базовый импульсный синапс с механизмом STDP-обучения. Наследуется от `NPulseSynapseCommon` и добавляет отслеживание активности пресинаптического и постсинаптического нейронов, расчет влияния STDP на вес синапса. В отличие от `NPulseSynapseStdp`, не включает модель динамики медиатора.

**Использование:** `Bin/Configs/SpikeSamples/STDP/STDP-Simple-01/`, `Bin/Configs/SpikeSamples/StructTrain/`; моделирование синаптической пластичности с STDP, изучение механизмов обучения

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NPulseSynapseCommon
    NPulseSynapseCommon <|-- NSynapseStdp
    NSynapseStdp *-- NPulseNeuronCommon : MainOwner
    NSynapseStdp *-- NLTZone : LTZone
    class NPulseSynapseCommon {
        +Type : double
        +PulseAmplitude : double
        +Resistance : double
        +Weight : double
        +Input : MDMatrix~double~
        +Output : MDMatrix~double~
    }
    class NSynapseStdp {
        +XModCoeff : double
        +YModCoeff : double
        +APlus : double
        +AMinus : double
        +XTau : double
        +YTau : double
        +XAvg : double
        +YAvg : double
        +XYDiff : double
        +PsActivityInput : MDMatrix~double~
        +StdpInfluence : MDMatrix~double~
        +SetXModCoeff(value) bool
        +SetYModCoeff(value) bool
        +SetAPlus(value) bool
        +SetAMinus(value) bool
        +SetXTau(value) bool
        +SetYTau(value) bool
        +New() NSynapseStdp*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate2() bool
    }
```

**Иерархия наследования:**
- `UNet` — базовая сеть Rdk Framework
- `NPulseSynapseCommon` — общий импульсный синапс
- `NSynapseStdp` — базовый STDP-синапс

**Ключевые свойства:**
- Параметры STDP: `XModCoeff`, `YModCoeff`, `APlus`, `AMinus`, `XTau`, `YTau`
- Состояния STDP: `XAvg` (средняя активность постсинаптического нейрона), `YAvg` (средняя активность пресинаптического нейрона), `XYDiff` (разница влияний)
- Входы/выходы: `PsActivityInput` (активность постсинаптического нейрона), `StdpInfluence` (влияние STDP на вес)

### UML-диаграмма последовательности

```mermaid
sequenceDiagram
    participant PreNeuron as Пресинаптический нейрон
    participant Synapse as NSynapseStdp
    participant PostNeuron as Постсинаптический нейрон
    participant LTZone as LT-зона
    participant Channel as Канал

    PreNeuron->>Synapse: Входной сигнал (Input)
    PostNeuron->>LTZone: Генерация спайка
    LTZone->>Synapse: PsActivityInput (или через MainOwner)
    Synapse->>Synapse: ACalculate2()
    Synapse->>Synapse: Проверка активности пре/пост нейронов
    Synapse->>Synapse: Обновление XAvg (постсинаптическая активность)
    Synapse->>Synapse: Обновление YAvg (пресинаптическая активность)
    Synapse->>Synapse: Расчет XYDiff = XAvg*APlus - YAvg*AMinus
    Synapse->>Synapse: Расчет StdpInfluence = 1 - XYDiff
    Synapse->>Synapse: Расчет Output с учетом StdpInfluence
    Synapse-->>Channel: Output (ток с учетом STDP)
```

**Жизненный цикл:**
1. **Инициализация**: Установка параметров STDP по умолчанию
2. **Отслеживание активности**: Определение активности пресинаптического и постсинаптического нейронов
3. **Обновление STDP**: Расчет средних активностей и их разницы
4. **Применение STDP**: Расчет выходного сигнала с учетом влияния STDP

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Built: Build()
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CheckPreActivity: Проверка пресинаптической активности
    CheckPreActivity --> CheckPostActivity: Проверка постсинаптической активности
    CheckPostActivity --> UpdateXAvg: Обновление XAvg
    UpdateXAvg --> UpdateYAvg: Обновление YAvg
    UpdateYAvg --> CalcXYDiff: Расчет XYDiff
    CalcXYDiff --> CalcStdpInfluence: Расчет StdpInfluence
    CalcStdpInfluence --> CalcOutput: Расчет Output
    CalcOutput --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: XAvg=0, YAvg=0, XYDiff=0
```

### UML-диаграмма активности

```mermaid
flowchart TD
    Start([Start Calculate]) --> CheckInput{Input подключен?}
    CheckInput -->|Нет| End([End])
    CheckInput -->|Да| CheckInputPulse{Input > 0?}
    CheckInputPulse -->|Да| SetPreActive[is_input_pulse_active = true]
    CheckInputPulse -->|Нет| SetPreInactive[is_input_pulse_active = false]
    SetPreActive --> CheckPostActivity
    SetPreInactive --> CheckPostActivity{Проверка постсинаптической активности}
    CheckPostActivity -->|PsActivityInput подключен| CheckPsInput{PsActivityInput > 0?}
    CheckPostActivity -->|!PsActivityInput| CheckLTZone{LTZone->Output > 0?}
    CheckPsInput -->|Да| SetPostActive[is_output_pulse_active = true]
    CheckPsInput -->|Нет| SetPostInactive[is_output_pulse_active = false]
    CheckLTZone -->|Да| SetPostActive
    CheckLTZone -->|Нет| SetPostInactive
    SetPostActive --> UpdateXAvg{is_output_pulse_active?}
    SetPostInactive --> UpdateXAvg
    UpdateXAvg -->|Да| XAvgSecretion[XAvg += (XModCoeff - XAvg) / (XTau * TimeStep)]
    UpdateXAvg -->|Нет| XAvgDissociation[XAvg -= XAvg / (XTau * TimeStep)]
    XAvgSecretion --> UpdateYAvg{is_input_pulse_active?}
    XAvgDissociation --> UpdateYAvg
    UpdateYAvg -->|Да| YAvgSecretion[YAvg += (YModCoeff - YAvg) / (YTau * TimeStep)]
    UpdateYAvg -->|Нет| YAvgDissociation[YAvg -= YAvg / (YTau * TimeStep)]
    YAvgSecretion --> CalcXYDiff
    YAvgDissociation --> CalcXYDiff[Расчет XYDiff]
    Note over CalcXYDiff: x_avg_res = is_output_pulse_active ? XAvg * APlus : 0
    Note over CalcXYDiff: y_avg_res = is_input_pulse_active ? YAvg * AMinus : 0
    Note over CalcXYDiff: XYDiff += (x_avg_res - y_avg_res) / TimeStep
    CalcXYDiff --> CalcStdpInfluence[StdpInfluence = 1 - XYDiff]
    CalcStdpInfluence --> ClampStdp{StdpInfluence < 0?}
    ClampStdp -->|Да| SetZero[StdpInfluence = 0]
    ClampStdp -->|Нет| CalcOutput
    SetZero --> CalcOutput[Расчет Output]
    Note over CalcOutput: Output = Input * Weight * StdpInfluence / Resistance
    CalcOutput --> End
```

**Алгоритм расчета STDP:**
1. Определение активности пресинаптического нейрона (`is_input_pulse_active`)
2. Определение активности постсинаптического нейрона (`is_output_pulse_active`) через `PsActivityInput` или `LTZone->Output`
3. Обновление средних активностей:
   - `XAvg` (постсинаптическая): увеличивается при активности постнейрона, уменьшается при отсутствии
   - `YAvg` (пресинаптическая): увеличивается при активности пренейрона, уменьшается при отсутствии
4. Расчет разницы влияний: `XYDiff += (XAvg*APlus - YAvg*AMinus) / TimeStep`
5. Расчет влияния STDP: `StdpInfluence = max(1 - XYDiff, 0)`
6. Расчет выходного сигнала: `Output = Input * Weight * StdpInfluence / Resistance`

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph NPulseSynapseCommon["NPulseSynapseCommon Base"]
        BaseSynapse[NPulseSynapseCommon]
    end

    subgraph NSynapseStdp["NSynapseStdp"]
        STDPModel[Модель STDP]
    end

    subgraph External["Внешние компоненты"]
        PreNeuron[Пресинаптический нейрон]
        PostNeuron[Постсинаптический нейрон]
        LTZone[LT-зона]
        Channel[Канал]
    end

    BaseSynapse -->|наследуется| NSynapseStdp
    NSynapseStdp -->|вычисляет| STDPModel
    PreNeuron -->|Input| NSynapseStdp
    PostNeuron -->|PsActivityInput| NSynapseStdp
    LTZone -->|Output| NSynapseStdp
    NSynapseStdp -->|Output| Channel
    NSynapseStdp -->|StdpInfluence| Channel
```

### Свойства

#### Параметры (ptPubParameter)

- **`XModCoeff`** (double) — коэффициент модуляции для постсинаптической активности. Определяет максимальное значение `XAvg`. Значение по умолчанию: 1.0

- **`YModCoeff`** (double) — коэффициент модуляции для пресинаптической активности. Определяет максимальное значение `YAvg`. Значение по умолчанию: 1.0

- **`APlus`** (double) — амплитуда усиления при постсинаптической активности. Определяет влияние постсинаптического спайка на увеличение веса. Значение по умолчанию: 1.0

- **`AMinus`** (double) — амплитуда ослабления при пресинаптической активности. Определяет влияние пресинаптического спайка на уменьшение веса. Значение по умолчанию: 1.0

- **`XTau`** (double) — постоянная времени для постсинаптической активности (в секундах). Определяет скорость изменения `XAvg`. Значение по умолчанию: 1e-2 (10 мс)

- **`YTau`** (double) — постоянная времени для пресинаптической активности (в секундах). Определяет скорость изменения `YAvg`. Значение по умолчанию: 1e-3 (1 мс)

**Наследуемые параметры от NPulseSynapseCommon:**
- `Type` (double) — тип синапса (<0 — тормозной, >0 — возбуждающий)
- `PulseAmplitude` (double) — амплитуда импульса
- `Resistance` (double) — сопротивление синапса
- `Weight` (double) — вес синапса
- `TrainerClassName` (string) — имя класса тренера синапса

#### Входные свойства (ptInput | ptPubState)

**Наследуемые от NPulseSynapseCommon:**
- **`Input`** (MDMatrix<double>) — входной сигнал от пресинаптического нейрона

- **`PsActivityInput`** (MDMatrix<double>) — входной сигнал активности постсинаптического нейрона. Если подключен, используется вместо `LTZone->Output` для определения постсинаптической активности.

#### Выходные свойства (ptOutput | ptPubState)

**Наследуемые от NPulseSynapseCommon:**
- **`Output`** (MDMatrix<double>) — выходной ток синапса. Рассчитывается как `Output = Input * Weight * StdpInfluence / Resistance`.

- **`StdpInfluence`** (MDMatrix<double>) — влияние STDP на вес синапса. Рассчитывается как `StdpInfluence = max(1 - XYDiff, 0)`. Используется для модификации выходного сигнала.

#### Состояния (ptPubState)

- **`XAvg`** (double) — средняя активность постсинаптического нейрона. Интегрируется согласно экспоненциальному закону с постоянной времени `XTau`. Начальное значение: 0.0

- **`YAvg`** (double) — средняя активность пресинаптического нейрона. Интегрируется согласно экспоненциальному закону с постоянной времени `YTau`. Начальное значение: 0.0

- **`XYDiff`** (double) — разница влияний STDP. Интегрируется как `XYDiff += (XAvg*APlus - YAvg*AMinus) / TimeStep`. Начальное значение: 0.0

### Методы

#### Публичные методы

- **`New()`** → `NSynapseStdp*` — создает новый экземпляр класса.

#### Защищенные методы жизненного цикла

- **`ADefault()`** → `bool` — инициализирует параметры по умолчанию. Устанавливает `XModCoeff=1.0`, `YModCoeff=1.0`, `APlus=1.0`, `AMinus=1.0`, `XTau=1e-2`, `YTau=1e-3`, вызывает `NPulseSynapseCommon::ADefault()`.

- **`ABuild()`** → `bool` — строит структуру синапса. Вызывает `NPulseSynapseCommon::ABuild()`.

- **`AReset()`** → `bool` — сбрасывает состояния синапса. Устанавливает `XAvg=0`, `YAvg=0`, `XYDiff=0`, обнуляет `StdpInfluence`, вызывает `NPulseSynapseCommon::AReset()`.

- **`ACalculate2()`** → `bool` — выполняет расчет синапса на одном шаге. Определяет активность нейронов, обновляет средние активности, рассчитывает STDP и выходной сигнал.

#### Методы установки параметров

- **`SetXModCoeff(const double &value)`** → `bool` — устанавливает коэффициент модуляции для постсинаптической активности.

- **`SetYModCoeff(const double &value)`** → `bool` — устанавливает коэффициент модуляции для пресинаптической активности.

- **`SetAPlus(const double &value)`** → `bool` — устанавливает амплитуду усиления. Проверяет, что значение > 0.

- **`SetAMinus(const double &value)`** → `bool` — устанавливает амплитуду ослабления. Проверяет, что значение > 0.

- **`SetXTau(const double &value)`** → `bool` — устанавливает постоянную времени для постсинаптической активности. Проверяет, что значение > 0.

- **`SetYTau(const double &value)`** → `bool` — устанавливает постоянную времени для пресинаптической активности. Проверяет, что значение > 0.

### Примеры использования

#### Пример 1: Создание STDP-синапса в коде C++

```cpp
// Создание базового STDP-синапса
auto synapse = storage->CreateComponent<NSynapseStdp>();
synapse->SetName("StdpSynapse");

// Инициализация
synapse->Default();

// Настройка параметров STDP
synapse->XModCoeff = 1.0;      // Коэффициент модуляции постсинаптической активности
synapse->YModCoeff = 1.0;      // Коэффициент модуляции пресинаптической активности
synapse->APlus = 0.01;         // Амплитуда усиления
synapse->AMinus = 0.012;       // Амплитуда ослабления
synapse->XTau = 0.02;          // Постоянная времени постсинаптической активности (20 мс)
synapse->YTau = 0.01;          // Постоянная времени пресинаптической активности (10 мс)

// Настройка базовых параметров
synapse->Resistance = 1.0e9;   // Сопротивление синапса
synapse->Type = 1.0;           // Возбуждающий синапс
synapse->Weight = 1.0;         // Начальный вес

// Сборка
synapse->Build();

// Использование
for (int step = 0; step < 10000; step++) {
    synapse->Calculate();
    double output = synapse->Output(0, 0);
    double stdpInfluence = synapse->StdpInfluence(0, 0);
    double xAvg = synapse->XAvg;
    double yAvg = synapse->YAvg;

    if (step % 1000 == 0) {
        std::cout << "Step " << step << ": Output = " << output
                  << ", STDP Influence = " << stdpInfluence
                  << ", XAvg = " << xAvg << ", YAvg = " << yAvg << std::endl;
    }
}
```

#### Пример 2: Конфигурация XML

```xml
<Synapse1 Class="NSynapseStdp">
    <Parameters>
        <Type>1</Type>
        <PulseAmplitude>1.0</PulseAmplitude>
        <Resistance>1.0e9</Resistance>
        <Weight>1.0</Weight>
        <XModCoeff>1.0</XModCoeff>
        <YModCoeff>1.0</YModCoeff>
        <APlus>0.01</APlus>
        <AMinus>0.012</AMinus>
        <XTau>0.02</XTau>
        <YTau>0.01</YTau>
    </Parameters>
</Synapse1>
```

### Использование в конфигурациях

`NSynapseStdp` используется в экспериментах по STDP-обучению:

- **SpikeSamples:** `Bin/Configs/SpikeSamples/STDP/STDP-Simple-01/`, `Bin/Configs/SpikeSamples/StructTrain/`
- Моделирование синаптической пластичности
- Изучение механизмов STDP
- Обучение нейросетей с помощью STDP

**Типичные значения параметров STDP:**

1. **Стандартный STDP**: APlus=0.01, AMinus=0.012, XTau=0.02, YTau=0.01
2. **Асимметричный STDP**: APlus=0.015, AMinus=0.01, XTau=0.03, YTau=0.01
3. **Симметричный STDP**: APlus=0.01, AMinus=0.01, XTau=0.02, YTau=0.02

## Источники

См. [Literature-References.md](../Literature-References.md): **[B]**.

### См. также

- [`NPulseSynapseCommon`](NPulseSynapseCommon.md) — общий импульсный синапс (базовый класс)
- [`NPulseSynapseStdp`](NPulseSynapseStdp.md) — импульсный STDP-синапс с моделью медиатора
- [`NPulseNeuronCommon`](NPulseNeuronCommon.md) — общий импульсный нейрон
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — общая импульсная LT-зона
- [Architecture.md](../Architecture.md) — архитектура библиотеки
- [Scientific-Background.md](../Scientific-Background.md) — научный фон (STDP, синаптическая пластичность)

---

## EN

### Purpose

**Class**: `NSynapseStdp` — base spiking synapse with STDP learning (Spike-Timing Dependent Plasticity).
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSynapseStdp", ...)`.
**Instances**: `ClassName = "NSynapseStdp"` in `Bin/Configs/*/Model_*.xml`.

`NSynapseStdp` implements a base spiking synapse with STDP learning mechanism. Inherits from `NPulseSynapseCommon` and adds tracking of presynaptic and postsynaptic neuron activity, calculation of STDP influence on synapse weight. Unlike `NPulseSynapseStdp`, it does not include neurotransmitter dynamics model.

**Usage:** Modeling synaptic plasticity with STDP, studying learning mechanisms

### UML Class Diagram

```mermaid
classDiagram
    NPulseSynapseCommon <|-- NSynapseStdp
    class NSynapseStdp {
        +XModCoeff : double
        +YModCoeff : double
        +APlus : double
        +AMinus : double
        +XTau : double
        +YTau : double
        +XAvg : double
        +YAvg : double
        +XYDiff : double
        +PsActivityInput : MDMatrix~double~
        +StdpInfluence : MDMatrix~double~
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant PreNeuron
    participant Synapse as NSynapseStdp
    participant PostNeuron
    participant LTZone

    PreNeuron->>Synapse: Input
    PostNeuron->>LTZone: Spike
    LTZone->>Synapse: PsActivityInput
    Synapse->>Synapse: Update XAvg, YAvg
    Synapse->>Synapse: Calculate STDP
    Synapse-->>Synapse: Output with STDP
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Built: Build()
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CheckPreActivity: Check presynaptic activity
    CheckPreActivity --> CheckPostActivity: Check postsynaptic activity
    CheckPostActivity --> UpdateXAvg: Update XAvg
    UpdateXAvg --> UpdateYAvg: Update YAvg
    UpdateYAvg --> CalcXYDiff: Calculate XYDiff
    CalcXYDiff --> CalcStdpInfluence: Calculate StdpInfluence
    CalcStdpInfluence --> CalcOutput: Calculate Output
    CalcOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready: XAvg=0, YAvg=0, XYDiff=0
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CheckInput{Input connected?}
    CheckInput -->|No| End([End])
    CheckInput -->|Yes| CheckInputPulse{Input > 0?}
    CheckInputPulse -->|Yes| SetPreActive[is_input_pulse_active = true]
    CheckInputPulse -->|No| SetPreInactive[is_input_pulse_active = false]
    SetPreActive --> CheckPostActivity
    SetPreInactive --> CheckPostActivity{Check postsynaptic activity}
    CheckPostActivity -->|PsActivityInput connected| CheckPsInput{PsActivityInput > 0?}
    CheckPostActivity -->|!PsActivityInput| CheckLTZone{LTZone->Output > 0?}
    CheckPsInput -->|Yes| SetPostActive[is_output_pulse_active = true]
    CheckPsInput -->|No| SetPostInactive[is_output_pulse_active = false]
    CheckLTZone -->|Yes| SetPostActive
    CheckLTZone -->|No| SetPostInactive
    SetPostActive --> UpdateXAvg{is_output_pulse_active?}
    SetPostInactive --> UpdateXAvg
    UpdateXAvg -->|Yes| XAvgSecretion[XAvg += (XModCoeff - XAvg) / (XTau * TimeStep)]
    UpdateXAvg -->|No| XAvgDissociation[XAvg -= XAvg / (XTau * TimeStep)]
    XAvgSecretion --> UpdateYAvg{is_input_pulse_active?}
    XAvgDissociation --> UpdateYAvg
    UpdateYAvg -->|Yes| YAvgSecretion[YAvg += (YModCoeff - YAvg) / (YTau * TimeStep)]
    UpdateYAvg -->|No| YAvgDissociation[YAvg -= YAvg / (YTau * TimeStep)]
    YAvgSecretion --> CalcXYDiff
    YAvgDissociation --> CalcXYDiff[Calculate XYDiff]
    CalcXYDiff --> CalcStdpInfluence[StdpInfluence = 1 - XYDiff]
    CalcStdpInfluence --> ClampStdp{StdpInfluence < 0?}
    ClampStdp -->|Yes| SetZero[StdpInfluence = 0]
    ClampStdp -->|No| CalcOutput
    SetZero --> CalcOutput[Calculate Output]
    CalcOutput --> End
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseSynapseCommon["NPulseSynapseCommon Base"]
        BaseSynapse[NPulseSynapseCommon]
    end

    subgraph NSynapseStdp["NSynapseStdp"]
        STDPModel[STDP Model]
    end

    subgraph External["External Components"]
        PreNeuron[Presynaptic neuron]
        PostNeuron[Postsynaptic neuron]
        LTZone[LT-zone]
        Channel[Channel]
    end

    BaseSynapse -->|inherits| NSynapseStdp
    NSynapseStdp -->|implements| STDPModel
    PreNeuron -->|Input| NSynapseStdp
    PostNeuron -->|PsActivityInput| NSynapseStdp
    LTZone -->|Output| NSynapseStdp
    NSynapseStdp -->|Output| Channel
    NSynapseStdp -->|StdpInfluence| Channel
```

### Usage in configurations

`NSynapseStdp` is used in STDP learning experiments:

- Synaptic plasticity modeling
- STDP mechanism studies
- Neural network training with STDP

**Typical STDP parameter values:**

1. **Standard STDP**: APlus=0.01, AMinus=0.012, XTau=0.02, YTau=0.01
2. **Asymmetric STDP**: APlus=0.015, AMinus=0.01, XTau=0.03, YTau=0.01
3. **Symmetric STDP**: APlus=0.01, AMinus=0.01, XTau=0.02, YTau=0.02

### References

See [Literature-References.md](../Literature-References.md): **[B]**.

### See Also

- [`NPulseSynapseCommon`](NPulseSynapseCommon.md) — common spiking synapse (base class)
- [`NPulseSynapseStdp`](NPulseSynapseStdp.md) — spiking STDP synapse with neurotransmitter model
- [Architecture.md](../Architecture.md) — library architecture
- [Scientific-Background.md](../Scientific-Background.md) — scientific background (STDP, synaptic plasticity)
