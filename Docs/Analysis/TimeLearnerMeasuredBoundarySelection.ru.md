# Выбранная измеренная граница ограниченного поиска

Статус19.11.2026: read-only выбор контроллера, полный native capture и явный
terminal restore адресно проверены в root3c5bfe01609a6e6bd290aa9697938e885d5904b6 /
Pulse1d8fc24cdb19b4b0e399483d198314b84548f9ea: M40 44/44 и Elder14/14.
Это не полная приёмка алгоритма. Portable schema8/summary9 и дальнейшее warm
продолжение проверяются отдельно; ordinary cold declarations ещё не включают
terminal policy. Более ранние описания ниже поясняют границу самого API.

`BoundedTrainingSession::SelectedMeasuredBoundary()` возвращает последнюю
`MeasuredCandidateSelection`: монотонный serial решения, sweep/inputPosition
и полную CandidateEvidence с model revision/epoch, anchor/input trial IDs,
всеми L/Ns/Rs, optional C state и измеренными остатками.

Выбор производится **существующим** Rank после полного принятого измерения
кандидата, включая все объявленные N0 repeats. Прогноз, частичный повтор,
неизмеренный edit и invalid observation не создают выбранную границу. Правила
ранжирования, допуски, порядок обхода и final verification не изменяются.

Локальный best сбрасывается при переходе к следующему входу; выбранная
граница сохраняется, чтобы адаптер мог наблюдать решение, принятое прямо
перед AdvanceInput. Следующее улучшение получает новый serial даже в другом
input/sweep context. Stop/ExhaustBudget не заменяют её текущим неизмеренным
графом. Новая сессия создаёт свой пустой selection.

Сам объект не содержит Rdk components, XML checkpoint или полномочия на
восстановление. Native adapter захватывает фактический полный граф при
наблюдении нового serial, проверяет revision/epoch/параметры/контекст и выполняет
отдельную terminal transaction. Старый composite actor не становится
действующим снова. Owner mirrors/C-control, edit budget и событие возврата
включены в адресные native проверки; согласованный portable model export
остаётся незавершённой приёмкой.
Наличие выбранной точки не означает Exact; нужна свежая all-input проверка.

Подготовлены три kernel-regressions в корневых штатных интеграционных тестах:
переход к final verification без потери selection; новый неизмеренный edit и
budget stop; отказ публикации незавершённых N0 repeats. Они используют реальный
ProbeRecorder и production BoundedTrainingSession, но управляемый физический
отклик. Их PASS сам по себе не подтверждает native graph restoration.

В новой portable серии A3 protocol4 warm Start удалял измеренные candidate/anchor
groups, на которые продолжал ссылаться terminal journal. Проверка повторного
экспорта отклоняла такую границу с terminal_selected_composite_measurement_missing.
Исправление удерживает объединение reference groups и trial IDs всех journal
selections, проверяет отсутствие потерянного primary и прежний общий byte budget
до публикации новой попытки. Данные не превращаются в живой checkpoint;
восстановленные старые измерения сохраняют исходные revision/epoch/attempt.
Исправление ожидает отдельной серверной приёмки. Warm regression теперь должна
принять реальное новое logical measurement перед stop/export/import, а не только
выполнить немедленный Start/Stop. Protocol5 уже удерживает весь свой archive.

Другой отказ A3 затронул четыре completed plain cases на штатно построенном
графе: legacy Build оставлял CapApplyPending=true при SyncCapacitanceTrim=0.
Native probe и scheduler уже считают pending physics только включённый trim,
но snapshot guard ошибочно отвергал сам неактивный sentinel. Guard согласован
с действующим условием обоих learner: pending && trim>0. Флаг не сбрасывается,
параметры модели не переписываются; включённый pending trim и PostTune остаются
запрещены. Отдельная native приёмка исправления ещё впереди.
