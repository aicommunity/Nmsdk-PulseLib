# N0: отдельный конечный normalization turn для каждой геометрии

Статус: кандидат реализации; серверные проверки и новый physical A/B ещё не
выполнены. Причина — реальный Branch N0 baseline: после Ns4/L25 с хорошей,
но вне допуска амплитудой следующие timing-ready L26/27/28 измерены лишь
при Ns1. Старое условие enqueue требовало улучшения общего best, поэтому
плохая исходная амплитуда новой длины подавляла её локальную нормализацию.

`BoundedTrainingSession::PrioritizeN0Neighbors` отделяет global ranking от
локальной истории. `CandidateCursor::N0GeometryTurn` хранит локальный best
завершённого кандидата/полной группы повторов и оставшееся число предложений.
Scope сравнивает полный parameter vector с исключением только активного Ns,
input и эквивалентный эффективный C-context. Inactive Ns/Rs, все длины и active
Rs должны совпадать. Разделение upstream bank посредством PrepareCandidate
создаёт другую scope; её амплитуды не объединяются с исходным bank layout.
Model/protocol/frequency/reset context неизменен в сессии; новый input очищает
историю, а suspended C contexts владеют собственными cursors.

После valid timing-ready amplitude-outside измерения новая scope получает
собственный turn, независимо от global best. Последующие предложения требуют
улучшения локального best. Его allowance равен минимуму из:

- 12 предложений;
- оставшихся candidates на вход;
- оставшихся logical probes / 2 / K, где K — число обязательных повторов;
- оставшихся edits;
- max(4, remaining candidates / 4).

Таким образом малый бюджет не отключает полезный короткий N0-поиск, но цепочка
улучшающихся Ns не может бесконечно вытеснять следующие длины. Уникальное
enqueue расходует allowance; invalid observations, повторный визит и уже
измеренный count его не пополняют. Размер истории ограничен глобальным числом
кандидатов. Никаких новых публичных tolerances/budgets не добавлено.

Соседние целые counts формируют только предложения. Их legality, full-vector
deduplication, native edit, fresh anchor/input measurements и K repeats идут
существующим путём. Нет предположения глобальной монотонности или дробного Ns;
конечный whole-domain fallback сохранён. Global Rank и независимая финальная
проверка всех входов не меняются. Search budget/invalid/quantized результаты
не становятся Exact и не доказывают PhysicalInfeasible.

Изменение не затрагивает Rdk lifecycle, scheduler, storage-образы, физику,
нормализацию N1 или выдачу waveform. Нет вложенного Calculate и изменений
параметров на каждом тике. Локальная история является состоянием поиска,
а не обещанием checkpoint динамики посреди пробы.

Адресные production-session fixtures находятся в корневом
`Tests/Integration/Libraries/Test_PulseLib_N0Geometry.cpp`
с общим `Test_PulseLib_BoundedFixture.h`:
Classic/Branch с глобально худшим стартом следующей геометрии, N4 bank
propagation, завершённые K repeats и ограничение цепочки улучшающихся counts.
Это controlled response oracle, не физический эксперимент. После них нужны
существующие regressions, actual native SDK gate и отдельно объявленный
same-budget N0 A/B на исходном stock profile и CapturedPerInput.
