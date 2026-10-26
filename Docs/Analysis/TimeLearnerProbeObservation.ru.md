# Общий измеритель проб TimeLearner

`Core/TimeLearner/ProbeObservation.h/.cpp` вводит ProbeSpec, ProbeObservation и
ограниченный ProbeRecorder для будущего общего контроллера. Это внутренний
формат training/characterization schema=1, отдельный от recognition CSV schema=4.
Legacy-контроллеры на него пока не переключены; ControllerVersion=2 не объявлена
готовой. Новый код не меняет критерии старого обучения.

## Данные и владелец

ProbeSpec неизменяемо копируется при создании recorder: run/trial/sample IDs,
hash сконфигурированного паттерна, непрозрачный modelRevision, полное временное
окно, максимальный разрыв между samples, максимум samples, baseline каждого
физического канала, peak windows и ожидаемые stimulus IDs. Для каждого окна
заданы input ID, индекс физического канала и ID фактического импульса.
Baseline передаёт адаптер из объявленного rest-state модели. Нулевой baseline
не подразумевается; один физический канал может обслуживать несколько окон.

Recorder не владеет Rdk-компонентами и не вызывает Build/Init/Reset/Calculate.
Native scheduler рассчитывает дерево; адаптер передаёт готовый кадр потенциалов
и фактические события стимулов. Для новой пробы создаётся новый recorder,
без сброса всего learner. Ошибочная спецификация даёт invalid_argument до
запуска; ошибки данных сохраняются в reasons. Это не изменение bool-контракта
методов Rdk.

## Измерение и validity

Амплитуда — signed deviation потенциала от явно переданного baseline; raw
potential сохраняется отдельно. Это не новая общая амплитудная цель. Reference
будущего контроллера должен быть captured в той же объявленной процедуре.

Внутри окна ищутся реальные локальные максимумы не ниже minimumAmplitude
(default 1e-6). Смежные равные отсчёты образуют один plateau. Сохраняются время
первого и последнего отсчётов plateau, без интерполяции. Несколько отдельных
максимумов не ниже этого пола дают Ambiguous; малые вторичные пики автоматически
не отбрасываются процентной эвристикой. Missing, Clipped и Incomplete отделены
от Valid. Максимум на краю окна считается Clipped. Это консервативное правило
первого измерителя, а не доказательство однозначной wave assignment для всех
физических профилей.

Числа невалидного/отсутствующего пика являются optional, а не нулями.
DelayFromStimulus остаётся unknown, пока не получено соответствующее реальное
событие, даже если сам пик измерен. Fixture сохраняет unknown как JSON null.

DelayFromStimulus вычисляется из timestamp измеренного пика и фактического
события с соответствующим ID. Пик до фактического импульса запрещает valid
итог. Предсказания кабельной модели на вход recorder не поступают и не могут
заменить отсутствующий пик.

Полнота требует покрытия всего окна, последовательных времён и отсутствия
разрывов больше maximumSampleGap. Досрочное Close, NaN/Inf, переполнение
потенциал−baseline, неверная размерность и исчерпание sample budget запрещают
валидный результат. Допуск сравнения времени порядка machine epsilon нужен
только для floating-point арифметики; он не заменяет SyncTolerance, амплитудные
допуски или допуск в один timestep.

Stimulus IDs должны совпасть с ожидаемым множеством; неизвестные/повторные
события и неправильные timestamps сохраняют ошибку. Список spikes опционален;
при recordSpikes=false ответ по спайкам неизвестен, а не равен нулю. При true
count выводится из списка timestamps/channel IDs. Поздний спайк внутри полного
окна сохраняется; дубли, неверный канал и время делают пробу невалидной.
Классификацию TP/FP и их response windows выполняет отдельный scorer.

`complete=true` означает покрытие окна, не успех обучения. Даже отдельный
PeakValidity::Valid не разрешает принятие решения: потребитель обязан проверить
весь ProbeObservation.valid, все reasons, revision и смысл протокола.

## Revision и будущий адаптер

Recorder сравнивает передаваемый currentModelRevision на каждом Observe и при
Close с revision ProbeSpec. Обнаруженное изменение остаётся ошибкой, даже если
потом восстановлен исходный token. Вычисление revision принадлежит адаптеру:
он должен учитывать всю влияющую физику, связи, топологию, timestep/reset policy
и каждое изменение, способное нарушить свежесть измерения. Cache допустим
только при гарантированном владении всеми записями. Сам recorder не обнаружит
изменение, о котором адаптер ему не сообщил.

Native fixtures используют production semantic identity физического нейрона
до/после замороженной пробы и при явном fault injection. Это ещё не полный
production механизм revision/invalidation для произвольных внешних setter,
Build, growth, retarget и reset policy. Его подключение остаётся обязательной
работой. Полный controller ModelSpec hash и TrainingCertificate здесь не введены.

```text
legal boundary:
    adapter applies queued edit and native physical reset
    adapter establishes revision and declared rest-state baseline
    arm recorder before first stimulus
native scheduler tick:
    calculate children once
    adapter reads real physical soma channels and actual stimulus edges
    recorder observes those data and current revision
window closure:
    observation = recorder.Close(actual_time, current_revision, reason)
    controller consumes only matching valid complete observations
    otherwise bounded recovery or explicit unresolved outcome
```

## Native проверки и границы

В Classic SomaNeuronAmplitude[1..N] содержит текущие потенциалы; в Branch эти
строки содержат сохранённые MaxIterSomaAmp. Поэтому fixture читает физические
NPulseMembrane::SumPotential: отдельные сомы Classic и общую Soma1 Branch.
Нельзя обобщать старые диагностические матрицы одним accessor без учёта смысла.

Fixtures создают immutable storage-профиль с LTZ threshold=1, выполняют cold
native Reset/первый tick, затем замораживают обучение и предъявляют штатную
пачку. Это subthreshold characterization текущего wiring, не completed Train,
не проверка всех solo-входов и не frozen evaluation обученной recognition-модели.
Fault injection намеренно меняет индивидуальный Rs посреди окна; такое действие
служит отрицательным контролем, а не разрешённой production записью.

Артефакты fixture содержат полные raw soma/stimulus CSV, actual event timestamps,
configured pattern hash и отдельный hash фактических stimulus events,
baseline/windows, observation reasons и native semantic identity до/после.
Сериализация этого fixture не является готовым production экспортом всех
ProbeObservation или адаптером нового CLI/scorer.
