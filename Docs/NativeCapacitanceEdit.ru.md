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

Серверная проверка нового API ещё не завершена. В корне объявлены8 native
fixtures Classic/Branch×N0/N1×2000/20000Hz: heterogeneous Exc/Inh, LT/Soma
preservation, native preview roundtrip, idempotence, real probe responses,
NaN/Inf/bounds/unstable candidate rejection, restore и missing growth baseline.
