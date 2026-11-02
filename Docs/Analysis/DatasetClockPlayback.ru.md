# Время предъявления NDatasetBase

Исправление требует серверной приёмки. Оно относится к штатному burst playback
NDatasetMatrix и файлового NDataset через NDatasetBase, а не меняет задачу обучения.

Baseline матрица корня:12 cases, timed/continuous,2000/20000Hz, Delay0/5/30ms,
origins0/0.1/1s, реальные генератор/синапс/сома и полные0.25s traces. При
нулевой паузе4 cases нарушили инвариантность: входы и soma зависели от origin.
В остальных8 invariance прошла. Fixture использует native storage, model
scheduler и Reset; неверные первые версии fixture сохранены отдельно и не
считаются доказательством ошибки dataset.

NDatasetBase прежде проверял окончание импульса строгим сравнением now с
now_at_fire+pulse_length, а onset/repeat — с допуском1e-12. Округление суммы
абсолютных double timestamp иногда продлевало ожидание на tick. Если Delay0,
это задерживало следующий Begin и меняло последующие предъявления.

Теперь длительности считаются от штатного целочисленного времени Rdk:

```text
Begin: sample_start_us = Environment.Time.GetTime()
Fire: pulse_start_us[f] = native_now_us; pulse_duration[f] = effective PulseLength
elapsed = double(native_now_us - native_start_us) / 1e6
compare elapsed with relative requested duration
```

Вычитание выполняется до перевода в секунды. Перенос абсолютного origin не
должен менять сравнение. Pulse boundaries и repeat используют один roundoff
guard: max(1e-12,16*epsilon*max(1,abs(elapsed),abs(duration))). Это точность
представления относительного времени; она не является SyncTolerance, не
задаётся долей timestep и не меняет амплитудные допуски. Исходный1e-12 onset
allowance сохраняется. Назад идущие часы без restart и нечисловые сравнения
передаются через штатное исключение/operation ledger; lifecycle bool не
становится error channel.

Timed mode сохраняет исходное правило: остановка строго после TimeGeneration.
Начало timed-session имеет отдельный native anchor, не меняющийся при repeat.
Публичный OperatingTime остаётся diagnostic/state в секундах. Явное внешнее
изменение этого state сохраняет прежнюю интерпретацию секунд через compatibility
ветвь; инвариантность этой произвольной внешней политики данным срезом не
обещается. Новые контролируемые эксперименты используют component-owned start.

Default очищает новые runtime caches; Begin/Fire создают их. Reset по-прежнему
ставит ResetDelay, и первый штатный Calculate запускает новую последовательность.
Дополнительного Calculate, Build или Reset learner нет. Структурная настройка,
storage physics, morphology, индивидуальные Rs/C и порядок обхода дочерних
компонентов Rdk не меняются.
Диагностические SampleStartTime/OneShotEndTime остаются абсолютными seconds.
Новые caches не являются полным checkpoint динамики Copy/XML.

Нужны повтор baseline12, extended clock/near-grid/timed-cutoff проверки,
learned/frozen transfer regression и normal/ASan, а также читаемые временные
отчёты с фактическими импульсами. Отдельные Delay и PatternDuration режимы
самого NPulseGenerator/NPulseGeneratorTransit требуют своего clock audit.
Изменённые реальные предъявления маркируются новым кодом/протоколом; это не
опровержение исторических августовских наблюдений.

## Фаза управления дочерними генераторами

Адресный B2 показал: integer-clock исправляет origin invariance записанных
серий, но ACalculate вызывается после children (Rdk UContainer.cpp). Первый
импульс поэтому опаздывает на tick, а немедленный Begin следующей пачки может
стереть последний high sample текущей. В реальном CSV2000Hz при Delay0
первый pulse имеет2 samples, последний только1. Это отдельный дефект от
roundoff; одной заменой double clock он не исправляется.

Для одинаковой частоты dataset, его ancestor chain и generator children
управление теперь выполняется в штатном ABeforeCalculate. Нативный scheduler
по-прежнему рассчитывает каждого ребёнка; вложенного Calculate нет. ACalculate
пропускает повторное управление. Для multirate source hierarchy сохраняется
прежний gated after-children путь: Before hooks в Rdk сами по себе не gated.
Это compatibility path, не объявление multirate поддержанным новым learner.
WasPlaybackHandledBeforeChildren — read-only diagnostic последнего tick.

Файловый NDataset подготавливает pending reload через PreparePlaybackData в
той же выбранной фазе. Invalid file/matrix выдаёт диагностику через native
exception. Unavailable остаётся idle с повторной попыткой. SyncGenerators
инициализирует новые/unready children при уже initialized owner до arming;
в cold Build сохраняется штатный Init. File reload и multirate требуют
отдельных native tests, наряду с повтором matrix20 и learned transfer14.

Новый fixture record schema2 объявляет origin_and_pulse_program_v1: фактический
phase, first onset, полную ширину каждого импульса, repeat и timed cutoff.
Record schema1 остаётся historical adapter с неизвестным полным набором
assertions. Проверки ещё не завершены; новый before-control код не принят.

Normal B3 подтвердил20 matrix-clock cases/76 recordings и14 transfer cases.
Последующий file lifecycle B4 воспроизвёл отдельный дефект: missing→load→growth
и shrink оставляли Ready=false после присваивания матриц в BeforeCalculate.
На следующем tick автоматический Build вызывал native AfterBuild→Reset,
повторно сдвигая начало предъявления. Первый pulse был21 вместо20 при2000Hz
и201 вместо200 при20000Hz; это реальный runtime failure, не исправление scorer.

Следующий кандидат выполняет reload как структурную операцию владельца:
Ready=false → native Build → ABuild/PrepareDataset/SyncGenerators → native
AfterBuild/Reset → проверка Ready/operation diagnostics → arming текущего tick.
File parser вызывается штатным ABuild, без второго предварительного чтения.
Unavailable остаётся retryable; ошибку Build нельзя замаскировать его bool=true.
Физика generators не записывается заново на каждом tick. Кандидат ещё требует
normal file2, native dispatch3, regression matrix20 и ASan; приёмки пока нет.
