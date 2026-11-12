# Повторное использование проверенного архива при экспорте

Кандидат12.11.2026, runtime-приёмка ещё не выполнена. Исходный случай: большой
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
точное равенство document bytes/primary старого и нового encoder, независимый
decoder, portable proof/references/certificate, отказы на изменённый внешний
CSV и coherently rehashed foreign source, неизменность живого графа/dataset.
Численные проверки и сборка выполняются на серверах; локальный diff не является
runtime-приёмкой. Большой actual N4 export остаётся отдельным обязательным gate.
