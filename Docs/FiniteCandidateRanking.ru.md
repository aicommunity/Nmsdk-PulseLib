# BoundedTraining: конечное сравнение рангов кандидатов

Нулевой допуск остаётся точным требованием. Ранее все ненулевые остатки
получали infinity и проигрывали по стоимости изменения исходной геометрии,
даже когда новая геометрия измеренно лучше. Положительный очень малый допуск
терял различия при переполнении error/tolerance.

Rank теперь сравнивает последовательно: нарушение timing readiness,
нарушение amplitude readiness, действительный модуль ошибки для sync=0,
действительный модуль амплитудной ошибки для amplitudeTolerance=0,
нормированную сумму для положительных допусков и стоимость изменения.
Ошибки разных размерностей при нулевых допусках не складываются;
timing имеет приоритет. Никаких epsilon/percent/timestep floors нет.

Обычная конечная нормированная сумма не меняется. При её переполнении
используется log-sum-exp тех же положительных слагаемых. Конечная сумма
всегда предшествует переполненной. Реализация не предполагает, что long double
шире double на целевой платформе.

Это меняет предложение/восстановление best measured candidate, но не
withinTiming/withinAmplitude, CapturedPerInput, bounds, физику, final gate
или терминальные статусы. Exact по-прежнему требует свежей проверки всех
входов. Corrected baseline8/8 expected failures и regression81/81 на Elder
normal/M40 ASan опубликованы в родительском репозитории. Дополнительно4/4
реальных native multi-length cases прошли на обоих workers: прежний Core
сохранял timing error70.5ms при измеренном61ms, correction выбирает61ms.
В этом engineering control amplitude tolerance1 задан явно; Exact не получен.
Обычные defaults и CapturedPerInput не менялись.

Root evidence: normal81 f9e823ced7d51614a2a1af307405bbdaaac0478f,
ASan81 b9ce935628f758fbee8c7a546b0f169b3122ad93,
normal physical4 f5e3bccacce748960f67b53afa5574f980bc4f7c,
ASan physical4 41db2c7641c41a934dec401bf11df09ef8f1665b.
Все48 physical CSV byte-equal между workers;48 packets semantically equal.
Full temporal cache96 probes/144 soma pairs/864 PNG хранится на M40;
14 selected originals просмотрены1920x960, root proof48a8b1be28c6a60bd8c280159e51122be8ddd410.
Полные границы, исправленный initial timing fixture и raw-derived readers:
родительский Docs/StructTrain/TimeLearner/FiniteCandidateRanking.ru.md.

Адресная проверка не завершает весь stage7, исходный план0–12 или приёмку
всех вариантов prerequisite features. Адресные prerequisite gates завершены:
root44548f908365f5030445e4ced11a71147d044331 содержит ASan163/163,
recording18/18, public diagnostic6/6 и readers32/32. Normal результаты и
temporal review опубликованы отдельно. Это достаточно для scoped merge
проверенных исправлений, но версия2 в целом не объявляется рекомендуемой.
