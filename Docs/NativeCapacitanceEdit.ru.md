# Физический механизм C-trim: явный per-channel baseline

CaptureNativeCapacitanceBaseline получает реальные Capacity всех NPulseChannel
текущего графа, включая отдельно Exc/Inh, Soma и LTMembrane. Schema1 содержит
частоту, исходный model revision, path→C0 и canonical digest. Parent
MembraneCapacity не используется. Это физическая декларация исходного C0,
не сертификат обучения и не полный checkpoint контроллера.

ApplyNativeCapacitanceScale принимает явные множители по dendritic membrane.
Каждый её канал получает собственное C0×scale. Scale абсолютный относительно
baseline: повторный вызов не умножает уже исправленную ёмкость. Поддерживается
положительная конечная ёмкость и конечная граница относительной поправки.
Новые сегменты без записи baseline отклоняются; C0 не угадывается по parent
или произвольной константе. Связанные Capacity и другие уравнения каналов
отклоняются, пока их поддержка явно не доказана.

Изменение разрешено только на consumed native probe boundary. Owner adapters
запрещают внешнее вмешательство при активном Train и frozen recognition.
Сначала native snapshot/Load создаёт preview, затем проверяются реальные
интеграционные уравнения и весь predicted model identity. Только после этого
меняются живые Capacity с Init/Reset физического нейрона; Calculate, learner
Reset/Build и prototype mutation отсутствуют. Idempotent edit не сбрасывает
живую динамику. При неожиданном live отказе прежние Capacity восстанавливаются
и сверяется исходная identity; ошибка rollback остаётся явным Failed.

Это foundation, а не готовый optional C-trim V2. Общий контроллер пока явно
отклоняет включение режима. Ещё нужны: выбор C-кандидатов по измерениям,
revision/epoch invalidation всех зависимых inputs, явная принадлежность baseline
к ModelSpec/profile, baseline extension при growth, сохранение correction state
при export/import/warm continuation, fresh final verification и вся матрица.
Не следует подключать legacy DendriteCapacitance как якобы полный baseline:
старая scalar-per-input запись уничтожает heterogeneous child physics.

Standalone API прошёл8/8 на Elder normal и8/8 на M40 ASan: root evidence
5acd3eebbf2c812a5e04e07bd0a14f88eb2ea4fb и23f0086845092feed0967112b1cd59583d086149.
Все32 CSV byte-equal и32 packets equal между workers. Проверены native
fixtures Classic/Branch×N0/N1×2000/20000Hz: heterogeneous Exc/Inh, LT/Soma
preservation, native preview roundtrip, idempotence, real probe responses,
NaN/Inf/bounds/unstable candidate rejection, restore и missing growth baseline.
## Диагностика native операций и границы приёмки

Совместное предложение L/Ns/Rs/C сначала проверяется на отдельной native модели.
Rs domain вычисляется с новой C. Живой граф изменяется транзакционно с checkpoint;
при восстановлении заменённого графа learner обновляет aliases и сообщает
graphWasReplaced. Это ещё не включение C-trim в алгоритм ControllerVersion2.

Init/Reset и сбор наблюдений теперь проверяют process-local operation ledger Rdk.
Bool lifecycle не используется для распространения ошибок. Исключение дочернего
Calculate делает наблюдение невалидным; ошибка Init/Reset запрещает принятие
кандидата. Проверяется только новая ошибка после marker: старая история не
блокирует следующую исправную пробу. Cleanup источника выполняется до проверки.

Первый joint gate: 8/10; оба injected rollback не сработали ожидаемым образом.
Этот отрицательный результат сохранён корневым evidence commit
7364e82e219b1a63b6fa6f598dc7876dc2e59b02. Новые тесты отдельно фиксируют
факт вызова fault hook и C в момент ошибки. Elder rootd8bf0419ef3e022514f77613e2f4a5acbe4e4875
прошёл9 Rdk unit и12 native cases, evidence6bb5e09b897d0932f641f42aa8e966b935f713a9.
Факт Reset fault при4nF, полное восстановление графа и следующая исправная
проба подтверждены обоими rollback tests. M40 та же версия тоже завершила9/12
под ASan; публикация полного результата ожидает отдельный regression owner.

Дополнительный Rdk дефект: старый UContainer::Build при exception возвращал
false. Native baseline738985c742f881e3fca3ff23229f68db7d3adf68 подтвердил это
единственным failure. Rdkc3850711b2cd73f33956aae18a8db6cd1c4ead41 исправляет
bool и переводит Connect/CreateStructure/Environment consumers на operation
diagnostics. Root5d73cd0858abc1e93655d5fa10189225da990201 прошёл9 unit и13 native
на Elder. ASan этого следующего Rdk patch ещё требуется. Полный C-trim controller,
baseline persistence/growth и вся матрица данным результатом не закрываются.

# Дополнение: привязанный baseline и рост структуры (проверка не завершена)

`CaptureBoundNativeCapacitanceBaseline` создаёт schema 2 отдельно от совместимого
baseline schema 1. Документ содержит исходный semantic graph, NativeModelSpec,
замыкание storage-образов, их семантический fingerprint, индивидуальные C₀,
классы, статическую физику и происхождение каждого канала.
Проверяется целостность документа и соответствие текущих каналов/образов.
Изменённые Resistance, FBResistance, RestingResistance, Type и флаги усреднения
не разрешают применять прежнюю поправку C как будто физика осталась прежней.

Совместный структурный кандидат может добавить C₀ нового пути только из
известного образа с совпадающими классом, C и статической физикой. Уже
захваченное C₀ никогда не заменяется текущим, возможно скорректированным C.
Расширенный baseline возвращается в `NativeEditResult.capacitanceBaseline`
только после успешного commit кандидата; при rollback результата нет.
Standalone schema 1 и запрет неизвестных новых каналов сохраняются.

`Encode/ParseNativeCapacitanceState` сохраняют baseline, полную карту абсолютных
масштабов, границу изменения и revision физической модели. Read-only
`ValidateAppliedNativeCapacitanceState` проверяет уже построенную модель,
включая каналы, пропущенные в карте масштабов, и model revision.
Это отдельное состояние параметров, пока не встроенное автоматически в
`NativeLearnedState`, и не checkpoint контроллера или динамики посреди пробы.
Полноценный поиск C-trim контроллером V2 пока не реализован; включать его в
экспериментах по-прежнему нельзя. Новые проверки Classic/Branch × N0/N1 ×
2000/20000 Гц добавлены; серверного подтверждения этого дополнения ещё нет.

