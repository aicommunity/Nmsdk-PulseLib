# Native threshold candidates и измеренное предложение порога

`Core/TimeLearner/NativeThresholdCandidate.h` создаёт immutable candidate из
полного `timelearner_recognition_state`. Единственное разрешённое физическое
изменение — реальный `LTZone.Threshold`. Образы storage, topology, bindings и
solo proof сохраняются. Вложенные candidates, nonfinite порог
и подменённые records отклоняются. Import заново выводит ожидаемую модель
через native archive path; дополнительный Calculate не вызывается.

`NativeRecognitionTrialRequest.recordThresholdPotential=true` включает schema 2:
каждый отсчёт содержит фактический `LTZone.Potential` после штатного scheduler.
Источник описан path/class/sample_phase/units. SI units не подтверждены.
Существующая schema 1 при выключенном флаге не меняется. Внешний source у
Potential и nonfinite значения отклоняются. Soma.SumPotential и LTZone.Potential
не подменяют друг друга.

`ProposeNativeThreshold` принимает явно ожидаемый сбалансированный набор
завершённых nonspiking проб, исходный training pattern hash, response window
и допустимые bounds. Нижняя граница учитывает весь foil window и запрещённые
target-outside ответы, верхняя — минимальный target peak внутри response window.
Результат содержит первичные observation digests, extrema и причину Proposed,
NoMeasuredGap либо NoRepresentableThreshold. Это предложение, а не калибровка:
обязательны реальные spiking search и отдельно rebuilt confirmation пробы.

Изменение порога не переносит solo certificate Exact на recognition model.
Candidate metadata помечает `calibrated=false`; source graph не меняется.
Повторный import восстанавливает тот же порог, не выполняя повторного conversion.
Используется штатная idle boundary: перед заменой модели необходимо завершить
пробу и прочитать её результат. Busy guard нельзя обходить через Reset.

На 31.10 выполнены адресные 2000-Гц пробы Classic/Branch и независимые первичные
проверки порогов; полная 2/20-кГц normal/ASan матрица ещё проверяется. Controller
всех PostTune modes, calibrated-state export и independent frozen evaluation
не объявлены готовыми. Новый dispersed two-pattern контроль не является
историческим августовским результатом. Подробные версии, failed attempts,
CLI select/confirm и compact artifacts: корневой документ
`Docs/StructTrain/TimeLearner/NativeThresholdCalibration.ru.md`.

Рабочее усиление следующего подэтапа: общий
`ValidateNativeRecognitionObservation` сверяет записанную request с объявленной,
число и точную native grid отсчётов, закрытие окна, finite soma/stimulus/LT values,
покадровые stimulus/spike histories и итоговые event arrays. Данные continuous
stream до начала окна не становятся событиями текущей пробы. Предложение порога
теперь вызывает эту проверку до использования экстремумов. Изменение требует
отдельного серверного gate; прежняя приёмка Core 7f17735 его не подтверждает.

`AssessNativeThresholdResponses` оценивает закрытую заранее объявленную матрицу
одной модели/частоты на одном clock. Порядок recorded rows не влияет на ответ;
порядок предъявлений сверяется с expected requests и реальными временами.
Для target требуется spike внутри response window, а при соответствующей
политике запрещены ранние/поздние spikes. Для foil проверяется всё окно.
Сохраняются TP/FP/FN/TN, каждый spike, нарушения target и отдельные reset policies.
Silent target даёт FN, а не успешное распознавание. Schema-1 и schema-2 данные
проверяются одним validator. Измеренная готовность не означает calibrated commit
или training certificate. Независимый Python scorer должен подтвердить native
assessment перед приёмкой нового контура. Первый scoped 2000-Гц gate пройден;
полный gate и independent readback ещё не приняты.

## Контракт знака и фактическая реализация LT-зоны

Первоначальный `explicit-native-threshold-v1` разрешал только положительные
пороги. Это ограничение не следует из NLTZone и противоречит штатным storage
образам `NPulseLTZoneIaF`/`NPulseLTZoneCable` с Threshold=-0.055, ThresholdOff=-0.07.
Рабочий v2 допускает любой конечный порог, включая ноль и отрицательные значения;
v1 packages по-прежнему читаются с исходным positive-only контрактом. Bounds
`nonspiking-native-ltz-bracket-v2` также могут быть отрицательными. Baseline
experiments сохраняют явно объявленный диапазон 1e-6..1.

Проверка scalar spike threshold использует фактический C++-тип. Storage image
`NPulseLTZoneIzhikevich` в CreateClassSamples создан через `new NPulseLTZoneThreshold`,
а не через одноимённый C++ класс. Bio/IaF/Cable images также являются образами
Threshold с другими параметрами. Поддержаны stock Threshold и leaky NPulseLTZone;
NPSimpleLTZone кодирует частоту, continuous LT-зоны не дают требуемого порогового
spike-контракта. Для них новый candidate/probe выдаёт явную диагностику, а не
трактует любое поле Potential как operand сравнения. Для иных C++ реализаций
нужен проверенный адаптер. Ordinary schema-1 frozen observations и legacy v1
package load этим не отключаются.

Signed normal gate root `0afbf97c4ba9df4591eebe9a41fb916e02d1903e`, PulseLib
`479084f4c35d61f60ef9b37eb0da246bc8dec87a` прошёл на Elder: 6/6 native
и 6/6 independent Python checks без ошибок и skips. Первичные данные находятся
в корневом `Reports/StructTrain/TimeLearner/s08-threshold-contract-normal-a2`.
ASan и полная повторная матрица остаются отдельными проверками. Проверка отдельной
LT-зоны не является проверкой структурного обучения всей IaF/Cable модели.

## Native чтение сохранённых наблюдений

`DecodeNativeRecognitionObservation(object, expected)` и
`ParseNativeRecognitionObservation(text, expected, maximumBytes)` восстанавливают
только полное наблюдение, пригодное для проверки калибровки. `expected` берётся
из отдельно объявленного дизайна: duration не восстанавливается вычитанием end
и begin, особенно на большом абсолютном времени. Поддерживаются существующие
схемы наблюдений 1/2 без изменения их записи.

Чтение проверяет типы, finite значения, лимит samples, точное обратное кодирование
всех полей и общий native validator. Неизвестное поле, расхождение относительных
timestamps/counts/descriptor, неполное окно и несогласованность событий с histories
отклоняются. Текстовый вход использует strict parser с лимитом байтов и проверкой
дублирующихся ключей. Диагностические censored observations остаются историческими
артефактами и не превращаются в доказательство калибровки.

API не вызывает Calculate/Build/Reset, не меняет модель и не присваивает
calibrated=true. Это необходимый путь повторной проверки сохранённых первичных
данных для будущего calibrated model import, а не готовый calibrated export.
Серверная проверка этой добавки ещё требуется отдельно от предшествующих gates.

## Калиброванное recognition state: реализация, ожидающая проверки

`CreateNativeCalibratedState` принимает immutable threshold candidate, объявленный
протокол/partition/attempt/clock, criterion и две полные матрицы — search выбранного
кандидата и свежее confirmation. Обе матрицы должны пройти общий native assessment
на одной физической revision/frequency, с одинаковыми sample/pattern/reset/duration
и в одинаковом порядке. Run IDs различаются; confirmation начинается строго после
всей search matrix. Невалидная, молчащая target или повторно использованная матрица
отклоняется. Поддержаны historical_same_matrix и frozen_independent_calibration;
само название второго протокола не доказывает статистическую независимость.

Пакет `timelearner_calibrated_recognition_state`, schema 1, хранит source candidate,
полные requests/raw observations обеих матриц и пересчитанные assessments.
Изменяется calibration metadata, но не физическая model revision, storage images,
Rs/C, топология или solo certificate. Recognition verification остаётся unverified,
recognition certificate — null. `grid_selection_verified`, `execution_attested`
и `statistical_independence_verified` явно false: это подтверждение выбранного
кандидата, а не скрытая проверка всей сетки, native execution attestation или Test.

`PrepareNativeRecognitionState` диспетчеризует новый формат. Import повторно читает
raw observations по сохранённым requests, пересчитывает обе оценки и весь expected
package; подписанный заново, но изменённый claim не принимается. Только после
этого штатный native archive import строит модель. Внутренний calibrationConfirmed
отделён от значения порога: нулевой порог не означает отсутствие калибровки.
Legacy numeric mirror CalibratedFixedLTZThreshold отражает подтверждённый порог;
статус следует читать из проверенного пакета, а не сравнивать mirror с нулём.

Proof format ограничен 10000 requests на матрицу, 10000 ISI на паттерн и 10000000
maximum_samples на request; превышение не округляется и не обрезается.
Сохранение не является mid-trial checkpoint. Владелец устанавливает пакет только
на idle boundary через существующий transactional native child import.
Добавлены Classic/Branch 2000-Гц controls import/export, Reset/Build, frozen probes,
отказа повторных/плохих проб и rollback при повреждённом claim.

Normal snapshot root `0f53ab4ec420b921f766cbd78a7fd10d248b8a86`, PulseLib
`ce7e81e002c802412ce50c1f2b2f76ffa7215b2a` прошёл 2/2 native cases и 9/9
independent checks: Classic/Branch N1 при 2000 Гц. Данные и обе версии моделей
сохранены в корневом `Reports/StructTrain/TimeLearner/s08-calibrated-normal-a2`
(581 files, 1797464-byte archive, commit 9746ea4ba908b3030762fd6c276809142f27b8ce).
Reader/CLI calibrated-state и обычный frozen scorer проверяют proof и новые
observations; они не подтверждают native execution или independent Test.

M40 визуальные отчёты root ff432f559 прошли 3 report и 5 plotting checks.
Полные 12 trial/soma pairs остаются cache; выбранные PNG/metadata опубликованы.
В данном двухпаттерновом контроле reset-per-trial пары удовлетворяют criterion,
но preserve-dynamics target даёт outside spike и FN (общие counts 3/4).
Order/position и reset policy здесь не разделены, причинного вывода об одном
reset нет. Source solo proof не превращён в recognition certificate.

ASan всего calibrated API, полный grid/controller transaction, PostTune,
остальные modes/profiles/frequencies и independent Test остаются открытыми.

## Обновлённая scoped приёмка 31.10

Normal и ASan на снимке ce7e81e прошли persistence 2/2, calibrated-state
2/2 и независимый reader 9/9. Первичные source CSV, модели, raw proof и
полные manifest доступны в корневых bundles s08-calibrated-normal-a2 и
s08-calibrated-asan-a2. Последний содержит 1171 файл в 3093212-byte архиве.
Полный normal signed gate: 16 native + 35 independent checks; исправление
glob в reader не потребовало повторного Train. Более ранние статусы выше
относятся к своим снимкам и не заменяют эту сводку.

На normal/ASan frozen данных report/plot проверки 8/8 прошли. Git хранит
выбранные рисунки и compressed primary; полные читаемые графики — M40 cache.
Исходное обучение обеих двухпаттерновых моделей имеет search_unresolved.
Калибровка reset-per-trial пары подтверждена; последующий набор даёт TP=1,
FN=1,TN=2,FP=0. Порядок и reset policy смешаны, причинный вывод недопустим.
Это не независимый Test, не August replay и не завершение этапа 8.

При ревизии native owner import найдено ещё одно сохранение QJsonValueRef
из временного объекта в Qt 5: чтение calibration.threshold могло обращаться
к уже уничтоженному QJsonObject. Теперь сохраняется owning QJsonValue.
Это исправление времени жизни C++ объекта; отдельный runtime gate нового
снимка обязателен. На прежние успешные gates как доказательство этого
исправления не ссылаемся.
