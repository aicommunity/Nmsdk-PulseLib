# BoundedTrainingSession

Внутренний контроллер `Core/TimeLearner/BoundedTraining` принимает захваченные
CapturedPerInput references, индивидуальные границы, исходные эффективные L/Ns/Rs
и fingerprint модели. NextAction выдаёт Edit или Probe; адаптер выполняет действие
между пробами через штатные компоненты и подтверждает readback. Контроллер сам
не вызывает Build/Reset/Calculate и не меняет storage-образы.

В каждой ревизии кандидат получает новые пробы последнего опорного и активного
входов. Проверка данных общая с FinalVerification. Перебор ограничен бюджетами
проб, изменений, кандидатов и проходов; внешняя временная граница передаётся
через ExhaustBudget, явная остановка — через ExternalStop. N0 исследует целые
длины/Ns до 128, N1 — длины и конечную объявленную сетку индивидуальных Rs.
Глобальная монотонность не предполагается. Последний вход фиксирован. Для Branch
позиции невозрастают; изменение общего кабеля инвалидирует все свежие измерения.

История кандидатов привязана к fingerprint/epoch и фактическим trial IDs.
Завершение локального поиска никогда не создаёт Exact: окончательный граф
обязательно проверяет отдельный FinalVerificationSession по всем входам.
Вне допуска после конечного числа проходов — SearchUnresolved, а не доказанная
физическая недостижимость. Успешность распознавания проверяется отдельно.

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
