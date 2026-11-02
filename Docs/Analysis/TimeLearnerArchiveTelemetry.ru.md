# Телеметрия в физическом model archive

Реализация требует серверной приёмки; это не новый checkpoint динамики.
`ModelArchive::Snapshot` после штатного SaveComponent меняет только текст
`State/StepDuration` на `0` в приватном USerStorageXML. Обход следует реальному
дереву компонентов и соответствующим узлам `Components`; проверяются тип
свойства ptPubSysState, скалярная форма и полнота структуры. Поиск всех XML
элементов с похожим именем не используется. Живые объекты не изменяются.

StepDuration — явно разрешённая измеренная wall-time телеметрия UContainer.
Остальные State сохраняются: в них находятся необходимые связи. TimeStep,
MaxCalculationDuration, CalculationDurationThreshold и прочие параметры
сохраняются. Семантическая model identity и формат archive остаются прежними.
Прежние signed snapshots с ненулевым StepDuration читаются существующим
импортом с проверкой их исходных hashes, без переписывания истории.

Проверки в корне: native-archive-telemetry-v1.json. Нужны сравнение всего
snapshot живого графа до/после изменения counters, неизменность живых counters,
сохранение scheduler limits, отличие модели при изменении действующего
MaxCalculationDuration, импорт прежнего ненулевого counter и fresh-process
перенос несимметричной модели с точными matrix/physics, внешним источником,
полными input/channel/soma/time массивами при 2000 и 20000 Гц. Дополнительно
повторяются текущие отрицательные archive controls и learned C₀ transfer.

Полный signed archive может различаться transient buffers отдельных image
instances: в normal A1 обнаружен InputFeedbackSignal 0×0 → 1×1. Storage
сравнивается по полному semantic fingerprint. После native Load/Init меняется
также производная TimeConstant индивидуального канала; сохранять её старое
кэшированное значение не требуется от physical import. Параметры, граф и
реальный отклик должны сохраняться. Эти различия не удаляются из архивов
настоящим изменением; canonical StepDuration — только один разрешённый metric.

Это уменьшает одну подтверждённую причину нестабильных байтов. Другие различия
исторических больших PostTune пакетов требуют отдельного анализа. Уникальные
первичные результаты не удаляются, hashes старых пакетов не пересчитываются.
