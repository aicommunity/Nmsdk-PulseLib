# Native model graph API в PulseLib

Публичные заголовки Core/TimeLearner/ModelBoundary.h, ModelInspection.h,
ModelIdentity.h и ModelArchive.h предоставляют единый readback, semantic identity
и versioned native graph archive. Namespace NmsdkTools сохранён для совместимости.
Console и интеграционные fixtures используют этот API из Nmsdk-PulseLib.core.
История переноса и приёмка: [ModelGraphOwnership](../../../../Docs/StructTrain/TimeLearner/ModelGraphOwnership.ru.md).

InspectModel не вызывает Build/Reset/Calculate и не исправляет модель. Full
сохраняет native property values; Structure предназначен для контроля графа,
bindings и cached pointers. IdentifyModel читает typed параметры и использует
тот же structural readback. Необъявленные внешние связи, неизвестные типы,
нечисловые параметры и повреждённый граф запрещают положительную проверку.

ModelBoundarySources задаёт живые caller-owned источники и стабильные port ID.
Экспортированный graph не владеет внешним источником. Перед возвратом graph или
source в storage разрывают native UNet связи. Экспорт выполняется на завершённой
границе, при остановленном изменении графа; API не является checkpoint динамики.

ImportModelArchive использует native storage/Load/Init и проверку identity.
Существующие images не заменяются. При неудаче новые неиспользуемые образы могут
остаться в storage: полноценная transaction ещё требуется. Наличие graph archive
не доказывает законченный Train→Test перенос обученных references, calibration,
conversion и controller certificate. Спецификации форматов и ограничения:
[identity](../../../../Docs/StructTrain/TimeLearner/ModelIdentity.ru.md),
[archive](../../../../Docs/StructTrain/TimeLearner/ModelArchive.ru.md),
[ports](../../../../Docs/StructTrain/TimeLearner/ModelBoundary.ru.md).
