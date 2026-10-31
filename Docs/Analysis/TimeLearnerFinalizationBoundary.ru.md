# Неизменяемая граница Train → finalization: адресные проверки

NativeFinalizationBoundary хранит значения, а не ссылки на owner/storage или
checkpoint динамики. CaptureNativeFinalizationBoundary читает настоящий экспорт
Solo, исходные phase parameters и фактические связи параметров (путь provider,
имя свойства, порядок sources, connected/external kind). Getter не пишет во
внешний источник и не вызывает Build/Init/Reset/Calculate исходного owner.
Экспорт штатно создаёт и инициализирует отдельные rehearsal/image instances;
это не прозрачный checkpoint динамики и не отсутствие любых storage-операций.

31.10.2026: root a282fe9a56cd49c09bc81da128b2f1e4c8283417,
PulseLib f8929f906d24f8824450b9542445bcb9016f9337: normal 24/24 (20.492 s),
ASan 24/24 (128.163 s), без skips; leak checking выключен. Сравнение сохраняет
native XML целиком и допускает только наблюдавшийся переход InputFeedbackSignal
NPMembraneBio с пустой матрицы в нулевую 1×1. Причина этого перехода в native
allocation/reuse пока не доказана. Независимый reader проверяет capture/replay и
отклоняет 16 corrupt controls на каждом worker. Это не training evidence audit,
не scientific verdict и не завершение configured phase dispatcher.
Подробности и первичные доказательства — в корневом
[NativeFinalizationBoundary](../../../../Docs/StructTrain/TimeLearner/NativeFinalizationBoundary.ru.md).

Для этих replay опубликованы четыре temporal reports, 12 complete trial-soma pairs,
все target/foil и soma channels, с реальными входными импульсами. PNG 1920×960,
detail ≥8 pixels/native tick; 3/3 server report checks, без skips. Полный cache
на M40 Data; в Git компактные metadata и четыре выбранных PNG. Выбранные
Classic/Branch target/first-soma overview/detail просмотрены; это не visual
acceptance всех foil/других somas/connected figures или всего cache. Научного
PASS и реализации configured dispatcher этим не заявляется.

## Active request import: новая реализация, runtime-проверка ожидается

ImportNativeFinalizationState принимает явный NativeFinalizationBoundary,
захваченный при завершённом Solo и активном request. Перед импортом повторно
проверяет исходную модель, весь XML (кроме указанного feedback cache перехода),
training outcome/evidence, controls и bindings. Stopped boundary запрещён.
Recognition candidate обязан происходить из точного frozen source_solo.

В managed hold Need остаётся true, CalculateMode и исходные PostTune/AutoCalibrate
не подменяются frozen mirrors. Read-only policy сохраняет значения и sources
всех controls; изменяться могут только три threshold mirrors. Снятие фактического
Need даёт native_finalization_external_stop, включая connected provider без
вызова setter. Policy ничего не пишет в provider и не считает stop сходимостью.

ReplaceNativeRecognitionState сохраняет текущий role/intent при import кандидата
и rollback; общий NativeRecognitionTransactionScope использует этот путь.
Обычный ImportNativeRecognitionState остаётся явным переходом к frozen policy:
Need/PostTune/AutoCalibrate=false. NativeFinalizationIntent возвращает копию
исходного immutable document; model export не подменяет его поздними mirrors.

Новые адресные тесты: Classic/Branch × CalculateMode 0/1 с настоящим connected
request, полным fixed PostTune, различными measurement/final thresholds,
отклонением чужого source, внешним stop и явным frozen import. Старые frozen
контуры проверяются отдельно. Эта реализация ещё не подключает configured
phase dispatcher, не определяет target-reference/mid/parallel-peak протоколы
и не снимает guards optional modes. Не считать её полной приёмкой этапа 8.

Допустима только ready/initialized Solo-модель с Finished training service и
полностью освобождённой границей probe: ни queued/running, ни непотреблённого
результата/физического routing state. NativeProbeOwnsControl сам по себе этого
не доказывает; новый IsNativeProbeAtEditBoundary учитывает состояние executor.
Lifecycle всех живых descendants, session/epoch/outcome и значения/связи controls сравниваются до и
после экспорта. Исходный training outcome/certificate сохраняется без повышения.

Stopped package по умолчанию отвергается; явный allowDiagnostic сохраняет
его как diagnostic_only с null certificate. Это не разрешение conversion или
продолжения обучения посреди пробы. Class value не содержит Rdk pointers и
должен переживать recognition import, Reset/Default и teardown реального owner.

В частности, независимо сохраняются AutoCalibrate, PostTune, Mid и silent
probe threshold; foil count не превращается в бюджет weight candidates.
Legacy Branch parallel-peak path отмечается отдельно даже при AutoCalibrate=false.
Public mode enums переводятся явным switch: legacy CanonRmin=1 и KeepDone=3
не совпадают с порядком NativePostTuneControllerMode. Неактивные параметры
сохраняются как декларация; неизвестный используемый enum отвергается.

Реальный InstallNativeRecognitionState не просто очищает NativeConfiguredPlan:
он заменяет его восстановленной cold configuration из Solo. Training service
заменяется пустым, а VisitNativeRecognitionControls зеркалирует Need/PostTune/
AutoCalibrate=false. Поэтому исходная phase declaration не восстанавливается из
последующих live mirrors. Внешний stop через connected property нельзя получить
проверкой setter: UProperty может пропустить setter при равенстве значения либо
получать значение из provider. Это требование к будущему phase dispatcher.

Здесь реализован только захват независимой границы. Он ещё не установлен в
Calculate/Reset hooks, не снимает guards optional modes и не заявляет поддержку
target-reference/parallel-peak/mid dispatch. Родительскую модель следует Init/Reset
до импорта завершённого learner: последующий явный parent Reset штатно сбрасывает
контроллер. Не добавлять обходные «будящие» Build/Reset ради сохранения checkpoint.

Приёмка: реальные Classic/Branch source packages, source bindings, queued probe,
external stop и распознавание полного объявленного набора после import; неизменность
снимка после Default/teardown; отсутствие зависимости от порядка public enums.


Снимок фиксирует finalization intent именно в момент Capture, а не историю
изменения параметров во время Train. Будущий dispatcher должен отдельно
замораживать декларацию при Start и различать собственные переходы controls.
Нельзя поднимать IsNeedToTrain после восстановления Finished ради тестового
снимка: для ControllerVersion=2 это штатный запрос новой сессии/ResetControl.
Connected provider может иметь true без вызова setter; этот реальный случай
проверяется отдельно. Принимается фактический request value, а не желаемый.
