# Полная физическая модель отдельно от истории обучения

Статус на19.11: исходные7 physical DATA cases приняты на свежем Elder SDK
Root893b7344eb8794e5ef981eb133e669b6060e5aa3 /
Pulsec508392a8c963ceb639050d737717bff0d853b3a. Полный primary находится в
центральном M40 archive, каталог
experiments/2026-11-19/native-terminal-portable/elder-recognition7-a1.json.
Эти7 проверяют asymmetric graph и native target/foil responses исходной версии.
Прямой solo terminal8 DATA roundtrip — новое расширение ниже; оно ещё не
собрано и не принято серверной проверкой. Этап5 остаётся частичным, нового
научного результата из этих проверок не следует.

`Core/TimeLearner/NativePhysicalModelData.{h,cpp}` вводит документ
`timelearner_physical_model_data`, schema 1. Он содержит целые штатные архивы
нейрона и Dataset, определения используемых storage-образов, реальные дочерние
компоненты и связи, частоту, индивидуальные L/Ns/Rs, физические C и порог,
декларацию модели, solo/recognition revision и запись преобразования.
Не используется список ручных XML overlays или реконструкция по parent defaults.

Экспорт принимает полный исходный native package: solo completed/stopped,
recognition/diagnostic recognition, threshold candidate или calibrated state.
Сначала вызывается существующий соответствующий importer со всеми проверками
исходного пакета. Затем полная физика отделяется от истории поиска/измерений.
Исходный пакет не сокращается и не удаляется; его SHA находится в
`source_boundary`. Проверка этого SHA по опубликованному исходному evidence
остаётся обязанностью внешнего pipeline.

Импорт нового документа создаёт private children через PrepareModelArchive,
ImportDatasetSnapshot и ImportModelArchive с реальной границей `burst`.
Проверяются effective graph/hash, параметры входов, классы, target, stimulus,
частота дерева, cache и реальные связи. Для этого используется общий
InspectNativePreparedChildren; проверки прикреплённого owner не ослаблены.
При установке на fresh owner штатный InstallNativePreparedModel отдельно
проверяет owner, вычислительный порядок, устойчивость и сохранность графа.
Вложенный Calculate, Default после загрузки и правки prototype не добавлены.

Документ помечен `data_only=true`, `dynamic_checkpoint=false`, `certificate=null`.
Возвращаемая структура не содержит service, search cursor, callbacks или
сертификата. Метаданные первоначального архива сохраняются как данные источника;
они не восстанавливают полномочия контроллера или подтверждение калибровки.
Существующие learned/recognition proof importers новый kind не принимают.
Сохранённый порог является физическим параметром, а не новым свидетельством
успешной калибровки. Возобновление в середине пробы не поддерживается.

Обязательные проверки корневого репозитория: Classic/Branch с несимметричными
входами, полное сравнение Soma waveform через native scheduler, противоречие
параметров/класса и попытка придать DATA документу certificate/checkpoint.
Первоначальный fixture начинается с честно остановленной cold-границы без
выдуманного обучения. Он не заменяет проверку реальных обученных моделей,
heterogeneous/C-trim, fresh process и всех паттернов. Новые terminal8 assertions
ниже используют существующие completed/restored native fixtures; научная N4
и полная физическая/режимная матрица остаются открытыми.

## Замороженное выполнение DATA-модели

Оба learner получают явные методы ImportNativePhysicalModelData,
ExportNativePhysicalModelData и HasNativePhysicalModelData. Импорт допускает
только документ из recognition/diagnostic recognition/threshold/calibrated
границы. Solo-архив сначала требует явного Convert; входы не подключаются
неявно ради прохождения проверки. Проверяются все input banks и совпадение
физического Threshold реального LTZone с metadata, без исправляющих overlays.

Общий InstallNativeFrozenState сохраняет существующую native commit boundary,
проверки owner/control/lifetime, порядок детей и readback. Прежние proof importers
вызывают тот же код через прежний PrepareNativeRecognitionState. Для DATA
вызывается PrepareNativeFrozenPhysicalModelData; static calibrationConfirmed=false
не позволяет вернуть полномочия калибровки из metadata исходного архива.

После импорта используется прежний QueueNativeRecognitionTrial и штатный
scheduler. BeginNativeTraining и экспорт recognition proof отвергаются.
Возвращать исходный DATA-документ разрешено отдельным ExportNativePhysicalModelData
только после полной проверки frozen hold. Нельзя заменить граф при queued/running
trial или войти через DATA в owned finalization. Старое solo C-control состояние
и завершённая finalization metadata не переносятся на новый DATA hold; физические
дочерние C сохраняются самим архивом. Native training service остаётся Idle,
certificate отсутствует, административный legacy phase не обозначает Done.

Принятый корневой fixture сравнивает старый full-source import и DATA import на
полных target/foil окнах: Soma, фактические входы, LT output, LT decision potential,
stimulus/spike timestamps, reset policy, completeness и model revisions. Отличаться
может только собственный run_id. Дополнительный отрицательный случай проверяет
solo и заново подписанную противоречивую threshold metadata. JSON/CSV сохраняются
для графиков при TIMELEARNER_EXECUTION_OUTPUT. Исходные7 случаев выполнены;
расширение terminal8 ожидает отдельной проверки.

Console/runner имеют physical-model-export/physical-model-evaluate и source-only
DATA snapshot. Полная проверка больших terminal8 моделей и опубликованной source
closure ещё требуется. Наличие методов компонента не завершает этап5,
не подтверждает реальные большие обученные модели и не даёт научный PASS.

## Terminal8: изменение в коде, runtime-проверка ожидается

Для source_boundary типов completed_state/stopped_state верхняя допустимая
версия повышена7→8. Для recognition/candidate/calibrated источников остаётся7;
неизвестные solo9 и recognition8 отклоняются. DATA schema1 и роли не меняются.
Полный исходный пакет сначала проходит штатный importer, затем сохраняется
actual физика; historical_state не подставляется вместо actual graph.

В корневом CompositeTraining corpus расширены16 существующих случаев:
8 configured completed и8 actual changed-graph restores, обе топологии,
Fixed4/Window5, N0/N1. Дополнительный fresh storage owner должен сохранить
граф/параметры и ненулевой штатный burst response, оставаясь Idle/epoch0 без
certificate/search. Источник не теряет собственное состояние. Все отсчёты
двух native probes сохраняются и независимо сравниваются Python reader.
Допуск≤1e−12 относится только к roundtrip response, не к обучению.

Дополнительно проверяются явная solo→recognition conversion и frozen DATA
import/snapshot без history siblings. Полная frozen waveform/Console проверка
новой комбинации и настоящая N4 оценка обязательны до научного использования.
Прежние успешные7/native78 не принимают новые assertions автоматически.
