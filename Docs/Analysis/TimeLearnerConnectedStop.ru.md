# Connected native training control

Изменение source, подключённого к IsNeedToTrain, может обходить setter learner.
Общий adapter читает GetData, а NativeTrainingService проверяет запрос до
действий BeforeTick и до принятия наблюдения AfterTick. False останавливает
active session через существующий ExternalStop: probe отменяется/сохраняется,
certificate отсутствует, модель остаётся diagnostic. Direct setter сохраняет
прежнюю немедленную семантику; Calculate/Build/Reset не получают новых bool errors
или вложенного Calculate.

Готовая сессия не меняет статус из-за последующего false. Завершённое обучение
и удержание внешнего true — разные состояния; новый code не переписывает source.
Восстановление после stop требует объявленного reset/start; автоматический
restart по восходящему внешнему фронту этим изменением не реализуется.

Baseline 01bf64842 на обоих workers: 16 missed stops и 4 working true controls.
Fixed Core c99c47d: Elder-1 57/57 native, independent Python 10/10; M40 ASan
gate ещё выполняется. Полная приёмка и ограничения — корневой документ
Docs/StructTrain/TimeLearner/ConnectedTrainingStop.ru.md.
