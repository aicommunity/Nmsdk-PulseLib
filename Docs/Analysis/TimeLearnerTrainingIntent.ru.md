# Исходный запрос Train и физическая граница finalization

NativeTrainingRequest() обоих TimeLearner возвращает value-copy исходного запроса
успешно начатой native-сессии: публичные параметры/их источники, run/session,
initial model revision, epoch, protocol/pattern hash и время начала. Снимок
формируется по controls, прочитанным перед Start, и содержит фактические session
поля после Start. SHA вычисляет production canonical serializer.

Позднее изменение controls не переписывает снимок. Это provenance исходного
запроса, не разрешение игнорировать текущий внешний stop или новые настройки.
Будущий dispatcher должен сравнить original/current controls перед переходом.
Сейчас изменение controls только отражается отдельно; автоматическое выполнение
финальных фаз ещё не подключено, optional guards сохранены.

Mode 0 может штатно снять локальный Need при завершении Train; начальное true
остаётся в intent. При обычном подключении Rdk SetDataDirect не пишет в source
и не снимает внешний true. Mode 1 сохраняет внешнее управление. Снимок сохраняет
исходный true после external stop, не приписывая такому останову сходимость.

Explicit Reset, Default, retarget, новый Start и успешная замена графа очищают
предыдущий intent. Неизменённый target и отклонённый import сохраняют его.
Внутренний Reset под NativeLifecycleDepth не стирает запрос. Intent не входит
в существующий versioned model export/import: восстановленный исторический
Solo не получает выдуманный train-time запрос. Возобновление intent через Git
или model checkpoint пока не реализовано; отдельный физический boundary остаётся
обязательным. Copy не является checkpoint.

Нативные сценарии добавлены для Classic/Branch × CalculateMode 0/1 × local/
connected request: старт, поздняя смена fraction, честный BudgetExhausted,
Mode-0 completion, отклонённый import, Reset без изменения learned parameters,
новая сессия, внешний stop, no-op target и retarget. Адресные серверные проверки завершены (см. ниже).
Это control-only пробы с малым declared budget, не независимое успешное Train и
не all-pattern recognition; завершённые waveform-ответы не выдумываются.


## Проверки 31.10.2026

Native root 6062fb58c4324a61da0e6346e77a5e8fb53e31ee /
PulseLib 86cd84326419d8d14f59a05a8d182450322fd903: normal 61/61 (50.707 s),
ASan 61/61 (316.845 s), без skips; ASan leak checking выключен. Восемь новых
request lifecycle controls, 20 existing connected-stop controls и 33 boundary/
lifecycle/codec/Solo regressions. Отдельные successful graph replacements с
заранее непустым intent пока не проверены; код очистки в этих entry points
реализован, но общий import regression не заменяет такую адресную проверку.

Независимый reader root f0278fba6733be92b4560bf31c719f516bae73c6:
normal 2/2 (0.040 s), ASan 2/2 (0.033 s), по 24 snapshots / 8 records и
32 re-signed corrupt controls на worker. Reader проверяет сохранённые значения,
схему, bindings и SHA. Промежуточные native assertions о Reset/stop/import
проверены нативными тестами, а не выдуманы из трёх JSON-снимков; отдельная
execution attestation не заявляется.

Корневые proofs s08-native-training-intent-normal-a1 и
s08-native-training-intent-asan-a1: по 321 members; 941804 / 954948 compressed
bytes, все hashes проверены. Включены полные исходные portable fixtures /
84 artifacts для genuine Solo regressions. Immutable runtime before/after
подтверждены внутри задания, предыдущий завершённый runtime использован повторно.

Configured phase dispatcher, сравнение original/current controls при переходе,
сохранение intent в model checkpoint и вся приёмка этапа 8 остаются открыты.


## Подключение к finalization boundary: runtime ожидается

CheckNativeTrainingIntent вызывается production CaptureNativeFinalizationBoundary
до экспорта Solo. Проверяются session/run/protocol/pattern binding, исходные
публичные controls и все их источники. Из сравнения значений исключён только
живой IsNeedToTrain: mode-0 local completion и внешний stop могут снять его,
источники сигнала остаются неизменными. Отдельные stopped/active-entry правила
не ослабляются. Изменение fraction, режима или переподключение source запрещает
захват; сам исходный запрос не переписывается и модель не изменяется.

Boundary с настоящим train-time request получает schema 2 и хранит полный intent;
legacy imported Solo без запроса продолжает schema 1. Во время capture проверяется
также неизменность самого intent. Это связывает границу с исходным запросом,
но не добавляет автоматический dispatcher или mid-probe checkpoint.

В восемь native lifecycle cases добавлены фактическое отклонение drift,
schema-2 diagnostic stopped boundary и успешный import при непустом intent.
Независимый reader поддерживает исторический schema 1; новые tests требуют schema 2
и проверяют source/session/control binding и запись успешного import. Серверные
проверки этой новой связки ещё не выполнены.
