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
Independent A2 audit и Git-публикация raw data ожидают восстановления jump host.

NativePostTuneOwnerBoundary — более поздняя рабочая реализация. Она ещё не
собрана/проверена и не покрывается результатами выше. Поддерживает только
native-owned idle scope: source training controller не копируется/сбрасывается,
проверяются его identity, goal, declaration/configuration и lifecycle. После
восстановления графа обновляются private child aliases и адресно возвращаются
owner threshold policy, L/Ns/Rs/bounds и loaded-value bookkeeping. Setter не
вызывается, связанных или external owner properties быть не должно: в Rdk
SetDataDirect может писать в external data source. Retarget не перезаписывается.

Это не готовый общий PostTune controller и не полный legacy checkpoint.
Candidate history/selection/commit, real-threshold calibration, frozen independent
Test и production wiring остаются открытыми. Source solo certificate не становится
сертификатом recognition-кандидата. Uniform physics по-прежнему задаётся storage
образами; Restore не создаёт parent overrides и не вызывает nested Calculate.

Подробный root документ: Docs/StructTrain/TimeLearner/PostTuneBoundaryTransaction.ru.md.
