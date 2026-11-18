# Полная физическая модель отдельно от истории обучения

Статус: исходный SDK a3 собран,228 случаев выполнены,226 прошли. Два новых
transfer fixtures сравнили одинаковые полные трассы, но ошибочно требовали
однозначного пика; исправление сохраняет честный peak_ambiguous. Эта правка
проверялась отдельной сборкой Windows; её итог пока недоступен после потери VPN.
Следующее расширение frozen DATA API подготовлено в коде, ещё не собрано.
Это частичное выполнение этапа 5, а не новый подтверждённый научный результат.

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
Fixture начинается с честно остановленной cold-границы без выдуманного обучения.
Она не заменяет проверку реальных обученных моделей, heterogeneous/C-trim,
fresh process и всех паттернов. Эти проверки и подключение frozen evaluation
к новому формату остаются открытыми.

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

Новый корневой fixture сравнивает старый full-source import и DATA import на
полных target/foil окнах: Soma, фактические входы, LT output, LT decision potential,
stimulus/spike timestamps, reset policy, completeness и model revisions. Отличаться
может только собственный run_id. Дополнительный отрицательный случай проверяет
solo и заново подписанную противоречивую threshold metadata. JSON/CSV сохраняются
для графиков при TIMELEARNER_EXECUTION_OUTPUT. Эти новые случаи пока не выполнены.

Console/runner entry point для DATA evaluation и resolver опубликованной исходной
границы ещё требуют интеграции. Наличие методов компонента не завершает этап5,
не подтверждает реальные большие обученные модели и не даёт научный PASS.
