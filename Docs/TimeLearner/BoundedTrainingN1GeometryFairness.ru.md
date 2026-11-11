# N1: независимый от recovery обход geometry/Rs

В NextDomainCandidate режим RetrySameCandidate больше не удерживает весь Rs-grid одной длины перед остальными длинами. Для всех native N1 на каждом проходе выдаётся один ещё не проверенный Rs каждой legal geometry. Каждый повторный вход требует свежей ResistanceDomainAssessment; source/background/C0 binding и finite budgets сохранены. Политика обработки действительно invalid observations не меняется.

Изменение касается явного общего V2 BoundedTrainingSession; legacy lifecycle и классы learner сохраняются. Математическая цель CapturedPerInput, anchor, допуски и измерительный критерий не меняются. Результат budget_exhausted остаётся отрицательным/неразрешённым исходом, не PhysicalInfeasible.

Регрессия в корневом Test_PulseLib_BoundedTraining.cpp проверяет обе топологии и обе recovery policies на valid observations при dense Rs mesh. Server before/after, native storage/Console A/B и вся обязательная матрица ещё ожидаются; source change пока не считается runtime-принятым.
