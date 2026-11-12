# Общая граница экспорта модели и сводки

Оба TimeLearner предоставляют ExportNativeLearnedBoundary(implementationId), а также вариант с явной NativeCapacitanceCorrection. Возвращается NativeTrainingExport { learnedState, summary }. Прежние ExportNativeLearnedState и EncodeNativeTrainingState остаются доступными с прежними схемами данных.

Для protocol5 фабрика один раз кодирует и проверяет measurement archive, воспроизводит portable proof и проверяет recovery ledger. Summary7/8 строится на той же завершённой границе; summary8 использует уже проверенный ledger, не пакует повторно CSV/base64. Предоставить такой ledger извне нельзя: ускоренный путь находится в private translation-unit helper и вызывается фабрикой после полной проверки. Отдельный вызов EncodeNativeTrainingState сохраняет самостоятельную проверку.

Проверяются сохранность attempt/epoch/outcome/reason, references/finals, размер истории и фактическая physical identity при экспорте. Recovery deliveries ещё раз сверяются с тем же проверенным архивом перед выдачей общей сводки. Это значение одной границы, а не постоянный service cache, checkpoint или новая live authority.

NativeFinalizationBoundary хранит исходную summary неизменяемым значением вне сериализованного документа. TrainingSummary() и NativeLiveTrainingSummary() позволяют Console забрать её до перехода в recognition; импорт/reset не подменяют исходные solo references текущей моделью. Serialized finalization schema/hash не меняются.

Calculate/Reset/Build/Default, storage profiles, CapturedPerInput, назначение пиков, алгоритм поиска и допуски не меняются. Export проверяется только на завершённой границе, вне queued/running probe; native dynamics не копируются как обещанный mid-probe checkpoint.

Статус12.11.2026: реализация подготовлена; серверная сборка, адресные интеграционные проверки и большой временной контроль ещё не приняты. Корневое описание дефекта: Docs/StructTrain/TimeLearner/RecoverySummaryExportTimeout-20261112.ru.md.
