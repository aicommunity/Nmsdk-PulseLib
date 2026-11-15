# Переносимое доказательство protocol5: первый блок package6

Исходная граница: Pulse `f7a0e3e63ec29d4647790a695606d1ca32164ef3`.
Это production DATA-компонент, а не включение package6, Train CLI или defaults.
`candidate_window_training_requires_package6` остаётся действующим.
Старый `CompositePortableTrainingProof` принимает только protocol4 и не меняется.

`CandidateWindowPortableTrainingProof::Replay` принимает закрытый
`DecodedNativeCompositeArchive` только schema2/protocol5. Создать такой архив
может существующий strict decoder: embedded primary обязательны, явно переданный
external resolver должен вернуть те же bytes; отсутствующий файл не заменяется
embedded-копией. Decoder повторяет native packet/context/source/model/reset/grid,
N+2 closure, полный waveform и basin geometry, actual frame projection,
proposal arithmetic/links, chronology, semantic IDs и retained budget ledger.
Новый proof не принимает public `valid`/`eligible`, raw scalar/packet3 или callback.

Полный original allN набор закрывается только через
`CandidateWindowPortableReferenceFactory::Replay`: actual reference plateau/P0,
original C0 при наличии, source/grid/storage/family и immutable reference proofs.
Из него строятся `CapturedReferences`; каждый raw target берётся из ACTUAL
full-before reference maximum. Current trained graph не заменяет initial targets.
Неполный reference prefix остаётся DATA без `References`, `WindowReferences`
и certificate. Полный reference set без полного selected final set даёт только
закрытые original targets.

Для final certificate нужны все N selected accepted final groups текущего attempt,
одного самого позднего OBSERVED final run, current base model/epoch и одного
candidate witness. Учитывается также поздний rejected/partial final record:
старый целый sweep не восстанавливается поверх уже наблюдённого нового.
Порядок ordinal берётся из актуального native namespace `/final/<canonical count>`
(включая разрешённый `/final/0`), не из лексикографического сравнения строк.
Trial serial соответствует положительному `NextRequest` serial, проверяется
в реальном Classic/Branch final order; duplicate groups/proofs/trials запрещены.
Recipe/measurement-instance IDs и protocol binding сохраняются в cert3.

В private FinalVerificationSession передаются только уже закрытые значения:
raw potential ACTUAL full-before peak и arrival/plateau relative к ACTUAL
protocol.originStimulusId именно этой группы. Absolute model time, delay from
assigned impulse, predicted peak и synthetic timestamps здесь не используются.
Общая private residual/Finish арифметика прежняя: CapturedPerInput, anchor N−1,
amp abs `1e-5`, relative `0`, sync из actual runtime (default `.02`). Proof не
расширяет tolerance. Публичные live Arm/Consume не вызываются; дополнительный
private friend не создаёт live authority или процессные prepared values.

`RequireCertificate` сравнивает полный пересчитанный cert3, включая content ID,
residuals/tolerances/source/family/reference/final proofs, current attempt/final run,
window recipe и measurement-instance IDs. Это **математическое DATA-сравнение**.
Archive не содержит authoritative конечный Service State/Outcome: даже Exact
из полных исторических измерений сам по себе не означает успешный Train.
Будущий package6 export обязан дополнительно требовать настоящий completed
service boundary и равенство его live certificate; stopped package хранит
diagnostic/null certificate. Non-Exact math certificate может существовать у
BudgetExhausted service и не становится Exact или completed state.

Proof не доказывает controller traversal, invalid/wall-budget history или native
execution; wall declarations не превращаются в elapsed attestation. Для
reference-only/partial DATA переданные current revision/epoch — caller boundary
context, а не независимо проверенный импортированный current graph. Будущий
package6 owner обязан импортировать/сверить actual physical archive, storage,
dataset/full mask/source и C0; восстановление новой live lease — отдельная
owned операция. Никакие адреса/callbacks не сериализуются.

Root actual tests используют существующие Environment/storage/scheduler Service5
fixtures, сохраняют source/after graphs, full/omit/repeat primary и proof companion.
Они проверяют Classic/Branch byte equality с live cert3, C0, разные absolute group
begins, default `.02` non-Exact, partial/merged/warm original targets, two actual
final sweeps, current model/epoch, rehashed foreign reference joins/claims,
missing primary и отказ protocol4. Они не доказывают N4 convergence, causal
decomposition nonlinear peaks, complete package6 import/export или Train→Test.
Локально разрешены только source/Git checks; compilation/numerical acceptance
этого нового компонента ещё не выполнены.
