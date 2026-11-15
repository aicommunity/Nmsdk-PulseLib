# Общий scalar acceptance: mechanical preparation для typed Train

Отдельный commit от Pulse c3ac0aa сохраняет существующую математику и admission
ProbeObservation schemas1–3. BoundedTrainingSession после прежних проверок
вызывает private AcceptValidatedScalar; весь прежний tail с anchor/residual,
N0 repeats, ranking, cost, candidate order и AdvanceInput перенесён точно.
FinalVerificationSession использует private scalar-record и input-finish
helpers. Finish с all-input residuals, anchor N−1 и tolerances не изменён.

Новых public scalar/eligible bool входов нет. Context/schema/anti-replay checks,
trial insertion, invalid/retry budgets и порядок их выполнения остались на
прежнем месте. Старые ProbeObservation и CapturedPerInput значения не получают
нового смысла. В этом commit ещё нет composite measurement, service composition,
protocol/config/package новых версий или включения нового метода в defaults.

[Static inverse receipt](TrainingScalarAcceptanceExtractionVerification.json)
подтверждает точное восстановление исходных cpp после обратной подстановки
извлечённых helpers, а также hashes admission/residual blocks. Это текстовая
проверка, не runtime acceptance. Обязательны существующие FinalVerification и
BoundedTraining regression cases на сервере; native/Train integration получает
собственный следующий feature commit и отдельные positive/negative gates.
