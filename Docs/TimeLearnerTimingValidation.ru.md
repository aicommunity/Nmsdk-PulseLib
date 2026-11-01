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
а не как успешный тест.

Исправление Core `ecf35a6c322f58fc9c4ffdfb998bf882f990252a` принято адресно:
на каждом worker4/4 property cases и21/21 native lifecycle/copy/XML/scheduler
regressions без skips;120 неконечных назначений отклонены с сохранением значения.
Normal primary — `fff3d9ddf116c39abaf22ab5fa8b13025efd350b`, ASan —
`afe5bdf4b56deddf4a6021853b1ad76d145a494e`. Первоначальный ASan regression
завершился timeout1800 s; повтор только21 regressions на том же frozen runtime
с лимитом3600 s прошёл за1997.296 wall s. Оба исхода сохранены, runtime не
пересобирался; это не независимые обучения. Root acceptance фиксирует child
merge с тем же tested Core. Полные режимы/профили и остальные свойства открыты.

Configured V2 берёт окна и tolerances из строгой ControllerConfiguration,
а не из этих legacy timing properties. Проверка property setter не заменяет
проверку production JSON parser, алгоритма поиска или полной физической матрицы.
Ветви false внутренних legacy ABuild/AReset не означают false публичных
Build/Reset: UContainer не читает bool этих hooks. Остаточную согласованность
hooks следует рассматривать отдельно, сохраняя раннюю остановку и диагностику.
