# Declared-window basin necessity: geometry-only candidate

Метод `declared-window-basin-necessity-v1` реализуется отдельно от matched
omission v1. Он выбирает actual full максимум в **заранее объявленном** окне
закреплённого input по необходимости источника для локальной вершины в
фиксированном бассейне. Он не является физической декомпозицией, Track/Train
certificate или доказательством доминирующего вклада импульса.

Первая реализация — pure geometry-only: один явно общий sample grid,
baseline/raw vectors fullBefore/fullAfter и ровно N omissions с исходными
ordinals. Caller отвечает за native context, source/event/frame correspondence,
model/reset/routing/binding и происхождение окна. Utility этого не доказывает
и не использует whitelist причин старого v1. N=1 оставлен SingleImpulse пути.

Сам utility не выводит окно из EstDelayPerSeg и не выбирает его по амплитуде
наблюдаемого кандидата. Окно фиксировано для одной независимой N+2 группы и
её full repeat. Controller может заранее предложить другое окно следующей
группы, в том числе используя delay model: это новая measurement specification
и полностью свежие observations. Допустимость таких proposals для Train должна
определяться отдельной policy; post-hoc подбор удобного окна из той же записи
не доказывает assignment. Выход вершины из узкого окна означает отсутствие
поддержанного измерения в этом протоколе, не физическую недостижимость.
Полная запись используется до ограничения окном.

Краткий контракт:

1. Проверить schema, конечность, размеры, строгий общий clock и budgets.
2. Выделить точные plateau-aware экстремумы на полной записи. Full candidates
   выше existing minimumAmplitude повторно используют ExtractMeasuredResponse.
   Бассейн ограничен ближайшими измеренными минимумами; края записи отмечены
   отдельно. Плато не делится искусственно по границе окна.
3. В каждом control искать полные геометрические максимумы в фиксированном
   бассейне. Один наблюдаемый максимум выше floor — Preserved; отсутствие
   максимума — AbsentWithinFixedBasin; split/multiple, sub-floor extremum,
   пересечение границы или clipped feature — Unknown. Control экстремумы
   выделяются глобально, а не на обрезанном подокне.
4. Без Unknown единственный absent ordinal задаёт UniqueNecessary. Несколько
   absent дают SharedNecessary; отсутствие absent не создаёт назначения.
5. Выбрать единственный высший actual full candidate назначенного ordinal.
   Equal/higher potentially-assigned ambiguous или clipped candidate **внутри**
   окна нельзя скрыть меньшим удобным. Если control omit-assigned однозначно
   сохраняет вершину, assigned не необходим для неё: другой UniqueNecessary
   либо SharedNecessary без assigned и даже Unknown по другим controls не
   дают veto назначенному более низкому кандидату. Поздняя shared вершина вне
   окна не даёт global veto. Если назначения вообще нет, ambiguous локальная
   геометрия сохраняет Ambiguous вместо выдуманного измерения.
6. Full repeat должен проходить отдельный absolute repeatability policy и
   сохранять локальную топологию extrema/valleys и диапазоны выбранного плато.

Невоспроизводимая миграция/слияние, видимые по геометрии, остаются отрицательными
outcomes. Невидимая миграция физического отклика между бассейнами может иметь
те же N+2 endpoint records: это ограничение причинной интерпретации, не иной
результат объявленного наблюдаемого критерия. Внешние event/source assertions
не восстанавливаются из общей геометрической оси. Raw potential, amplitude,
time и plateauEnd выбираются из fullBefore; difference trace не измеряется.

Train integration, codec/schema для этого метода и сертификация не добавлены.
Старые метод/codec/schemas1–6 и их outcomes не меняются. Candidate требует
отдельного server build/runtime gate; локальные численные tests не запускаются.

Отдельный root target `Test_PulseLib_DeclaredWindowBasinAssignment` компилирует
именно production DeclaredWindowBasinAssignment.cpp и MeasuredResponse.cpp,
без native/storage SDK dependency. Тот же test source включён в полный storage
target; чистый gate не заменяет будущий native context bridge. Предобъявленный
Исходный filter `DeclaredWindowBasinAssignment.*` содержал35 literal TEST cases, включая
residual tail, higher known/shared-other/Unknown, under-floor/control split,
plateau/valley/window/record boundaries, repeat topology/winner, invalid data и
budgets. Migration-endpoint case специально допускает измеримый operational
outcome и проверяет diagnostic-only eligibility: физическая lineage не
выдумывается из этих endpoint records. Серверный normal/ASan запуск исходного
root236/Pulsefce выявил34/35: отрицательный plateau fixture оставлял следующий
sample выше плато и фактически задавал монотонный shoulder. Отдельный root
fixture-only commit `3c2ae1fd696462dad4677a795a05db80f5bac93d` сохраняет expected
Ambiguous, делает оба соседа плато ниже и добавляет исходный shoulder как positive
case. Текущий inventory —36. Исходные negative35 receipts не переписываются;
новый36 gate и native bridge acceptance учитываются отдельно.
