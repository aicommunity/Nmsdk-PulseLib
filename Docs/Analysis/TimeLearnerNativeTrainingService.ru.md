# NativeTrainingService

BeginNativeTraining у обоих TimeLearner запускает общую последовательность
захвата per-input references, ограниченного поиска и независимой свежей
финальной проверки. Before/After hooks штатного scheduler вызывают service и
native probe runner; вложенного расчёта физики нет. Edit выполняется через
NativeModelEditor между пробами, с preview и effective readback.

Это рабочий API-подэтап, ещё не configured ControllerVersion=2. Default и
старые конфигурации сохраняют legacy-ветвь до явного BeginNativeTraining.
После явного начала старое обучение не возобновляется между пробами или после
ошибки. Reset очищает временное управление, сохраняя модель и goal; Default
освобождает native-владение. Retarget очищает goal и certificates. Warm restart
требует прежнего протокола и точного последнего model hash.

Отдельные model/wall budgets и external stop не производят Exact. Все reference
и search observations проходят общий validator; отрицательный final certificate
содержит остатки с Exact=false. CalculateMode 0/1 не заменяют typed outcome.

ProbeEvidenceJson кодирует полный internal ProbeSpec/Observation, контекст
подготовки, фактические события, все пики и назначенные волны. Requested spec
сохраняется также при отмене до подготовки. Nonfinite JSON measurements
отклоняются. При recordSpikes=false spike count остаётся unknown, не нулём.
Native executor проверяет hash фактической Dataset ISI-матрицы.

Серверные проверки этого подэтапа ещё выполняются. Актуальный журнал, области
приёмки и оставшиеся задачи — корневой Docs/StructTrain/TimeLearner/
NativeTrainingService.ru.md. C-trim, PostTune/calibration, полный trained-state
transfer, новые builders и конфигурационный dispatch ещё обязательны.
