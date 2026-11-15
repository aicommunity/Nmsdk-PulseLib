# N1: независимый от recovery обход geometry/Rs

В NextDomainCandidate режим RetrySameCandidate больше не удерживает весь Rs-grid одной длины перед остальными длинами. Для всех native N1 на каждом проходе выдаётся один ещё не проверенный Rs каждой legal geometry. Каждый повторный вход требует свежей ResistanceDomainAssessment; source/background/C0 binding и finite budgets сохранены. Политика обработки действительно invalid observations не меняется.

Изменение касается явного общего V2 BoundedTrainingSession; legacy lifecycle и классы learner сохраняются. Математическая цель CapturedPerInput, anchor, допуски и измерительный критерий не меняются. Результат budget_exhausted остаётся отрицательным/неразрешённым исходом, не PhysicalInfeasible.

Регрессия в корневом Test_PulseLib_BoundedTraining.cpp проверяет обе топологии и обе recovery policies на valid observations при dense Rs mesh. На Elder и M40 принят адресный C++ before/after: исходный обход даёт два ожидаемых отказа среди 98 случаев, исправленный проходит 98/98 на каждом сервере. Оба отказа проверяют одну новую семантику: геометрия не должна зависеть от политики восстановления invalid observations. Это не утверждение об успешном обучении физических моделей.

Скомпилированные PulseLib subjects: before `c10206d5e338792d42b58ab28e48e7364b07e481`, after `5398a5a563d3e358165fa855cd70236032f5b542`. В обоих используется один и тот же выделенный production codec CandidateRecoveryEvidenceJson; код не скопирован в тест. Выделение устраняет зависимость численного теста от native/Rdk заголовков. Общие JSON-примитивы переиспользуются через TrainingEvidenceJsonValues.h.

Доказательства, полные JUnit, actual loaded libraries и история неуспешных постановок A1–A3 сохранены в корневом commit `aef8038e65ef35418480682049dc82780ac5a9de`, документ `Docs/StructTrain/TimeLearner/N1GeometryFairness-20261111.ru.md`, primary `Reports/StructTrain/TimeLearner/N1GeometryDecisionAB-20261111`. Все процессы A4 закрыты, lease освобождены. Эта правка документации не меняет скомпилированный Core.

Полная SDK-сборка нового codec, native storage/Console A/B и вся обязательная матрица ещё ожидаются. Этап 7 и общий план остаются незавершёнными; unit-проверка не заменяет проверку реальной физики и Train→Test.

## Уточнение16.11.2026

Последний абзац выше описывает границу исходного11.11 снимка. После него M40 native SDK147/147 принят на candidate42e35, а Elder109/109 до/после подтверждает общий численный корпус. Это адресная проверка, не полная физическая/режимная матрица. Новое opt-in поведение только после sealed rejected-window описано в [N1RejectedCandidateCoverage](N1RejectedCandidateCoverage.ru.md); оно не меняет valid dense-Rs scheduling. Полные primary теперь хранятся во внешнем M40 архиве, прежние пути Reports являются историческими ссылками/метаданными.
