# Конечность временных параметров TimeLearner

В legacy timing setters обоих learner требуется конечное значение.
IterationGap, SyncTolerance, PeakMeasureMargin и DelayAgreeMarginMin сохраняют
область >=0. Classic EstDelayPerSeg остаётся строго положительным. Branch
InitEstDelayPerSeg допускает конечное значение <=0 для прежнего fallback0.005;
новый setter не меняет момент копирования seed в приватную адаптивную оценку.

Ошибка передаётся штатным отказом property setter / EPropertySetterFail,
до записи значения. Это не bool lifecycle Build/Calculate/Reset; контракт
этих методов данным изменением не меняется. SetDataDirect и внешние bindings
имеют собственную семантику Rdk и не объявляются проверенными этим setter.

Исходная native-проверка на Elder: root
`c9e82b7a4aabda5d4eca2d06cd60b14a23082f1c`, PulseLib
`70f467d3b66ea4defdec2f58b84e76b692ab3602`; четыре cases Classic/Branch ×
2000/20000 Гц, два последовательных TakeObject/ReturnObject, по30 назначений
NaN/+Inf/-Inf на case. Все4 cases выявили нарушение: из120 назначений84
были приняты. Исходные process_error/returncode1 сохраняются как baseline,
а не как успешный тест. Проверка исправления normal/ASan ещё не завершена.

Configured V2 берёт окна и tolerances из строгой ControllerConfiguration,
а не из этих legacy timing properties. Проверка property setter не заменяет
проверку production JSON parser, алгоритма поиска или полной физической матрицы.
Ветви false внутренних legacy ABuild/AReset не означают false публичных
Build/Reset: UContainer не читает bool этих hooks. Остаточную согласованность
hooks следует рассматривать отдельно, сохраняя раннюю остановку и диагностику.
