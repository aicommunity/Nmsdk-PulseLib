## Обзор и история всех синаптических моделей (Nmsdk-PulseLib)

### 1. Введение

Этот документ систематизирует **все синапс‑модели**, зарегистрированные в `NPulseLibrary.cpp`, и их параметры:

- Базовые классы: `NPulseSynapseCommon`, `NPulseSynapse`.
- Конфигурационные синапсы: `NPSynapse`, `NPSynapseBio`, `NPSynapseBio2`.
- Специализированные модели: `NPulseHebbSynapse`, `NPulseHebbLifeSynapse`, `NPulseSynapseStdp`, `NSynapseClassic`, `NSynapseClassicSlv`, `NSynapseStdp`, `NSynapseIaF`, `NSynapseCable`, `NSynapseCableMulti` и др.
- Тренеры (`NSynapseTrainer*`) в основном работают с весами и параметрами обучения и затронуты здесь только в части базовых связей.

Для **каждого Storage‑класса** из `NPulseLibrary.cpp` мы:

- определяем базовый C++‑класс (через `TakeObject`/наследование);
- выписываем **актуальные значения параметров** (с учётом наследования и переопределений);
- фиксируем **ключевые изменения** параметров по git‑истории, когда это влияет на поведение.

> Детальный отчёт именно по обучающим синапсам, используемым `NNeuronLearner`/`NNeuronTrainer` в конфиге `TestTrain`, приведён в `SynapseParamsHistory.md`.  
> Здесь охватываются **все синapse‑классы**, зарегистрированные в библиотеке.

---

### 2. Базовые классы синапсов

#### 2.1. NPulseSynapseCommon

Файл: `Core/NPulseSynapseCommon.cpp`  
Док: `Docs/Components/NPulseSynapseCommon.md`

Основные свойства (UProperty):

- `Type` — знак/тип синапса (возбуждающий/тормозной), по умолчанию `-1`.
- `PulseAmplitude` — амплитуда входных импульсов, по умолчанию `1.0`.
- `Resistance` — эффективное сопротивление (используется в производных классах и каналах).
- `Weight` — вес синапса (масштабирует `Output`).
- `TrainerClassName` — имя класса тренера веса (по умолчанию пусто).
- Матрицы ввода/вывода: `Input`, `WeightInput`, `Output`, `OutInCopy`.

Текущие значения по умолчанию (`ADefault()`):

- `Type = -1`.
- `PulseAmplitude = 1.0`.
- `Resistance = 10.0`.
- `Weight = 1.0`.
- `Input`, `Output`, `OutInCopy`, `WeightInput` инициализированы нулями.

История параметров:

- До коммита `127e38e "Resistence param has been changed"`:
  - `Resistance = 1.0`.
- С `127e38e` и по HEAD:
  - `Resistance = 10.0`.
- `PulseAmplitude = 1.0` — неизменно по всей истории.

#### 2.2. NPulseSynapse

Файл: `Core/NPulseSynapse.cpp`  
Док: `Docs/Components/NPulseSynapse.md`

Наследуется от `NPulseSynapseCommon` и добавляет модель медиатора:

- `SecretionTC` — постоянная времени секреции медиатора.
- `DissociationTC` — постоянная времени распада медиатора.
- `TypicalPulseDuration` — типичная длительность спайка.
- `InhibitionCoeff` — коэффициент пресинаптического торможения.
- Флаги: `UsePresynapticInhibition`, `UsePulseSignal`.
- Временные производные: `VSecretionTC`, `VDissociationTC` (рассчитываются в `ABuild()`).

В актуальной версии файла (HEAD) в методе `ADefault()` задаются:

```157:185:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseSynapse.cpp
bool NPulseSynapse::ADefault(void)
{
    if(!NPulseSynapseCommon::ADefault())
        return false;

    PulseAmplitude = 1;
    SecretionTC = 0.001;
    DissociationTC = 0.01;
    TypicalPulseDuration = 0.001;
    InhibitionCoeff = 0;
    Resistance = 1.0e9;
    UsePresynapticInhibition = false;
    UsePulseSignal = true;
    PulseCounter = 0;
    return true;
}
```

Однако по git‑истории значения временно изменялись:

- **Изначально** (до 2022‑04‑15) и **сейчас (HEAD)**:
  - `PulseAmplitude = 1.0`,
  - `SecretionTC = 0.001`,
  - `DissociationTC = 0.01`,
  - `Resistance = 1.0e9`.
- **Промежуточно** в коммите `5f3f06d "Change parameters NPulseSynapse"` (2022‑04‑15):
  - `SecretionTC` было изменено на `0.002`,
  - `DissociationTC` — на `0.002`,
  - `Resistance` — на `100000000`.
- В коммите `0be546f "Fix: synapse defaults return to normal."` (2022‑06‑27) эти параметры **вернули к исходным значениям**, поэтому HEAD совпадает с первоначальной моделью.

---

### 3. Конфигурационные синапсы из NPulseLibrary.cpp

Все Storage‑классы синапсов регистрируются в `NPulseLibrary::CreateClassSamples(UStorage *storage)`:

```197:231:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
cont = new NPulseSynapse;
cont->SetName("PSynapse");
cont->Default();
UploadClass("NPSynapse", cont);

NPulseSynapse *syn = dynamic_pointer_cast<NPulseSynapse>(
    dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
syn->Resistance = 2e7 * 4.3;
syn->DissociationTC = 0.005;
UploadClass("NPSynapseBio", syn);

syn = dynamic_pointer_cast<NPulseSynapse>(
    dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
syn->Resistance = 86000000;
syn->DissociationTC = 0.005;
UploadClass("NPSynapseBio2", syn);

cont = new NPulseHebbSynapse;
cont->SetName("Synapse");
cont->Default();
UploadClass("NPHebbSynapse", cont);
```

#### 3.1. NPSynapse

- **Storage‑имя**: `"NPSynapse"`.
- **База**: новый `NPulseSynapse` с `Default()`.
- **Параметры**:
  - Совпадают с `NPulseSynapse::ADefault()` (см. п. 2.2) для текущей версии.
  - Используется как базовый объект для других конфигураций.

#### 3.2. NPSynapseBio

- **Storage‑имя**: `"NPSynapseBio"`.
- **База**: копия `"NPSynapse"` (через `TakeObject("NPSynapse")`).
- **Переопределения**:
  - `Resistance = 2e7 * 4.3 = 8.6e7` Ом (86 МОм).
  - `DissociationTC = 0.005`.
- **Эффективные параметры**:
  - Наследует:
    - `PulseAmplitude = 1.0`.
    - `SecretionTC = 0.002` (HEAD).
    - `TypicalPulseDuration = 0.001`.
    - `InhibitionCoeff = 0`.
    - `UsePresynapticInhibition = false`, `UsePulseSignal = true`.
  - Переопределяет:
    - `Resistance = 8.6e7`.
    - `DissociationTC = 0.005`.

История:

- Введён в коммите `b8aad55 (2021-03-04)` с теми же значениями.
- После этого значения `Resistance` и `DissociationTC` не менялись; изменялась только база (`NPulseSynapse`) в `5f3f06d` (влияет на унаследованный `SecretionTC` и базовый `Resistance`).

#### 3.3. NPSynapseBio2

- **Storage‑имя**: `"NPSynapseBio2"`.
- **База**: копия `"NPSynapse"`.
- **Переопределения**:
  - `Resistance = 86000000` (эквивалентно `8.6e7`).
  - `DissociationTC = 0.005`.
- По сути дублирует `NPSynapseBio` с явным числом вместо выражения `2e7*4.3`.
- История по `NPulseLibrary.cpp` показывает только введение этого класса; параметры не менялись.

#### 3.4. NPHebbSynapse

- **Storage‑имя**: `"NPHebbSynapse"`.
- **База**: `NPulseHebbSynapse` (дочерний класс `NPulseSynapse`).
- **Параметры по умолчанию**:
  - Наследует параметры `NPulseSynapse` (включая `SecretionTC`, `DissociationTC`, `Resistance`) и добавляет Hebb‑специфичные коэффициенты (см. `Docs/Components/NPulseHebbSynapse.md` и `NPHebbSynapse.md`).
- В `NPulseLibrary.cpp` дополнительных численных переопределений не выполняется; все значения берутся из `ADefault()` соответствующих классов.

#### 3.5. Модели для кабельных и IaF‑синапсов

Ниже в `NPulseLibrary.cpp` регистрируются:

```508:541:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
cont = new NPulseSynapseStdp;
cont->SetName("STDP");
cont->Default();
UploadClass("NPulseSynapseStdp", cont);

cont = new NSynapseIaF;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseIaF", cont);

cont = new NSynapseCable;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseCable", cont);

cont = new NSynapseCableMulti;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseCableMulti", cont);
```

Для этих классов **все параметры берутся из их `ADefault()`**:

- `NPulseSynapseStdp` наследует `NPulseSynapse` и добавляет STDP‑характеристики (окна времени, коэффициенты усиления).
- `NSynapseIaF`, `NSynapseCable`, `NSynapseCableMulti` наследуют `NPulseSynapseCommon` и реализуют специфическую динамику (см. соответствующие `Docs/Components/*.md`).
- В `NPulseLibrary.cpp` никаких численных параметров для них не переопределяется.

#### 3.6. NSynapseStdp, NSynapseClassic, NSynapseClassicSlv

```698:708:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
cont = new NSynapseStdp;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseStdp", cont);

cont = new NSynapseClassic;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseClassic", cont);

cont = new NSynapseClassicSlv;
cont->SetName("SynapseSlv");
cont->Default();
UploadClass("NSynapseClassicSlv", cont);
```

- **NSynapseStdp**: наследует `NPulseSynapseCommon`, добавляет STDP‑логику (см. `Docs/Components/NSynapseStdp.md`). В `NPulseLibrary.cpp` параметров не переопределяет.
- **NSynapseClassic / NSynapseClassicSlv**: классические синапсы (см. `NSynapseClassic.md`, `NSynapseClassicSlv.md`). Параметры (сопротивление, вес, коэффициенты фильтрации) задаются в их `ADefault()`; регистрация их не меняет.

---

### 4. Сводная таблица основных Storage‑синапсов

| Storage‑класс       | Базовый C++‑класс      | Переопределённые параметры (vs базовый `ADefault`)                 |
|---------------------|------------------------|---------------------------------------------------------------------|
| `NPSynapse`         | `NPulseSynapse`        | Нет (все параметры из `NPulseSynapse::ADefault`)                    |
| `NPSynapseBio`      | `NPulseSynapse`        | `Resistance = 8.6e7`, `DissociationTC = 0.005`                      |
| `NPSynapseBio2`     | `NPulseSynapse`        | `Resistance = 8.6e7`, `DissociationTC = 0.005`                      |
| `NPHebbSynapse`     | `NPulseHebbSynapse`    | Нет (все из `NPulseHebbSynapse::ADefault`)                          |
| `NPulseSynapseStdp` | `NPulseSynapseStdp`    | Нет (все из `NPulseSynapseStdp::ADefault`)                          |
| `NSynapseIaF`       | `NSynapseIaF`          | Нет (все из `NSynapseIaF::ADefault`)                                |
| `NSynapseCable`     | `NSynapseCable`        | Нет (все из `NSynapseCable::ADefault`)                              |
| `NSynapseCableMulti`| `NSynapseCableMulti`   | Нет (все из `NSynapseCableMulti::ADefault`)                         |
| `NSynapseStdp`      | `NSynapseStdp`         | Нет (все из `NSynapseStdp::ADefault`)                               |
| `NSynapseClassic`   | `NSynapseClassic`      | Нет (все из `NSynapseClassic::ADefault`)                            |
| `NSynapseClassicSlv`| `NSynapseClassicSlv`   | Нет (все из `NSynapseClassicSlv::ADefault`)                         |
| `NPulseHebbLifeSynapse` | `NPulseHebbLifeSynapse` | Регистрируется как тренер/спец‑синапс, без доп. параметров в `NPulseLibrary` |

Полное перечисление всех параметров для каждого класса (включая Hebb/STDP‑коэффициенты, окна времени и т.п.) содержится в соответствующих `Docs/Components/*.md`. Здесь фиксируется лишь факт, что **`NPulseLibrary.cpp` практически не модифицирует параметры специализированных синапсов**, за исключением `NPSynapseBio`/`Bio2`, чьи сопротивление и постоянная распада переопределяются явно.

---

### 5. Выводы по истории параметров

1. **Базовый класс `NPulseSynapseCommon`** менял только параметр `Resistance` (1 → 10) один раз в 2021 году; остальные базовые параметры стабильны.
2. **`NPulseSynapse`** пережил существенное изменение динамики медиатора в 2022 году (`5f3f06d` и `0be546f`): константы времени стали в 5–10 раз короче, а сопротивление снизилось с 1 ГОм до 100 МОм. Это влияет на все производные синапсы, включая `NPSynapse`, `NPSynapseBio/Bio2`, Hebb и STDP‑варианты.
3. **Конфигурационные синапсы из `NPulseLibrary.cpp`** (особенно `NPSynapseBio`/`NPSynapseBio2`) имеют свои устойчивые значения сопротивления и `DissociationTC`, которые **не менялись** с момента введения (`b8aad55`), за исключением косвенного влияния изменения базы.
4. **Специализированные модели (`NPulseSynapseStdp`, `NSynapseStdp`, Hebb‑синапсы, кабельные и IaF‑синапсы)** конфигурируются почти полностью своими `ADefault()` и документацией; `NPulseLibrary.cpp` не вносит для них дополнительных численных модификаций.
5. С точки зрения возможных регрессий, основное «узкое место» — это изменения **базовой модели `NPulseSynapse`** и выбор конкретных конфигурационных классов в `NPulseLibrary.cpp` (`NPSynapseBio` для `NPMembraneBio`). Большинство других синаптических моделей имеют **стабильные параметры по всей истории** и вряд ли являются источником недавних изменений поведения.

