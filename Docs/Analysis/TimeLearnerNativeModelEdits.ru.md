# NativeModelEditor: структурные действия между пробами

Оба TimeLearner предоставляют ReadNativeParameters/ApplyNativeParameters.
Общий Core/TimeLearner/NativeModelEditor изменяет эффективные L/Ns/Rs только
на границе завершённой, полученной caller native-пробы. Это API адаптера,
ещё не конфигурационное включение ControllerVersion=2.

Кандидат проходит native Save/Load preview в том же storage, проверку реальных
связей, неизменяемых физических параметров и поддерживаемых дискретных уравнений.
Затем применяется к живому нейрону штатными structural setters/Build. Вложенного
Calculate, whole-learner Build/Reset, изменения прототипов и исправляющих записей
каждый tick нет. Проба после изменения сбрасывает физическую динамику штатно;
предыдущие измерения не становятся сертификатом новой модели.

Владение ограничено input-банками и объявленными inhibitory targets. Чужие банки,
их внутренние связи, выделенный extra cable и неоднородная физика сохраняются.
Удаление источника внешнего наблюдателя отклоняется до live apply: проверяются
native connectors, property reverse cache и реальные inbound bindings owner tree.
Нельзя предполагать, что один reverse cache описывает все способы создания связей.

Rejected preview сохраняет live hash; Failed после начала изменения означает
modelMayHaveChanged и блокирует дальнейшие пробы/расчёт до ремонта. Bool lifecycle
не превращается в канал диагностики. Полная транзакция PostTune ещё нужна.

AssessNativeIntegration читает конкретные NPulseChannel/NPulseSynapse, реальные
Weight, число входных slots, averaging, PSI и секрецию. Проверка локального
дискретного шага не является доказательством спектральной устойчивости всей сети.
Неподдерживаемая физика или динамически управляемые коэффициенты получают явный
отказ. N1 вычисляет индивидуальные интервалы Rs для фактического графа; общего
пересечения по всем входам не требует.

Проверено на серверах: root d06c51012d21141d12556e500db81c68e35804ce — Classic/
Branch × N0/N1 × 2000/20000, отдельный tight-burst negative control; root
b0cd47e4acc335416bff8c3a516d446cfafbed83 — 35 адресных cases на обоих серверах
после исправления external-observer guard. Root
bbc81c3d857ceedc188275087354a31f74c71bdb — 36 cases на каждом сервере и M40 ASan
с detect_leaks=0. Root 4a27a579b5fff61e025ed200f627aa0482f06e1a добавляет native
четырёхвходовой coupled edit: [1,1,1,1] → [2,2,2,1], actual bank readback и свежая
проба; он прошёл на обоих серверах. Полные наборы этого снимка ещё проверяются.

Детальный журнал неудачных и успешных попыток, границы приёмки и ссылки на
Git evidence поддерживаются в корневом Docs/StructTrain/TimeLearner/
NativeModelEdits.ru.md. Это не selectivity experiment, historical replay,
полный trained-state export или принятие всей физической матрицы.
