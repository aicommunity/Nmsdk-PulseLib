# Native PostTune boundaries: проверенный граф и рабочая owner policy

NativeLearnerGraphCheckpoint (NativeModelBuilder) сохраняет immutable snapshots
Neuron/DatasetMatrix и восстанавливает оба дерева через native import, связывая
нейрон с новым Generator1. Старые объекты сохраняются до проверки подключения;
attach failure возвращает текущий speculative graph. Snapshot привязан к owner,
storage/environment/frequency и native lifecycle. Это completed boundary,
не mid-probe checkpoint и не возврат старого clock.

PulseLib 32dc2e0223cab8adca8280eb1c4586764b6a9c86, root
325052a0802e431f45ed91619c089fbc0eef7922: normal Elder и ASan M40 — по 14/14
native tests. Исходный normal A1 дополнительно: 24 native / 10 Python,
16570 exact waveform pairs. Owner Reset failure воспроизведён на pre-guard
baseline; научная причинность для исторических экспериментов не утверждается.
A1 ASan был намеренно прерван SIGTERM; его частичные данные не являются PASS.
После восстановления связи independent A2 завершён на обоих workers:
по 2 Python tests, 16570 exact sample pairs, 20 Elder / 8 полных M40 A1 CSV
совпадают с A2. Пакеты s08-boundary-elder1 и s08-boundary-m40 опубликованы в root
Git; локально сверены 426 stored / 424 raw и 241 / 239 hashes соответственно.

NativePostTuneOwnerBoundary — более поздняя реализация PulseLib
`c7266dcb93ec51a765125bc30647cf1b59129c91`, native root
`912ef71b36fbc040c6177a82070a796ee5eb99de`. На Elder normal и M40 ASan
завершены по 23/23 native cases: девять owner-policy и 14 graph/guard regressions.
Это отдельный source scope, не результат старой A2 проверки. Поддерживает только
native-owned idle scope: source training controller не копируется/сбрасывается,
проверяются его identity, goal, declaration/configuration и lifecycle. После
восстановления графа обновляются private child aliases и адресно возвращаются
owner threshold policy, L/Ns/Rs/bounds и loaded-value bookkeeping. Setter не
вызывается, связанных или external owner properties быть не должно: в Rdk
SetDataDirect может писать в external data source. Retarget не перезаписывается.

Шесть положительных случаев охватывают Classic/Branch N0/N1 при 2 кГц и N1
при 20 кГц, изменение параметров/threshold policy, возврат исходного состояния и
native waveform. Три отрицательных — connected scalar, retarget и намеренная
подмена reference trial IDs при равном certificate/state/epoch. Последняя
является corruption control над настоящими packets, не новыми наблюдениями.
Fixture объявляет sync tolerance 1 с: она проверяет boundary, а не сходимость
при default 0.02 с. При CalculateMode=1 управление снимается внешним setter
после Exact; при CalculateMode=0 проверяется штатное автоматическое завершение.

Independent Python A1 664ced234 и A2 54b3d5607: по 4/4 tests на каждом worker,
42 actual training packets, 8646 waveform sample pairs. A2 дополнительно
сверяет sample-count summary с первичными CSV, без нового native расчёта.
Проверены production audit_probe, привязка final trial IDs к certificate и
byte-exact replay. Полный training summary с повторным выводом всех условий
сходимости этим fixture не экспортируется и не объявляется проверенным.
M40 использует ASan detect_leaks=0. Публикация и текущие full regression gates
отражаются в поддерживаемых root документах; full-source приёмка пока открыта.

Это не готовый общий PostTune controller и не полный legacy checkpoint.
Candidate history/selection/commit, real-threshold calibration, frozen independent
Test и production wiring остаются открытыми. Source solo certificate не становится
сертификатом recognition-кандидата. Uniform physics по-прежнему задаётся storage
образами; Restore не создаёт parent overrides и не вызывает nested Calculate.

Подробный root документ: Docs/StructTrain/TimeLearner/PostTuneBoundaryTransaction.ru.md.

## Полная интеграционная проверка 29.10.2026

Root 912ef71b36fbc040c6177a82070a796ee5eb99de / PulseLib
c7266dcb93ec51a765125bc30647cf1b59129c91: normal Release на Elder-1 и M40
по 491 native cases, 480 passed / 11 helper-role skips / 0 failures; три
выбранные Rdk CTest groups pass. Независимый Python source
54b3d56073ebbe9e2cc3ef9edacb5085bf817293: по 114 tests без skips/failures/errors.
Полные пакеты опубликованы в root Reports/StructTrain/TimeLearner:
s08-integrated-elder1 (bc37077f18adce02c9d90b2aecdc2b75fc8e6b4f),
s08-integrated-m40 (182191b9534f62257687b97b24f9f7fc936dd71e).
По 3268 stored / 3266 raw hashes проверены локально.

Этот gate разрешает приёмку указанного source scope; production PostTune
candidate loop, полная controller/threshold transaction, frozen recognition
и весь исходный план не завершены. Позднейшие owner metadata guards и Rdk
BOM/config изменения в этот native source не входят.
