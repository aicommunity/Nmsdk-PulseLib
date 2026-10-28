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

## Перенос состояния learner

`NativeLearnedState.h` над физическим архивом переносит Dataset, configuration,
references и сертификат завершённой solo-границы через `ExportNativeLearnedState`
и `ImportNativeLearnedState` обоих learner. Завершённый пакет имеет
`kind=timelearner_completed_state`, schema 1. Проверяются закрытие storage-классов,
эффективные параметры/связи и повторное воспроизведение доказательств сертификата.
При отказе подключения нового Neuron возвращаются прежние Dataset и Neuron;
полная приёмка общей версии ещё требует maintained-suite gate.

В ветке `stage05/stopped-training-state` добавляется отдельный диагностический
`timelearner_stopped_state`, schema 1. Он содержит исходную физическую модель,
причину остановки, все фактические попытки проб и валидный префикс references,
но всегда `certificate=null`. Незавершённая проба сохраняется как censored
evidence, а не как checkpoint её интегратора. После import управление выключено;
новый warm run захватывает незавершённую цель только на исходной модели/epoch,
либо требует явного cold restart/retarget. Explicit target invalidation удаляет
прежний capture plan, включая callbacks. Reset не подменяет retarget.

Подробности и границы приёмки:
[завершённая граница](../../../../Docs/StructTrain/TimeLearner/NativeLearnedState.ru.md),
[остановленная граница](../../../../Docs/StructTrain/TimeLearner/StoppedTrainingState.ru.md).
Recognition conversion, PostTune и calibrated transfer ещё не покрыты этим API.
