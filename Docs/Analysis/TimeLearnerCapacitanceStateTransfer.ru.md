# Перенос исходных C₀ вместе с обученной моделью

Статус: адресно приняты explicit native C₀ export/import, bound baseline/growth
и транзакционные edits. Полный C-trim V2 и весь этап5/7 этим не закрыты.

`ExportNativeLearnedState(implementationId, NativeCapacitanceCorrection)`
экспортирует completed/stopped package schema 2. К полному физическому архиву,
декларации, наблюдениям и сертификату добавлено `capacitance_state`:
неизменяемый baseline schema 2, полная карта абсолютных множителей,
объявленная граница поправки и revision фактической конечной модели.
Экспорт проверяет физические C всех каналов, классы/статическую физику,
storage-образы и единую частоту. Сертификат проверяется независимо и должен
по-прежнему соответствовать текущей модели. Поправка C не создаёт Exact.

Импорт сначала восстанавливает приватные нейрон и dataset штатным путём,
проверяет C₀ и поправки на этом графе и только затем заменяет живые дочерние
компоненты. Ошибка C₀ не должна менять прежнюю модель или её controller state.
Оба learner сохраняют импортированное состояние C₀; последующий обычный
`ExportNativeLearnedState(implementationId)` повторно проверяет его и
экспортирует schema 2. `ImportedNativeCapacitanceState()` возвращает именно
состояние последнего импорта, а не автоматически обновляемый сертификат.
Default и успешный ConstructNativeModel очищают состояние прежнего импорта.

Пакеты schema 1 продолжают использовать прежний путь. Отсутствие C₀ в них
означает отсутствие сведений о происхождении C, а не доказательство отсутствия
trim. Нельзя восстанавливать исходный baseline из уже обученных C.

Ограничения данного среза: перед первым экспортом caller явно передаёт полную
поправку; автоматическое владение baseline алгоритмом C-trim V2 ещё предстоит
встроить. Если после импорта выросла структура, карта C₀ должна быть явно
расширена проверенным native editor; экспорт неполной карты отклоняется.
Приватная retention-запись не является обещанием Save/Copy/XML checkpoint
контроллера или его динамики. Вложенный Calculate не вызывается; Build/Reset
выполняются штатно при применении структуры между пробами. Численная обработка
остаётся в scheduler. SyncCapacitanceTrim On
для V2 по-прежнему не включён.

В корне добавлены NativeCapacitanceStateTransport fixtures: Classic/Branch,
N0/N1, completed transfer с явно заданным sync=0.2, реальные frozen/fresh-process
пробы, identity/scaled recognition, partial stopped transfer и повреждённая
карта поправок. Это проверки переноса, а не сходимости при default sync=0.02.
Первичные CSV и schema-2 packages должны проверяться независимыми readers.

## Адресная приёмка 03.11.2026

Проверенный Core: `69d463aa50144a1c8ddb4a674bb900c2a07e769f`.
Normal root58d941c2 подтвердил9/9 Rdk unit и14/14 native transfer; исходный
Python reader не включал growth-only ClassName dependencies. Root6db2d5156
исправил его; независимый reader24/24 повторно проверил прежние primary,
включая удаление growth-only dependency. Исторический failed reader не переписан.

ASan root `6db2d5156fe51e03e1ebdeb1c256c5010dadb9b7` завершил9/9 unit,
35/35 native и24/24 readers:8 bound C₀ (обе топологии/N0/N1/2000/20000Hz),
6 joint candidates/rollback,3 native operation diagnostics и14 transfer.
Actual libasan mappings и все766 members/5 367 892 compressed bytes проверены.
Primary в корневом Git:

- Normal: `57383f8d93ebf660bd2f80d08ac315a67abe2896`,
  `Reports/StructTrain/TimeLearner/s05-capacitance-state-normal-b3`.
- Independent reader: `b7b2d5d0a15e281da503d0b5af076d03e072f21f`,
  `Reports/StructTrain/TimeLearner/s05-capacitance-state-readers-b5`.
- ASan: `780b21afcd6e9d477bc7eb8181217c5a655fa0df`,
  `Reports/StructTrain/TimeLearner/s07-capacitance-combined-asan-c2`.

Границы: completed positive fixtures явно используют SyncTolerance=0.2;
это не сходимость при default0.02 и не научная классификация. Cross-inventory
normal14/ASan35 совпали по non-time columns118/122 общих CSV;4 waveform
отличаются при разной предыстории model time старого dataset. Полную
cross-run эквивалентность не заявляем. Source clock defect исследуется
отдельной веткой; в этот Core не входят последующие dataset/archive изменения.

Остаются automatic C₀ ownership/search, XML/Copy persistence correction map,
полная матрица профилей/режимов и научные эксперименты. В develop объединяется
проверенный explicit API; SyncCapacitanceTrim On для V2 остаётся запрещённым.


### Кандидат portable boundary schema3 (2026-11-03; серверная приёмка ожидается)

ConfiguredNativeTraining schema3 явно задаёт `capacitance_history.record=true` и
`maximum_relative_change`; schemas1/2 сохраняют прежний смысл. При первом native
запуске configured learner связывает C₀ через штатный metadata-only binding до
начала измерений. Повторный binding проверяет владельца, не захватывает новую
базовую физику. Это ещё не включение алгоритма C-trim On.

Completed/stopped model package schema3 сохраняет signed capacitance_state,
исходное и текущее physical witnesses и точную привязку сохраняемых trial IDs.
При экспорте история поиска удаляется; остаются references/finals, а у diagnostic
boundary также реально наблюдавшиеся пробы. Это перенос завершённой границы,
не checkpoint поиска или физической динамики. Import проверяет C/C₀ для каждого
канала, классы, частоту, storage closure, origin/policy и сохранность heterogeneous
baseline. Переподписанный digest не заменяет проверку физики. Повторный Start
удаляет контексты проб прежней попытки, сохраняя действительные references и C₀.

Корневой regression spec содержит209 cases: прежние197 и12 новых boundary
controls. Новые случаи охватывают Classic/Branch, N0/N1, configured/manual,
повторы N0, dormant state, completed/stopped, same/fresh-process transfer,
rollback повреждённого пакета и остановку до первой пробы. Independent reader
добавляет12 проверок первичных CSV и повреждённых связей. До серверного запуска
эти изменения имеют статус «реализовано в коде», без runtime-подтверждения.
