# Configured Train и явно поставленная измерительная проба

H11 root14e95660 /Pulse253fe870 выполнил209 native cases:207 passed,
два configured C-history cases упали до Train. Диагностика фиксирует
probe_status=queued, training.state=finished, outcome=invalid_model,
reason=training_capacitance_read_during_probe и пустую configuration.
Это не исчерпание физического времени обучения или доказательство несходимости.

Причина: ABeforeCalculate начинал automatic ControllerVersion2 Train при Idle
и поднятом IsNeedToTrain, не проверяя уже поставленную внешнюю native probe.
BindNativeTrainingCapacitance пытался захватить/прочитать C0 во время Queued,
запрещённого границей физического контроля. После ошибки проба оставалась Queued.
Предыдущая гипотеза о frozen evaluation не объясняла этот pre-Train отказ.

Исправление в обоих learner: automatic Start разрешается, когда probe не владеет
деревом, либо находится на подтверждённой AtEditBoundary: расчёт закончен,
временное routing/физическое состояние восстановлено и результат уже прочитан.
Queued/Running и непрочитанный результат не передают управление Train.
Сохраняющийся IsNeedToTrain является запросом automatic Train после этой границы;
снятый флаг не возобновляет Train. Активный training service по-прежнему владеет
своими пробами и не разрешает внешнему caller занять runner.

Изменяется только условие передачи управления перед штатным scheduler.
Нет вложенного Calculate, принудительного Build/Reset, записи физических
параметров на каждом тике или проверки bool lifecycle. Методы возвращают true,
ошибки сохраняют специализированную диагностику. C0 читается только на
законной границе, образцы storage и математическая цель не меняются.

Приёмка ожидается: четыре адресных Classic/Branch N0/N1 ownership cases,
повтор двух исходных отказов и весь native213/readers87; затем ASan и closure.
Наличие правки в исходнике не означает runtime-verified исправление.
