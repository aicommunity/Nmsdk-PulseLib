# Повторное использование проверенного архива при экспорте

На M4012.11.2026 приняты свежая SDK-сборка и36/36 native проверок, включая
четыре новых `NativeValidatedArchiveExport`. Проверены outer+36 closed groups,
released lease и неизменность source/runtime/protected bytes. Точные версии:
root08b898b1ad89d002cccde534e08e0925e080e45d,
Pulse e9b80746103750d3b5a100d74549462d6d2c46c6.
[Исходные JUnit и доказательства](https://github.com/aicommunity/Nmsdk/blob/fe6c29736/Docs/StructTrain/TimeLearner/ValidationPlans/M40ValidatedExport36A2-20261112/RESULT.ru.md).
Уточнение12.11: полный N4 native roundtrip принят2/2 на SDK448b153f2/Pulse7b524b8: Classic/Branch import → native Build/Init → export → второй fresh import, равные model revision/retained ledger и все stimulus/Soma samples,4 stimulus edges/ненулевой excursion. Whole outer+2 child groups closed, lease released, input/source/runtime неизменны. [Точные native proofs и scope](https://github.com/aicommunity/Nmsdk/blob/b6f1690f903959deceb7f6e04d1ed3f0716f1a93/Docs/StructTrain/TimeLearner/ValidationPlans/M40RecordedN4NativeRoundtrip2A3-20261112/RESULT.ru.md). Обе модели остаются stopped: invalid_observation/budget_exhausted без certificate; cold training и классификация этим не подтверждаются. Большой export теперь runtime-проверен, однако separate before/after binary performance, independent readback нового полного native2 corpus и Elder с optimized SDK ещё открыты.

Исходный случай: большой
N4 Branch завершён process timeout во время экспорта, до записи итогового
пакета. Это не доказательство ошибки измерения или физической невозможности.

`EncodeAndValidateNativeCandidateWindowServiceArchive` возвращает пару:
исходный `NativeCompositeArchiveDocument` и точный результат его полного
`DecodeNativeCompositeTrainingArchive`, включая проверку внешнего CSV mirror.
`DecodedNativeCompositeArchive` по-прежнему нельзя создать конструктором по
умолчанию извне. Это закрытые DATA, не live execution authority.

```text
capture fresh service archive at the existing legal boundary
encode the same records, primary bytes, hashes and semantic bindings
fully decode and validate the resulting document and its primary mirror
return document + validated decoded value
move the decoded value into the portable proof
continue the existing model/reference/certificate/storage checks
```

Factory protocol5/package6/7 потребляет готовое проверенное значение через move
вместо второго декодирования того же документа. Summary8 использует такое же
проверенное значение для recovery ledger. Старый публичный encoder сохраняет
сигнатуру и формат результата, делегируя той же полной проверке.

Это устраняет по одному избыточному полному декодированию в каждом из двух
путей. Формирование отдельного summary всё ещё повторно создаёт свой архив;
Console publisher и внешние import/readers по-прежнему выполняют независимые
проверки. Общий immutable export snapshot для всех трёх потребителей пока не
реализован. Ускорение end-to-end и достаточность лимита финализации не заявлены.

Постоянного кэша в learner/service нет. Между вызовами, revision, Reset/Build
или попытками обучения значения не переиспользуются. Lifecycle, scheduler,
физика, алгоритм поиска, допуски и wire schemas не меняются.

Приёмка кандидата: четыре новые реальные Classic/Branch×completed/stopped
проверки `NativeValidatedArchiveExport`, старые32 controls SDK. Проверяются
точное равенство document bytes/primary старого API wrapper и нового API
в одном новом бинарнике (не A/B двух бинарников), независимый
decoder, portable proof/references/certificate, отказы на изменённый внешний
CSV и coherently rehashed foreign source, неизменность живого графа/dataset.
Численные проверки и сборка выполняются на серверах; локальный diff не является
runtime-приёмкой. Большой actual N4 export остаётся отдельным обязательным gate.
