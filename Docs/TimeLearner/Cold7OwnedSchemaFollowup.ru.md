# Cold7: owning-value follow-up

Статус: исходное исправление, без локальной компиляции, численных тестов или runtime-приёмки.

Обычный merge baseline Pulse `4a5c2ec9df9a9f704dcd8260714323494bfa4415` в исходный candidate-window Pulse `b2878e565febe20a4ba7c9c64536d6e66f672626` сохранён отдельным коммитом `ff787837d1835497a9b022fb64ebf9f34c676b95`. Baseline cold6 при этом не получает будущие Train5/owner-token изменения.

Этот follow-up исправляет ту же self-object proxy ошибку в отдельной cold7 ветке encoder: числовой `schema_version` копируется в owning `QJsonValue` до вставки `base_schema_version`. Старые schema1–6, decoder, строгая проверка числовых типов, протоколы, assignment, физика и tolerances не меняются. Recovery5 уже использовал owning value и остаётся прежним.

Запланированные ROOT controls используют реальную модель Classic/Branch, плоские base schemas1–4, production encode/decode, Compact JSON bytes и production parser. Они отдельно проверяют сохранение всех flat полей и отказ на bool/string/null вместо numeric base. Это configuration portability, без утверждения об успешном Train, C0 измерении или живом admission Train5. Опубликованные ошибочные JSON остаются неизменными.
