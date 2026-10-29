# Frozen recognition owner API

Рабочая ветка `stage08/frozen-recognition-owner`; Core
`0b9d0ff992238b6f72bc1046d50c5d80fecee6d7`. Новые методы обоих learner:
ImportNativeRecognitionState, ExportNativeRecognitionState, HasNativeRecognitionState.
Новых UProperty defaults либо неявного выбора ControllerVersion=2 нет.

Import готовит и проверяет частный recognition граф из immutable solo package,
проверяет совместимость owner metadata/control values с внешними источниками
до замены детей, затем использует общий native CommitNativeLearnerChildren.
Prepared state сохраняет исходную конфигурацию/режимы. Muted solo baseline
для recognition выключен; порог наследуется из проверенного physical archive.

В frozen состоянии Before/AfterCalculate проверяют действительные параметры,
связи, порядок, cached native state и частоту через ModelRevisionObserver;
детей рассчитывает штатный scheduler. Training service очищен и не содержит
recognition certificate. Source solo proof остаётся внутри экспортируемого
пакета в source-only scope. Наличие frozen state не означает качество распознавания.

Build/Init/Reset сохраняют модель, Default завершает scope. Неявный возврат к
training, solo export, training probe и parameter edit запрещены. Повторный
import загружает тот же архив, не масштабирует live Rs повторно. Для нового
обучения требуется явный cold construct либо успешный solo import.
Ошибка физической/owner проверки доступна через NativeModelEditFailure;
Build/Reset/Calculate сохраняют bool-контракт Rdk.

## Проверенный scope

На Elder normal и M40 ASan (detect_leaks=0) A1 root6ff1d43a: по 8 owner cases
и 4 cold-training regressions, 12/12 pass. A2 root
`112acdae1c21bb6e0b1667b3fffdeb90a02859b1`: по 12/12 owner cases без skips/failures.
Охват: Classic/Branch N0/N1, historical scaling/clipping, N=1/2/6, configured V2,
CalculateMode=0/1, Init/Reset/repeat import, forged certificate и прямое изменение
физики. Six-input control использует explicit sync tolerance 1 s и фиксированную
геометрию; это проверка размерности, не default convergence.

Strict independent A3: по 8/8 Python tests без skips, 164 source trials и
5052 paired samples. Schema-2 owner observation хранит actual before/after
hashes обеих моделей. Новый recognition-owner анализ использует общий conversion
и paired-waveform auditor, без восстановленных из ожидания измерений.
Первый A2 Python запуск пропускал CLI class из-за отсутствия domain fixture;
A3 передаёт его явно и запрещает skips. Исходный пропуск сохранён.

Root reports s08-recognition-owner-elder1 (aa75bc2d82d2655aa0da4080fe896267645f3072)
и s08-recognition-owner-m40 (8b9febb3ce18548a4644d57aad89b10d0a1d4552):
на каждом 912 stored / 910 raw hashes проверены локально. Native Core одинаков
для A1/A2. A1 prototype schema 1 не содержит полного hash readback; его поля
не достраиваются, отдельно проверены 8 source packages / 108 trials.

Текущие controls меняют training learner на recognition state и сравнивают
его со вторым независимо импортированным physical graph. Новый destination
learner и fresh-process owner требуют отдельных controls. Полный transient
foil runner, реальные spike timestamps, threshold confirmation и independent
scientific evaluation ещё не реализованы. RecordSpikes=false, scientific
verdict и recognition certificate — null. Этот подэтап не закрывает весь этап 8.
