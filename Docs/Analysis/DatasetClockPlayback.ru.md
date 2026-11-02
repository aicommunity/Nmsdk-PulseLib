# Время предъявления NDatasetBase

Исправление требует серверной приёмки. Оно относится к штатному burst playback
NDatasetMatrix/NDatasetFile через NDatasetBase, а не меняет задачу обучения.

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
storage physics, morphology, индивидуальные Rs/C и computing order не меняются.
Диагностические SampleStartTime/OneShotEndTime остаются абсолютными seconds.
Новые caches не являются полным checkpoint динамики Copy/XML.

Нужны повтор baseline12, extended clock/near-grid/timed-cutoff проверки,
learned/frozen transfer regression и normal/ASan, а также читаемые временные
отчёты с фактическими импульсами. Отдельные Delay и PatternDuration режимы
самого NPulseGenerator/NPulseGeneratorTransit требуют своего clock audit.
Изменённые реальные предъявления маркируются новым кодом/протоколом; это не
опровержение исторических августовских наблюдений.
