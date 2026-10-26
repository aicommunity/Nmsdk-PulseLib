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

Search API представляет одно Rs на входной банк. Неоднородные исходные Rs банка
отклоняются как неподдерживаемое представление, сохраняя фактическую модель.
У представимого банка learned Rs применяется ко всем синапсам, в том числе
новым из storage; другие параметры существующих членов сохраняются. Effective
readback проверяется на preview до live apply. Root
1e980f9103920c6012afe810d1e4be23929c80dc прошёл 43 адресных cases на каждом
сервере, включая N0 bank Rs + Ns growth при 2000/20000 и отказ неоднородному
исходному банку. M40 ASan: 4/4, detect_leaks=0. Это отдельный tested snapshot.

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
проба; он прошёл на обоих серверах. Четыре поддерживаемых CTest suites этого
снимка прошли: PulseLib 293 pass + 9 skip из 302, другие suites — 18 pass.
Opt-in compatibility и process helper skips не считаются доказательством.

Детальный журнал неудачных и успешных попыток, границы приёмки и ссылки на
Git evidence поддерживаются в корневом Docs/StructTrain/TimeLearner/
NativeModelEdits.ru.md. Это не selectivity experiment, historical replay,
полный trained-state export или принятие всей физической матрицы.
