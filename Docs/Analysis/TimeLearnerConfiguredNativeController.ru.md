# Configured TimeLearner V2: cold JSON/XML

`ControllerVersion=1` — default и прежние конфигурации; `2` — native cold
declaration из `ControllerConfiguration`. Schema JSON отдельна от номера
контроллера: 1 сохраняет исходный протокол (одна проба кандидата N0), 2 требует
`search.n0_candidate_repeats` от 2 до 128 и разрешена только для N0. N1 с этим
фактором отвергается. [Повторы N0](TimeLearnerN0CandidateRepeats.ru.md) описывают
измерения и отдельную schema 2 training summary; эти номера не меняют схему
primary observation. `ConfiguredNativeTraining` содержит storage ModelSpec, реальный
StimulusProfile, начальные ISI, окна, допуски и конечные бюджеты. Кодеки и native
plan builder находятся в Core/TimeLearner/ConfiguredNativeTraining.

Оба компонента строят модель через общий native builder и запускают общий service
из Before hook штатного scheduler. Дополнительных Calculate нет. Повторный Build
проверяет готовый граф, а не возвращает исходные Rs/C. Внешнее снятие Need не
порождает Exact. Ошибки передаются typed состоянием/диагностикой; bool публичных
lifecycle методов не служат error channel.

Новый JSON не является checkpoint: он не содержит callbacks, динамику,
references/certificate и определения storage images. Образы должны существовать
в runtime через штатную регистрацию. Неизвестные поля, неверные типы, дробные
счётчики и дубли JSON keys (также escaped) отвергаются. После cold creation
декларация фиксирована; configured retarget/reconfiguration ещё требует реализации.

ConfiguredModelInitialized — публичный сохраняемый Parameter|State marker.
Parameter нужен из-за Copy(false): Rdk вызывает Build до копирования State.
На новом экземпляре Load/Copy может восстановить параметры в любом порядке;
при marker=true и отсутствии process-local declaration Build удерживает
`configured_trained_model_requires_explicit_state_import`, сохраняя физический
граф. Это защита от потери модели, а не готовый trained-state import.

Root cb9e035b390390746ada5caa2c500917b8884612: на каждом сервере 14/14 configured
controls и 21/21 addressed native regressions; прежняя широкая A2 — 97/97.
Проверены Classic/Branch × N0/N1, настоящее cold XML обучение без Begin API,
saved XML и Copy(false/true) guards с неоднородной child C, forced Build/Reset,
codec roundtrip/corruption. M40 ASan: A2 12/12 и A3 9/9, detect_leaks=0.
Независимые 18 Python tests проходят отдельно на assigned и isolated данных
на каждом сервере. Исходные два no-op corruption failures сохранены вместе
с исправлениями; правила production auditor не ослаблялись.

Полный журнал, Git-пакеты и открытые требования находятся в корневом
Docs/StructTrain/TimeLearner/ConfiguredNativeController.ru.md. Полный trained-state
transfer, dynamic Rs domains, optional C-trim/PostTune/calibration, production
experiment pipeline и научная матрица ещё не завершены. Все шесть проверенных
configured/XML поисков дают честный SearchUnresolved с отрицательным сертификатом.
