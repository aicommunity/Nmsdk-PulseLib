# Общая диспетчеризация lifecycle TimeLearner

Кандидат механического выделения из Pulse `cc97149c90ce98ff24ca688452f4a40d54e0e63d`; runtime-приёмка ещё требуется.

Оба компонентных класса сохраняют виртуальные ABeforeCalculate/ABeforeDefault/ABeforeReset. Общий NativeLearnerLifecycle выполняет прежний порядок действий с теми же полями состояния:

- BeforeCalculate: вход в live finalization, PostTune, recognition, configured training, training/probe before-children, затем legacy readiness и preflight.
- BeforeDefault: прерывание PostTune и освобождение связанных состояний, инвалидация target/control, затем квалифицированный базовый hook.
- BeforeReset: прерывание PostTune; явный Reset очищает управление, implicit Reset инвалидирует активную пробу; восстановление legacy readout и базовый hook.

Rdk вызывает эти hooks и рассчитывает детей своим scheduler. Все пути диспетчера возвращают true; ошибки остаются в NativeEditFailure, состояниях training/probe и ForceSkipComponentCalculation. Конкретные классы передают базовые hooks лямбдами, сохраняя квалифицированный вызов UNet.

Integration-preflight predicate остаётся в каждом .cpp: Classic использует failure codes2/3/4, Branch4/5/6. Одинаковый текст прежних hooks не означал одинаковых числовых кодов. Общий helper принимает сам predicate и не дублирует эту таблицу.

В inverse text check три тела методов возвращаются к исходным байтам после обратной замены owner и callback-параметров; ABI полей и публичные properties сохраняются. Это только механическое свидетельство. До merge требуются реальные scheduler/native gates, canaries пропуска нестабильной сомы в обеих топологиях, interruption/reset/default и A/B наблюдения с исходным SDK. Математические изменения поиска проходят отдельными commits.
