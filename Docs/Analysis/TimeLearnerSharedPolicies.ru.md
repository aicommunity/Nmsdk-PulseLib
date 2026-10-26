# Общие политики legacy-контроллера TimeLearner

`Core/TimeLearner/LegacyController.h/.cpp` содержит общие вычисления для
Classic и Branch. Это механическое выделение прежней логики; оно не вводит
ControllerVersion=2 и не исправляет математическую постановку одновременно
с переносом кода.

| Функция | Вход и результат | Что остаётся у топологии |
|---|---|---|
| ChooseDelay | Выбор измеренной либо модельной задержки по прежнему gate согласия | Расчёт модели кабеля, выбор входа и передача допусков |
| ComputePeakTimes | Векторы PeakRel и DelayFromPulse из абсолютных времён | Привязка волн к импульсам, окна, validity и накопление пиков |
| DetectPulses | Разница счётчика либо edge fallback; обновление предыдущего уровня | Получение реального генератора, время начала пачки и управление пробой |
| ProposeResistance | Предложение Rs и effective gain с прежним overshoot/undershoot damping | Per-input ClampResistance, применение к модели и debug orientation |

Все физические параметры и константы policy передаются явно. Общая часть
не владеет Rdk-объектами, не выполняет Calculate/Build/Reset, не меняет
storage-образы, не применяет Rs и не создаёт общего интервала сопротивлений
разных входов. Обе topology wrappers сохраняют ClampResistance и текущую
последовательность вызовов.

Сохранены и ограничения старого алгоритма. Fallback ChooseDelay является
предсказанием, а не подтверждением измеренного пика. Нулевые элементы
ComputePeakTimes при missing peak не являются v2-valid наблюдением.
DetectPulses сохраняет прежнее поведение при уменьшении/reset счётчика:
previousCounter не уменьшается и может использоваться edge fallback.
Эти решения не следует переносить в новый проверяемый контроллер как
нормативный алгоритм. Их исправления требуют отдельного изменения и проверки.

CapturedPerInput, fixed reference N−1, Classic round-robin, Branch reverse/mute,
N0/N1, PostTune и conversion этим переносом не изменяются. Независимая свежая
финальная проверка всех входов, revision invalidation, bounded search,
TrainingCertificate и полный export/checkpoint остаются отдельной работой.

Адресные checks связываются с production-функциями этого файла; копий тела
алгоритма в тестах нет. Приёмка механического переноса требует также native
A/B Classic/Branch × N0/N1 × 2000/20000: actual stimulus timestamps, полный
binary64 soma hash, последовательность решений и effective graph должны
совпасть для одной и той же физики. Трёхсекундный fixture не является
завершённым обучением или историческим replay августа.
