# Общая защита recognition-транзакций

`NativeRecognitionTransactionScope<Owner>` выделяет из существующей
`NativeThresholdCalibrationTransaction` проверки lifecycle, weak identity
recognition context, storage/environment/frequency, направления model clock,
полного ожидаемого package и принадлежности trial. Он не выбирает кандидатов,
не измеряет, не рассчитывает компоненты и не определяет научный verdict.

Native import выполняет штатные Build/Init/Reset и полный readback. Возвраты bool
не проверяются как error channel. Cancel разрешён только для записанного run/trial.
Rollback сначала сверяет context и ожидаемый package; чужое состояние не стирается.
Destructor scope ничего не откатывает: завершение принадлежит контроллеру.

Первое изменение является механическим выделением существующих guard-правил,
отдельным от нового алгоритма PostTune. Старый calibration controller использует
этот же helper с прежним diagnostic prefix. Приёмка требует A/B на обоих workers:
прежний immutable binary и новая сборка, реальные grid/ownership tests и сравнение
всех записанных физических CSV. До таких receipts это непроверенная переработка.
