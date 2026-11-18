# Выбранная измеренная граница ограниченного поиска

Статус18.11.2026: подготовлен read-only API контроллера; серверные проверки
нового API ещё не выполнены. Native capture/restore/export этой границы пока
не подключены. Глобальный возврат лучшей модели не объявляется реализованным.

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
восстановление. Native adapter должен захватить фактический полный граф при
наблюдении нового serial, проверить revision/epoch/параметры/контекст и иметь
отдельную законную terminal transaction. Старый composite actor не становится
действующим снова. Перенос owner mirrors/C-control, учёт edit budget, событие
возврата и согласованный model export остаются отдельной незавершённой работой.
Наличие выбранной точки не означает Exact; нужна свежая all-input проверка.

Подготовлены три kernel-regressions в корневых штатных интеграционных тестах:
переход к final verification без потери selection; новый неизмеренный edit и
budget stop; отказ публикации незавершённых N0 repeats. Они используют реальный
ProbeRecorder и production BoundedTrainingSession, но управляемый физический
отклик. Даже их будущий PASS не подтвердит native graph restoration.
