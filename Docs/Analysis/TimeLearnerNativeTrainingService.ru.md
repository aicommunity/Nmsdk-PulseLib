# NativeTrainingService

Явные ConstructNativeModel/AdoptNativeModel и сохраняющий граф forced Build
описаны в [NativeModelBuilder](TimeLearnerNativeModelBuilder.ru.md).
Configured ControllerVersion=2 и полный trained-state codec ещё не реализованы.

Подэтап assigned-wave-selection добавляет явную PeakSelectionPolicy в ProbeSpec
и VerificationProtocol: прежний IsolatedWindow и opt-in AssignedBurstWave
только для schema 3. Назначенный выбор требует полной measured burst assignment
и реальных соседних samples вокруг плато внутри окна; missing/extra/clipped
не принимаются. References, Start и final verification связывают одну политику.
Evidence envelope 2 записывает её явно. Root
f2fe81f96d68859690ae56e4ef29f5c32c549bc7 прошёл 56/56 addressed C++ и 18/18
Python на каждом сервере без пропусков. Те же 56/56 C++ cases прошли
M40 ASan с detect_leaks=0; отсутствие утечек не заявляется.
Актуальный статус — корневой Docs/StructTrain/TimeLearner/AssignedWaveSelection.ru.md.

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

Root 2fd45d4424360d7834ba094f875ddeda678c1625 прошёл 65 адресных cases
на каждом сервере и 8 M40 ASan, detect_leaks=0. После сохранения полных
reference observations при Reset/warm: root
27068892e86095e67feb2a4bf904ca5171c090d1 прошёл 31/31 на каждом сервере.
Диагностический EncodeNativeTrainingState с конфигурацией, параметрами,
кандидатами и остатками проверен в root
90d11d1990221753e6b494dbf40d83964b7d0fa3: 39/39 на каждом сервере.
Это summary, не checkpoint/import. Root audit module проверил 56 полных
observations/CSV и четыре отрицательных сертификата на каждом сервере.
12 corruption/data tests прошли на каждом; evaluation verdict не создаётся.

Актуальный журнал, области
приёмки и оставшиеся задачи — корневой Docs/StructTrain/TimeLearner/
NativeTrainingService.ru.md. C-trim, PostTune/calibration, полный trained-state
transfer и конфигурационный dispatch ещё обязательны. Native builder реализован
отдельным API-подэтапом; это не завершает всю production mode matrix.
