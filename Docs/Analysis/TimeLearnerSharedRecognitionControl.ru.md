# Candidate: общий контроль recognition probes

Core/TimeLearner/NativeRecognitionControl.h механически объединяет7 методов, ранее имевших одинаковые тела в Classic и Branch: ObserveNativeRecognitionContext, HasNativeRecognitionTrialIdentity, IsNativeRecognitionTrialIdle, QueueNativeRecognitionTrial, TakeNativeRecognitionTrialResult, CancelNativeRecognitionTrial, ExportNativeRecognitionState.

Общий helper не хранит новое состояние. Классы learner остаются штатными Rdk-компонентами; их public signatures, member layout, scheduler и topology-specific model bindings сохраняются. Связанные данные остаются у NativeRecognitionHold/NativeOwnedPostTunePhase. Единственный доступ через friend template struct, аналогично NativeLearnerLifecycle.

Порядок проверок сохранён: active owned PostTune запрещает внешнее управление trials; Queue проверяет recognition availability и текущий hold до enqueue; TakeResult/Cancel сохраняют прежнее поведение отсутствующего recognition; Export проверяет наличие, failure, implementation ID и hold перед возвратом immutable package. Ни новый Calculate/Reset/Build, ни изменение physics/storage, ни retarget/recalibration не добавлены. Тела двух топологий побайтно совпадали до переноса; обратное преобразование owner qualification восстанавливает исходный текст каждого тела. Это38 строк дублированных original bodies, а не весь общий контроллер.

Механическое совпадение не является runtime acceptance. Требуются завершение предыдущего lifecycle8644 baseline gate, затем отдельные server build и реальные recognition/phase/finalization/import/frozen checks на изменённом candidate. Native18/133 старого lifecycle snapshot нельзя приписывать этому helper. ASan и все реальные режимы PostTune/storage/physics остаются в общей матрице. Никакие numerical tolerances или CapturedPerInput/N−1 не изменены.

Изменение выполняется отдельным commit после предыдущего механического lifecycle выделения; algorithm fixes и acceptance не смешиваются с ним. До проверок candidate не объединяется в develop. Source hash и exact inverse receipt сохраняются в корневом отчёте соответствующего этапа.
