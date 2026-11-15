# Максимум закреплённого импульса: matched omission v1

`Core/TimeLearner/MatchedOmissionAssignment.h/.cpp` реализует чистый production
validator/selector составного наблюдения. Метод
`matched-omission-assigned-maximum-v1` выбирает измеренный максимум полной
пачки, поддержанный только закреплённым импульсом в объявленном протоколе
исключения импульсов. Это первый алгоритмический блок native composite
integration: сам utility не запускает компоненты, не обучает модель и не
создаёт `TrainingCertificate`. Смысл существующих schemas 1–6 не меняется.

## Область применимости и запись

Для N>=2 один witness содержит `fullBefore`, по одному `omitEachInput` для
каждого исходного ordinal и `fullAfter`: ровно N+2 законченных native пробы.
Маска full содержит все исходные ordinals; omit-j — все, кроме j. Порядок
элементов вектора controls не задаёт их идентичность. Пробы должны идти
последовательно, без перекрытия фактически записанных интервалов; два full
ограничивают controls с обеих сторон. Каждый trial ID используется один раз.

N1 возвращает `Unsupported/single_input_uses_existing_single_impulse_path`.
Для него остаётся существующий SingleImpulse путь; диагностический schema5
packet от этого не становится сертификатом. V1 поддерживает штатный
прямоугольный source: конечная положительная amplitude, PulseLength>=1/f,
FrequencyDeviation=0, UseTransitSignal=false, UsePatternOutput=false.

Производитель передаёт `MatchedOmissionTrace` из фактических callbacks и
проверенного native closure: `prepared`, `observation`, `stimulusSamples`,
исходные `presentedOrdinals`, physical paths и source profile. Поля
`nativeCompleted/fullMaskRestored` должны отражать native readback.
`modelInstanceBinding/resetBinding` задаёт producer из живого контекста и
объявленной native reset процедуры. Utility не получает доступ к графу и не
доказывает происхождение произвольно сконструированных структур.

`MatchedOmissionSpecification` связывает run/group/sample, исходный pattern,
protocol, base/effective model revisions, epoch, instance/reset binding,
закреплённый input и физический канал. Classic использует канал assignedInput
из N каналов; Branch — один канал0 и тот же actual solo selector во всех
пробах. В controls observation.patternHash относится к предъявленной маске,
а sourcePatternHash — к исходной полной пачке.

## Проверки до выбора

`ValidateMatchedOmissionPolicy(policy, source, inputCount)` выполняет дешёвый
preflight без знания prepared effective revision. Полная проверка контекста
повторяется в `SelectMatchedOmissionMaximum(specification, policy, witness)`.

- У всех проб совпадают binding/revisions/epoch, source physics, input/channel,
  исходная цель и native reset процедура; полное предъявление восстановлено.
- Baseline каждого канала сверяется с fullBefore в отдельном baselineAbsolute.
  Ни один baseline не предполагается нулевым.
- Все каналы имеют одинаковую длину и общий clock. Относительные timestamps
  соответствуют последовательным native frame indices без потерь/дубликатов;
  покрыто объявленное окно, а закрытие произошло на первом допустимом frame.
- `timeComparisonTolerance` задаётся явно (default NaN запрещён), конечен,
  >=0 и строго меньше половины native tick. Он покрывает roundoff вычитания
  абсолютных часов. Интерполяции, сдвига на tick или ресэмплинга нет.
- Каждый retained actual event имеет тот же исходный ordinal, тот же frame
  index и относительное время в этом допуске. Одного совпадения ISI/hash мало.
- Source readback содержит amplitude на `floor(PulseLength*f)` frame начиная
  с фактического event и ноль вне retained pulse regions. Формы, удалённые
  импульсы, незаконченные/перекрывающиеся source pulses проверяются явно.
- Все значения и промежуточная арифметика конечны. Каждый frame и все N+2
  bursts учитываются в maximumSamples/maximumTotalSamples/maximumNativeProbes.

Объединённый reset/model binding и повторяемость — конечные operational
условия метода, не checkpoint всех динамических переменных и не теорема о
нелинейной причинности. Маски разрешено предъявлять только штатным native
runner; нельзя менять target обучения для создания этих controls.

## Support и измеренная величина

`ExtractMeasuredResponse` извлекает все реальные положительные максимумы
fullBefore. Дополнительно учитываются положительные clipped boundary maxima,
чтобы незавершённый высокий отклик не исчез из решения. Непрерывное плато
имеет реальные first/last samples; равные раздельные максимумы различаются.

Два full должны совпасть по baseline-normalized отсчётам всех каналов в
отдельных repeatabilityAbsolute/Relative. Повтор должен также воспроизвести
единственный победивший максимум с теми же first/last sample indices. Смена
высшего максимума или сдвиг плато не скрываются pointwise допуском.

Для full-максимума p сравнивается его плато с фиксированным neighbourhood
`comparisonRadiusTicks>=1`. В каждом frame:

```text
a = fullBefore.raw - fullBefore.baseline
b = fullAfter.raw  - fullAfter.baseline
c = omit_j.raw    - omit_j.baseline
I = [min(a,b), max(a,b)]
T = influenceAbsolute + influenceRelative * max(abs(a),abs(b),abs(c))
```

Input j — Supported, если хотя бы в одном frame расстояние c до всего I
строго больше T. Если для всех frames максимальное расстояние до концов I
не больше T — NotSupported. Иначе — Unknown. Это проверка влияния с
неопределённостью двух full, не оценка доверительного интервала по выборке.
Знак эффекта не важен: увеличение потенциала при omission тоже является
влиянием и может указывать на inhibition/interaction.

Допустимый максимум имеет support set ровно `{assignedInput}`, без Unknown,
не предшествует assigned actual event и не clipped. Из таких максимумов
выбирается единственный самый высокий **full amplitude**. Меньшие экстремумы
не мешают. Более высокий/равный максимум, который ещё может принадлежать
assigned input, но имеет shared/unknown support, даёт Ambiguous; меньший
однозначный максимум не подставляется. Possibly assigned clipped boundary
не позволяет объявить законченный максимум. Изменение pre-stimulus максимума
при omission будущего assigned event также остаётся Ambiguous.

Возвращаемые rawPotential/amplitude/time/plateauEnd взяты из fullBefore.
Максимум difference signal никогда не используется вместо полного потенциала.
У посторонних импульсов не требуются response peaks. Все обязательные source
events, кроме явно исключённых маской, по-прежнему должны быть зарегистрированы.

## Результат и следующий native шаг

`MatchedOmissionResult` содержит Valid/Missing/Ambiguous/Clipped/Incomplete/
InvalidContext/NonRepeatable/BudgetExhausted/Unsupported, причины и support
каждого full-максимума. `measuredMaximum/selectedFullMaximum` заполнены только
при Valid. Это измерительный operational outcome; старые domain hypotheses
не повышают свою eligibility, schemas1–6 не ослабляются.

Native integration должна предъявить bounded группу для references, поиска
и fresh final verification, сохранять реальные traces/provenance и учитывать
их стоимость в бюджетах. После любого edit старый witness не переносится на
новую model revision. Certificate integration, codecs/readers и native
normal/ASan проверки относятся к следующему этапу; unit utility их не заменяет.
