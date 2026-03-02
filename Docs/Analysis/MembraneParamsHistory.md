## Отчёт по параметрам мембран (NPulseMembrane) для обучающих конфигов TestTrain

### 1. Введение

Этот документ дополняет `IonParamsHistory.md`, `SynapseParamsHistory.md` и `LTZoneUsageHistory.md` и фокусируется на **мембранных моделях**, которые используются в обучающих нейронах:

- `NNeuronLearner` и `NNeuronTrainer` в конфиге `TestTrain` (`Bin/Configs/Bakhshiev/TestTrain`).
- Внутренний нейрон: `NSPNeuronGen` (`NeuronClassName = "NSPNeuronGen"`).
- Мембрана: `NPMembraneBio` (биоинспирированная импульсная мембрана).

Цели отчёта:

- зафиксировать базовые параметры `NPulseMembrane` и их конфиг‑варианты из `NPulseLibrary.cpp`;
- проследить по git‑истории, изменялись ли численные параметры этих мембран;
- оценить, могли ли изменения мембран повлиять на наблюдаемое обучение (рост дендритов и числа синапсов).

---

### 2. Используемые мембранные классы в TestTrain

#### 2.1. Связка Learner/Trainer → NSPNeuronGen → NPMembraneBio

- В `NNeuronLearner` и `NNeuronTrainer` (файлы `Core/NNeuronLearner.cpp`, `Core/NNeuronTrainer.cpp`) по умолчанию задано:
  - `NeuronClassName = "NSPNeuronGen"`.
- В `NSPNeuronGen` (см. `Docs/Components/NSPNeuronGen.md`):
  - `MembraneClassName = "NPMembraneBio"`;
  - `LTZoneClassName = "NPulseLTZoneThreshold"`;
  - `LTMembraneClassName = ""`.
- В `Model_00.xml` для TestTrain:
  - у нейрона и всех дендритов/сом явно указано `Class="NPMembraneBio"`.

Итак, в цепочках:

- `NNeuronLearner` → `NSPNeuronGen` → `NPMembraneBio`,
- `NNeuronTrainer` → `NSPNeuronGen` → `NPMembraneBio`.

Другие мембранные конфигурации (`NPMembraneBio2`, LT‑мембраны и пр.) в TestTrain прямо не используются.

---

### 3. Базовые мембраны: NPulseMembraneCommon и NPulseMembrane

#### 3.1. NPulseMembraneCommon

Файлы: `Core/NPulseMembraneCommon.h`, `Core/NPulseMembraneCommon.cpp`.

Основные задачи:

- хранение списка каналов (`Channels`) и синапсов (`Synapses`);
- накопление суммарного потенциала `SumPotential`;
- учёт активности нейрона (`IsNeuronActive`) через поле `Feedback` от LTZone.

Ключевые моменты реализации (актуальная версия):

- В `ACalculate()` после вызова `ACalculate2()`:
  - если `Feedback > 0` и нейрон был неактивен — устанавливается `IsNeuronActive = true` и вызывается `NeuronActivated()` у всех каналов;
  - если `IsNeuronActive` и `Feedback <= 0` — `IsNeuronActive` сбрасывается в `false`;
  - затем суммируется потенциал всех каналов:

```185:203:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseMembraneCommon.cpp
 if(IsNeuronActive && Feedback<=0)
  IsNeuronActive=false;
 SumPotential(0,0)=0;
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   SumPotential(0,0)+=Channels[i]->Output(0,0);
 return true;
```

По git‑истории:

- ранние коммиты (`700b7df`, `66d105a`) добавляли базовый функционал и общие классы;
- в `b8aad55` был лишь переразбор форматирования блока суммирования (см. diff выше), **численная логика не менялась**;
- в более поздних коммитах изменения касались структуры кода, а не численных параметров мембраны.

Заключение: `NPulseMembraneCommon` не содержит собственных численных RC‑параметров (они в каналах), и их значения по истории **не менялись**.

#### 3.2. NPulseMembrane

Файлы: `Core/NPulseMembrane.h`, `Core/NPulseMembrane.cpp`.

Добавляет поверх `NPulseMembraneCommon`:

- параметры:
  - `FeedbackGain` — коэффициент обратной связи от LTZone;
  - `ResetAvailable` — флаг наличия механизма сброса;
  - `SynapseClassName` — имя класса синапсов;
  - `ExcChannelClassName`, `InhChannelClassName` — имена классов возбудительных и тормозных каналов;
  - `NumExcitatorySynapses`, `NumInhibitorySynapses` — количество создаваемых синапсов.

Значения по умолчанию (HEAD) в `NPulseMembrane::ADefault()`:

```308:318:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseMembrane.cpp
bool NPulseMembrane::ADefault(void)
{
 if(!NPulseMembraneCommon::ADefault())
  return false;
 FeedbackGain=2;
 ResetAvailable=true;
 SynapseClassName="NPSynapse";
 ExcChannelClassName="NPExcChannel";
 InhChannelClassName="NPInhChannel";
 NumExcitatorySynapses=1;
 NumInhibitorySynapses=1;

 return true;
}
```

По git‑истории (по `git log -- Core/NPulseMembrane.cpp`):

- численные значения `FeedbackGain=2`, `NumExcitatorySynapses=1`, `NumInhibitorySynapses=1` присутствуют уже в самых ранних версиях и **не менялись**;
- изменения в истории затрагивали:
  - исправление багов в построении/удалении частей (`0330359`, `6943c32` и др.);
  - добавление логирования и проверок;
  - перенос инициализации в `ADefault`, без изменения самих чисел.

Таким образом, базовая мембрана `NPulseMembrane` по всей доступной истории имеет **стабильные численные параметры**.

---

### 4. Конфигурационные мембраны из NPulseLibrary.cpp

Файл: `Core/NPulseLibrary.cpp`, функция `NPulseLibrary::CreateClassSamples(UStorage *storage)`.

Фрагмент, регистрирующий мембраны (упрощённо):

```340:369:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
UEPtr<NPulseMembrane> membr;
cont=new NPulseMembrane;
cont->SetName("PMembrane");
cont->Default();
UploadClass("NPMembrane",cont);

// Регистрация NPNeuronMembrane как конфигурационного варианта NPMembrane
membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->Default();
UploadClass("NPNeuronMembrane",membr);

// Регистрация NPNewNeuronMembrane как конфигурационного варианта NPMembrane
membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->Default();
UploadClass("NPNewNeuronMembrane",membr);

membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->ExcChannelClassName="NPExcChannelBio";
membr->SynapseClassName="NPSynapseBio";
membr->InhChannelClassName="NPInhChannelBio";
UploadClass("NPMembraneBio",membr);

membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->ExcChannelClassName="NPExcChannelBio2";
membr->SynapseClassName="NPSynapseBio2";
membr->InhChannelClassName="NPInhChannelBio2";
membr->FeedbackGain = 0.02;
UploadClass("NPMembraneBio2",membr);

// Участки мембраны низкопороговой зоны и др. (NPLTZoneNeuronMembrane, NPSynNeuronMembrane, NCSynNeuronMembrane и т.д.)
...
```

#### 4.1. NPMembrane (базовая Storage‑мембрана)

- **Storage‑имя**: `"NPMembrane"`.
- **База**: новый `NPulseMembrane` с `Default()`.
- **Параметры** (все из `NPulseMembrane::ADefault()`):
  - `FeedbackGain = 2.0`;
  - `ResetAvailable = true`;
  - `SynapseClassName = "NPSynapse"`;
  - `ExcChannelClassName = "NPExcChannel"`;
  - `InhChannelClassName = "NPInhChannel"`;
  - `NumExcitatorySynapses = 1`;
  - `NumInhibitorySynapses = 1`.

История: с первых версий `NPulseLibrary.cpp` эти значения **не переопределяются** и не меняются по всей глубине истории.

#### 4.2. NPNeuronMembrane / NPNewNeuronMembrane

- **Storage‑имена**: `"NPNeuronMembrane"`, `"NPNewNeuronMembrane"`.
- **База**: копии `"NPMembrane"`:
  - вызывается `TakeObject("NPMembrane")` и `Default()` без изменения чисел.

Итого: численные параметры этих классов совпадают с `NPMembrane` и использованы для обратной совместимости/новых нейронов, но **не отличаются RC‑параметрами**.

#### 4.3. NPMembraneBio (используемая в TestTrain)

- **Storage‑имя**: `"NPMembraneBio"`.
- **База**: копия `"NPMembrane"`.
- **Переопределения в `NPulseLibrary.cpp`**:
  - `ExcChannelClassName = "NPExcChannelBio"`;
  - `InhChannelClassName = "NPInhChannelBio"`;
  - `SynapseClassName = "NPSynapseBio"`.
- **Наследуемые параметры**:
  - `FeedbackGain = 2.0` (не изменяется);
  - `ResetAvailable = true`;
  - `NumExcitatorySynapses = 1`;
  - `NumInhibitorySynapses = 1`.

История (по `git log` и `git show b8aad55`):

- Класс `NPMembraneBio` был введён в коммите `b8aad55 (2021‑03‑04)` **с этими же значениями**;
- во всех последующих коммитах блок регистрации `NPMembraneBio` не менялся — ни `FeedbackGain`, ни классы каналов/синапсов для этой мембраны не трогались.

Таким образом, **головная мембрана, используемая в обучающих конфигурациях TestTrain, имеет стабильные параметры с момента введения**.

#### 4.4. NPMembraneBio2 (альтернативная биомембрана)

- **Storage‑имя**: `"NPMembraneBio2"`.
- **База**: копия `"NPMembrane"`.
- **Переопределения**:
  - `ExcChannelClassName = "NPExcChannelBio2"`;
  - `InhChannelClassName = "NPInhChannelBio2"`;
  - `SynapseClassName = "NPSynapseBio2"`;
  - `FeedbackGain = 0.02`.

Эта мембрана предназначена для альтернативных моделей (например, совместимых с кабельной моделью) и **не используется** в текущем TestTrain, но важна как пример:

- уменьшенный `FeedbackGain` сильно ослабляет влияние обратной связи LTZone на мембрану;
- отличные RC‑параметры каналов (`NPExcChannelBio2`/`NPInhChannelBio2`) меняют временную динамику.

По git‑истории блок `NPMembraneBio2` был добавлен позже (коммиты `98d1edc`/`f44e78c`) и после этого **не менялся**.

#### 4.5. LT‑ и Syn‑мембраны

Другие зарегистрированные мембраны в `NPulseLibrary.cpp`:

- `NPLTZoneNeuronMembrane` — LT‑участки мембраны, используют `NPLTExcChannel`/`NPLTInhChannel` (каналы с `Capacity = 1e-8`, `RestingResistance = 1e6`).
- `NPLTZoneSynNeuronMembrane` — LT‑сегменты для синаптических каналов (`NPLTSynExcChannel`/`NPLTSynInhChannel`). 
- `NPSynNeuronMembrane` — мембрана с `NPSynExcChannel`/`NPSynInhChannel`.
- `NCSynNeuronMembrane` — непрерывная мембрана с `NCSynExcChannel`/`NCSynInhChannel`.
- `NPNeuronHebbMembrane` — мембрана с `SynapseClassName = "NPHebbSynapse"`.

В TestTrain‑конфиге `NSPNeuronGen` и связанные нейроны **не используют** эти мембраны напрямую; они важны для других сценариев (LT‑зоны, Hebb‑обучение, кабельная модель).
По истории `NPulseLibrary.cpp` их численные параметры (RC‑характеристики каналов и `FeedbackGain` у `NPMembraneBio2`) после введения не менялись.

---

### 5. Сводная таблица параметров мембран

| Storage‑класс        | Базовый класс       | FeedbackGain | ResetAvailable | ExcChannelClassName   | InhChannelClassName    | SynapseClassName  | Примечание                          |
|----------------------|---------------------|--------------|----------------|------------------------|------------------------|-------------------|-------------------------------------|
| `NPMembrane`         | `NPulseMembrane`    | 2.0          | true           | `NPExcChannel`        | `NPInhChannel`        | `NPSynapse`       | базовая мембрана                    |
| `NPNeuronMembrane`   | `NPulseMembrane`    | 2.0          | true           | `NPExcChannel`        | `NPInhChannel`        | `NPSynapse`       | копия `NPMembrane`                  |
| `NPNewNeuronMembrane`| `NPulseMembrane`    | 2.0          | true           | `NPExcChannel`        | `NPInhChannel`        | `NPSynapse`       | копия `NPMembrane`                  |
| `NPMembraneBio`      | `NPulseMembrane`    | 2.0          | true           | `NPExcChannelBio`     | `NPInhChannelBio`     | `NPSynapseBio`    | **используется в TestTrain**        |
| `NPMembraneBio2`     | `NPulseMembrane`    | 0.02         | true           | `NPExcChannelBio2`    | `NPInhChannelBio2`    | `NPSynapseBio2`   | альтернативная биомембрана          |
| `NPLTZoneNeuronMembrane` | `NPulseMembrane`| 2.0 (наследует) | true        | `NPLTExcChannel`      | `NPLTInhChannel`      | `NPSynapse`       | LT‑участки мембраны                  |
| `NPLTZoneSynNeuronMembrane` | `NPulseMembrane` | 2.0       | true           | `NPLTSynExcChannel`   | `NPLTSynInhChannel`   | `NPSynapse`       | LT‑сегменты с синапс‑каналами       |
| `NPSynNeuronMembrane`| `NPulseMembrane`    | 2.0          | true           | `NPSynExcChannel`     | `NPSynInhChannel`     | `NPSynapse`       | мембрана с син. каналами            |
| `NCSynNeuronMembrane`| `NPulseMembrane`    | 2.0          | true           | `NCSynExcChannel`     | `NCSynInhChannel`     | `NPSynapse`       | непрерывная син. мембрана           |
| `NPNeuronHebbMembrane`| `NPulseMembrane`   | 2.0          | true           | `NPExcChannel`/`NPInhChannel` | те же           | `NPHebbSynapse`   | мембрана для Hebb‑нейронов          |

Все значения в таблице отражают **HEAD**; по истории `NPulseLibrary.cpp` переопределения для этих мембран не менялись после их первой регистрации.

---

### 6. Выводы

1. **Мембраны, реально используемые в обучающих конфигурациях TestTrain, — это `NPMembraneBio` (через `NSPNeuronGen`).**  
   Их параметры (`FeedbackGain = 2`, классы Bio‑каналов и Bio‑синапсов) были заданы в 2021 году (`b8aad55`) и с тех пор **остаются неизменными**.

2. **Базовая логика `NPulseMembraneCommon` и `NPulseMembrane` по суммированию потенциалов и работе с обратной связью стабильно по всей истории.**  
   Изменения в этих файлах касались в основном форматирования, исправления багов построения структуры и совместимости, но **не численных параметров**.

3. **Альтернативные мембраны (`NPMembraneBio2`, LT‑ и Syn‑мембраны)** предлагают другие RC‑параметры и `FeedbackGain`, но в TestTrain они не задействованы.  
   Они могут быть использованы в будущем для экспериментов с другой динамикой, но не объясняют текущее поведение Learner/Trainer в рассматриваемой конфигурации.

4. **С точки зрения регрессий, наблюдаемое поведение обучения (слабая чувствительность к числу синапсов, выбор длины дендритов) нельзя объяснить изменениями параметров мембран.**  
   Параметры `NPMembraneBio` и базовых мембран/каналов были стабилизированы давно; гораздо вероятнее, что корень проблемы — в критериях алгоритмов (`ChangeDendriteStatus`, `SomaSynchronizePattern`, `SomaSynapseNormalization`) и текущих RC‑параметрах каналов/синапсов, уже разобранных в связанных отчётах.

5. **Практический вывод:** при поиске причин поведения алгоритмов роста структуры стоит сосредоточиться на логике Learner/Trainer и параметрах каналов/синапсов, а мембраны можно считать «фиксированной платформой», параметры которой по истории не переживали драматических изменений.

