# BoundedTrainingSession

Внутренний контроллер `Core/TimeLearner/BoundedTraining` принимает захваченные
CapturedPerInput references, индивидуальные границы, исходные эффективные L/Ns/Rs
и fingerprint модели. NextAction выдаёт Edit, Probe или ResistanceDomain; адаптер выполняет действие
между пробами через штатные компоненты и подтверждает readback. Контроллер сам
не вызывает Build/Reset/Calculate и не меняет storage-образы.

В каждой ревизии кандидат получает новые пробы последнего опорного и активного
входов. Проверка данных общая с FinalVerification. Перебор ограничен бюджетами
проб, изменений, кандидатов и проходов; внешняя временная граница передаётся
через ExhaustBudget, явная остановка — через ExternalStop. N0 исследует целые
длины/Ns до 128, N1 — длины и конечную объявленную seed-сетку индивидуальных Rs.
Глобальная монотонность не предполагается. Последний вход фиксирован. Для Branch
позиции невозрастают; изменение общего кабеля инвалидирует все свежие измерения.

История кандидатов привязана к fingerprint/epoch и фактическим trial IDs.
Завершение локального поиска никогда не создаёт Exact: окончательный граф
обязательно проверяет отдельный FinalVerificationSession по всем входам.
Вне допуска после конечного числа проходов — SearchUnresolved, а не доказанная
физическая недостижимость. Успешность распознавания проверяется отдельно.

В feature N1 measured refinement добавлен `MeasuredResistanceSearch`: после
трёх разных согласованных измерений одной полной геометрии и normalized physical
scope предлагается геометрическая середина между соседними Rs с ошибками разных
знаков. Текущая domain source amplitude должна совпадать с immutable protocol.
Каждая точка заново проходит native edit/probe; final gate не меняется.
Немонотонность, timing-invalid точки или несогласованные повторы сохраняют mesh
fallback. N0, depth=0 и standalone callers без physical-domain ownership не
меняются. Полная спецификация и состояние серверных сборок находятся в корневом
`Docs/StructTrain/TimeLearner/MeasuredResistanceRefinement.ru.md`.
Runtime-приёмка этой feature ещё не завершена; наличие кода не объявляет Exact.

Текущий native service доступен через ControllerVersion=2; реализованы native
структурные адаптеры и versioned export/import в проверенной области. Полная
матрица моделей, C-trim и научные контроли остаются частью плана. Текущие
ограничения: корневые Docs/StructTrain/TimeLearner/NativeLiveFinalization.ru.md
и BoundedTraining.ru.md. Ниже приведены исторические проверки своих снимков.

Проверки в корневом репозитории: root 5f3737013ec3c18d3514f1d3292e1367181cb9e0
прошёл 29 addressed cases на обоих серверах, включая реальные коррекции Rs
Classic/Branch × 2000/20000 Гц (фиксированные длины, явный sync tolerance 0.2 с).
Root 067a4dba91b0e1a2bca83d8c73850baa9c0aa9b7 / PulseLib
c33085f37ad7ce64ec446e05765dab0b742fd81d прошёл 27 cases на обоих серверах и
27 M40 ASan cases с detect_leaks=0: controlled search, final gate, deadline,
native N0/N1 readback совпадающих Branch позиций. Эффективные параметры общего
банка должны совпадать у его входных меток; legacy requested Ns/Rs не заменяют
readback. При невозможности разделения позиций самостоятельная нормализация
связанного входа не разрешается скрытым изменением anchor.

Evidence: Reports/StructTrain/TimeLearner/s07-bounded-elder1 и s07-bounded-m40,
206 проверенных artifact hashes. Это не full CTest или полная матрица алгоритмов.

Native L/Ns/Rs adapter описан в [NativeModelEdits](TimeLearnerNativeModelEdits.ru.md).
Отклонённый preview подтверждается через DeclineEdit с неизменным live hash.
Для cold Branch с N>2 явное действие может также сдвигать ещё не посещённые
предшествующие точки, сохраняя порядок и разделяя несовместимые банки; anchor
и уже посещённые последующие входы остаются фиксированными. Стоимость включает
все изменения, и свежие измерения требуются после всего действия. Контроль
четырёх входов и native execution не заменяют полный configured Train.
Native N1 получает отдельные ResistanceDomain actions для фактической
геометрии и свежего preview; static mesh standalone callers не доказывает
физическую допустимость. Shared-bank singleton и separated geometry различаются.
Полная матрица физических профилей остаётся открытой.

## Измеряемое предложение N0

При улучшении измеренного rank после всех обязательных повторов, если timing
допустим, но amplitude вне допуска, контроллер предлагает соседние Ns на этой
полной геометрии. Оба соседа проходят прежнюю проверку bounds/shared banks и
новые anchor/input пробы. Сохранение timing и монотонность не предполагаются.
Неудачный сосед не порождает слепой обход Ns: без нового улучшения продолжается
базовый перебор длин. Уже выданные полные кандидаты не расходуют бюджет дважды;
required repeats и финальное свежее восстановление best не удаляются.

Baseline root31934548524ab0da77311f4fc046db05cdf69b81/Coref23d1b8d на M40
воспроизвёл потерю локального бюджета в двух controlled kernel cases. Новый
Corec25db29404b544311a3256336c0c43991a384e12 прошёл scoped regression
на обоих workers:31/31 native cases и4/4 actual-data checks, затем3/3 configured
N0-repeat cases и3/3 repeat-data checks. В каждом случае сохранены реальные
JSON/CSV и negative certificates; source root942e5df4559467a9a19c56755f945ce6501e7780.
ASan leak detection выключен. Временные отчёты покрывают4 base training phases
(56trials/84soma pairs) и2 configured repeat phases(48/72). Контролируемые kernel
fixtures подтверждают улучшение порядка предложений; реальные native controls
проверяют регрессии и остаются SearchUnresolved, не классификационным PASS.

Полная матрица N0/N1, physical applicability, quantized/best-effort semantics,
C-trim и научные контроли по-прежнему открыты. Подробные source SHA, primary
и visual receipts: корневой Docs/StructTrain/TimeLearner/N0CandidateOrdering.ru.md.

Впоследствии root develop merge `2b5af8c191b6ea20dff0f8b676e702db8c53c82f`
принял адресные physical N0 proposal controls на обоих workers и N=1/2/4/5
dimension controls. Измеренная последовательность L1/Ns1 → L2/Ns1 → L2/Ns2 →
L3/Ns1 подтверждена для Classic/Branch; все четыре proposal Train остаются
SearchUnresolved. Это проверка физического пути proposal, а не классификация
или доказанное преимущество policy. См. корневой NativeDimensionControls.ru.md.

## Адресный контроль N1 measured refinement

Актуальный helper/kernel PulseLib `521df5450c3655e0f08b3a01a7aed7d18351c250`
проверен normal a3 (40/40 выбранных C++ cases) и ASan a2 (32/32). Последний
ASan набор включает весь изменённый kernel и реальные read-only domain previews;
все десять native-domain regressions ранее прошли на a1 отдельного прототипа.
Leak detection выключен. Реальный ненулевой refinement пока проверен при
2000 Гц, двух входах и двух топологиях; это не полная матрица.

В одинаковых cold jobs Classic и Branch исходная finite mesh V2 давала
SearchUnresolved на обоих workers. Новый measured refinement дал Exact:
Rs[0]=64369082.70599112, L[0]=2, Ns[0]=1, опорный вход сохранён. Свежий
all-input gate измерил амплитудный остаток около -9.574e-7 при исходном допуске
1e-5 и timing около -0.019 s при допуске0.02 s. Собственные reference amplitudes
не уравнивались. Каждый worker прошёл 2/2 проверок первичных данных.

Primary proofs в корневом Git: normal
`bcbd30f1d87e352825e6c8400c5b929eff184ae5`, ASan
`44858d7fb92a330470f50741fb862f9f8db23840`. Полные temporal caches охватывают
292 пары probe/soma baseline и142 нового поиска; по три production report
checks, с actual input overlays. Все24 выбранных PNG просмотрены1920x960;
полная ручная проверка cache не заявлена. Visual source:
`e0adf29b2db4fb918e1c6a5c8a1994dfbb2a03cb`, отдельный selected QA в корне.
Эта проверка сходимости не доказывает классификацию и не является replay
августовских результатов либо сравнением legacy V1/V2.

## Native N1: измеренный локальный интервал и Branch growth family (13.11.2026)

Предыдущая версия9656b420 прошла77/77 controller checks. Реальный same-budget Branch A3(root4acc1f0ec) завершён, native completed/valid, outer test1/1 PASS2757.464s:52consumed candidates,55domain assessments,0rejections,5timing-ready и0joint-ready; BudgetExhausted, certificate=null. На геометрии[26,26,25,1] дляinput2 при86M измерены amp residual−0.0118549032499361/timing−0.015s, при2.880071555M residual+0.06956639592980918/timing−0.017s. Это измеренные opposite signs, а не PhysicalInfeasible или Exact.

Следующая реализация перераспределяет прежние конечные global budgets: один local normalization turn на geometry visit, allowance=min(12, remaining candidates/4, remaining probes/8, remaining edits/4). Probes считаются парами fresh anchor/input. Allowance выдаётся только после valid timing-ready amplitude miss, не пополняется от новых scalar/invalid probes; после конечной mesh/refinement серии обход продолжает другие геометрии. Valid endpoint с потерей timing не создаёт сертификат и не отменяет оставшуюся конечную mesh-проверку. Domain вновь оценивается для каждого нового кандидата. Все прежние global budgets, CapturedPerInput и tolerances сохранены.

Для Branch N1 length proposal допускает измеренные точки одного обычного или separated-bank семейства от одного background: только unvisited upstream lengths следуют max(background[j].length, activeLength+offset), offset0/1. Inactive Rs/Ns, anchor и committed downstream неизменны; варианты не смешиваются; C-context проверяется отдельно. Helper SameBranchLengthProposalFamily не утверждает физическую эквивалентность и не даёт measurement authority. Предсказанный length ограничен текущимL/2..2L и исходными bounds; он только меняет порядок legal candidates. Новые edit/readback/anchor/assigned-burst measurement и fresh all-input final verification обязательны.

Новые изменения пока не приняты на сервере. Root tests добавляют N4 coupled-growth positive control и исключение смешанных bank families, чужих Rs/Ns/downstream и nonfinite inputs. Legacy controller1, Rdk lifecycle/scheduler, storage physics и N0 не меняются. Scientific acceptance нового варианта требует собственного native same-budget Train; old77/77 и A3 не переименовываются в его PASS.

Уточнение следующего варианта: внутри подходящего измеренного bracket используется секущая по log(Rs), её доля ограничена[0.25,0.75] для конечного сокращения интервала. Magnitudes нормализуются до сложения для исключения overflow при больших конечных residuals. Три согласованные measured points, source/domain/geometry/timing guards и mesh fallback сохраняются. Интерполяция никогда не становится измеренным результатом: новый Rs проверяется настоящими пробами. Добавлены controls для log-affine и нелинейного response, ограниченного числа новых измерений и extreme finite residuals. Научная физическая сходимость этого варианта пока не проверена.
