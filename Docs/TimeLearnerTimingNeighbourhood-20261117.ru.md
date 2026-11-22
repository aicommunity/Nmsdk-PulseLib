# N1: соседние длины после временной готовности

Статус: кандидат реализации; серверные C++/native проверки ещё не пройдены.
Работающие cold-опыты остаются на прежних immutable runtimes.

В полном cold Classic17.11 сначала измерены L1,2,3, затем прогноз предложил
L241. Несколько сопротивлений этой геометрии удовлетворили timing tolerance,
но не исходному amplitude tolerance; даже Rmin оставил остаток−0.00329223243
при1e−5. Затем поиск вернулся к L4…50. Это не доказательство недостижимости:
соседние с241 длины в этом бюджете не измерялись.

`ProposeMeasuredLength` правильно не выдаёт числовую «коррекцию задержки»,
если уже есть timing-ready точка. Но отсутствие временного прогноза не должно
автоматически означать отказ от локального исследования геометрии совместной
задачи. В `PrioritizeMeasuredLength` введён отдельный N1 proposal fallback:

```text
if no measured timing proposal and current observation is timing-ready
   and amplitude is outside original tolerance and native N1 domains enabled:
    exclude already visited geometry-prefix lengths and current length
    propose nearest remaining integer (smaller length wins a tie)
    skip illegal shared-bank candidates within a finite proposal allowance
    enqueue next geometry after the current finite Rs turn
ordinary controller:
    reassess effective Rs domain -> native edit/readback
    fresh anchor -> fresh assigned-input observation
    compare both original residual tolerances
```

Порядок не предполагает монотонность амплитуды или задержки. Не меняются
цель CapturedPerInput, Rs bounds, ограничения структуры, бюджет кандидатов/
проб/edits, N0, существующий Rs turn, C contexts и независимый final gate.
Длина — только предложение; сосед не наследует старое измерение, domain,
peak assignment или certificate. Branch сохраняет ограничитель displacement
и штатную проверку общего кабеля. Невалидная геометрия не становится скалярной
оценкой или PhysicalInfeasible.

Nearest helper работает по множеству выданных длин, не перебирает весь integer
domain; INT_MAX endpoints обрабатываются через int64. При исчерпании ограниченного
proposal turn остаётся обычный конечный cursor. Это не исправление terminal
best-state restore и не гарантия нахождения глобального решения.

Корневые production C++ tests должны проверить nearest ties/bounds/INT_MAX,
контроллеры Classic/Branch со свежими domain и anchor/input observations после
amplitude miss, а также неизменный N0 порядок. Controlled-response fixtures
проверяют state machine, не работу физического нейрона. После них необходимы
immutable SDK и отдельный actual cold A/B с сохранённой физикой/допусками.
