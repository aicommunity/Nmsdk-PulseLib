# Максимум отклика одного предъявленного импульса: кандидат

Ветка `codex/timelearner-impulse-maximum`, исходная Core из storage/prepared SDK
`8702d264bc7b4f4c464e200f4a1092e55c6b44ff`. Адресные gates ниже подтверждают
часть нового кода; Console replay и адресный native ASan приняты отдельно.
Whole-burst и полная матрица ещё открыты. Прежние
native/ASan receipts SDK8702 не переносятся на него автоматически.

`ProbeSpec` schema5 явно выбирает `SingleImpulseMaximum`
(`single_impulse_maximum`). Это diagnostic-only single-pulse characterization:
предъявляется первый импульс исходного паттерна выбранной физической точке
подключения. Топология, полный training target, storage physics и их владельцы
сохраняются; используется прежний native presentation/reset/restore путь.

После закрытия полного окна recorder выбирает самый высокий измеренный пик.
Меньшие локальные максимумы сохраняются в candidateCount и не делают выбор
неоднозначным. Раздельные равные наибольшие пики дают Ambiguous; непрерывное
плато сохраняет первую/последнюю фактические samples. Boundary clipping,
incomplete/nonfinite coverage, missing stimulus и изменение revision не дают
валидного наблюдения. Пик до фактического импульса остаётся измеренным, но
пакет содержит `peak_precedes_stimulus` и не является валидным ответом.

Schema4/IsolatedWindow сохраняет прежнее требование одного локального пика.
Старые схемы и OrderedWholeBurst не переоцениваются. JSON encoder/decoder
сохраняют явную политику; schema5 без single-pulse purpose либо с несколькими
stimuli отвергается. Protocol/CapturedReferences/FinalVerification не принимают
диагностические schemas4/5 для сертификации Train.

Это исправление общего выбора максимума в однозначном causal случае, не
решение атрибуции пиков штатной пачки. Для Train нужен максимум закреплённого
импульса; глобальный максимум пачки его не заменяет. Для Test используется
общий максимум полного паттерна и реальные выходные спайки. Включать schema5
как substitute Train, менять CapturedPerInput, defaults или порог запрещено.

Обязательная адресная приёмка: поздний более высокий пик после спада >15%,
меньшие экстремумы, плато/равные maxima, clipping/incomplete, peak до onset,
сохранение schema4 и запрет неправильных schemas; native Classic/Branch ×
2000/20000 с A/B одинаковой физики и восстановление при трёх fault boundaries.
Затем отдельный Console replay архивных многопиковых single responses с
независимым Python reader и графиками. До полного набора этих проверок
кандидат не рекомендуется для новых production experiments.

## Выполненные адресные gates

Root `62997ad1e7cb7869012d11da82b5a48b529ac1e7` / PulseLib
`24f9767ace1f498d5dc048a70fc303789adac757` (эта Core6c520fb):

- M40 observer unit34 под ASan, включая семь новых controls; production
  ProbeObservation.cpp компилируется непосредственно, копии тела нет.
  Evidence корня `05083f7adc476bed4318ab069a70e33727f370db`,43 members,
  actual compiler commands и loaded libasan проверены; physical integration
  этим executable не выполнялась, leak check выключен.
- Elder Release SDK:38 C++ tests,25 readers и отдельный unit34, без skips.
  Evidence `a822275717754818668124c39cc02777104ecded`,295 members,
  496 root/265 Pulse hashes, восемь flags/link files и два actual process
  bindings сверены. В четырёх native N2/high-threshold A/B парах5284 frames
  совпали по stimulus/soma точно, часы — с roundoff1e-12. Это не stock
  low-threshold многопиковая проверка и не научный результат классификации.

Шесть schema5 fault captures сохранены раздельно, новые readers их проверили.
Исходная Elder A1 имела37/38 и12 ошибок создания одноимённых test artifact
files; её отрицательный evidence `240d2179aa513117ca55009d8a4e4aadda3e6c90`
сохранён, не переименован в PASS. Исправление относилось к именованию файлов
и reader corpus, а не к изменению физики ради прохождения проверки.
Корневые maintained отчёты: `Docs/StructTrain/TimeLearner/`
`ProbeAlgorithmsASan.ru.md`, `SingleImpulseMaximumElderAcceptance.ru.md`.


## Console replay и физический ASan: последующая приёмка

Normal Console Root038/Pulse217 прошёл четыре реальные Classic/Branch
операции на архивных multi-peak single responses при исходном threshold0.0117.
Exact archived/A/B waveforms и effective revisions сохранены; новая политика
выбрала наибольший из3/2 пиков. Первая admission failure сохранена отдельно:
Root9f9de91 исправляет только idle training-owner конфигурацию, оставляя schema5
transient diagnostic и запрет training certificate. Повтор не меняет физику.
Evidence корня `e7b73e1fdc6c5d626604eb0e841adfb95d00ea70`.

На M40 compiled Root `038a66413b7a41e75bf3826eda365db1c78c364b` / PulseLib
`217dc53c4b1cc010ad6938852a91ee6d2df09e3c` завершил native38, readers25 и
observer unit34 без failures/errors/skips. Это26 прежних storage/prepared,
7 recorder и5 new native cases. Последние используют N2, две топологии,
2000/20000Гц, explicit threshold100 и три fault boundaries для каждой топологии.
Native C++ A/B и independent readers проверяют actual samples, не bool API.

Evidence `b0864d9c3c5eb6e5bb25f514a6133e48a12894ec`:
295 primary members/664828 compressed bytes;498 Root/265 Pulse source hashes,
73/19 actual loaded files native/unit и8 generated flags/link files сверены.
Обе actual mappings содержат libasan, compiler flags имеют address sanitizer.
Native38 занял1124.73s; unit34 повторяет observer cases и не добавляет34
независимых физических условия.68 file bindings шести прежних runtime
неизменны, свежая whole-group closure и released lease подтверждены.
Requested detect_leaks=0; live readback опции отсутствует, проверка утечек не
заявляется. Producer terminal оставляет sanitizer acceptance unattested;
отдельные collector/independent receipts подтверждают instrumentation/mapping,
не переписывая producer. Console в этой ASan SDK только собрана, не испытана.

В корне: `SingleMaximumConsoleReplay.ru.md`,
`SingleImpulseMaximumPhysicalASan.ru.md`,
`SingleImpulseMaximumPhysicalASanVerification.json`.
Новая размерная матрица N1/4/6 добавлена после этого compiled snapshot и требует
собственных normal/ASan receipts. Full-burst attribution, остальные physics/
normalization modes, convergence и классификация этим gate не закрываются.

Пользователь предпочёл консервативно считать одну слившуюся вершину двух
откликов неоднозначной, отметив научную неопределённость. Она не становится
двумя независимыми закреплёнными измерениями из-за пересечения окон.
Это не доказательство отсутствия физического ответа. Меньшие дополнительные
пики одиночного отклика остаются допустимыми при уникальном наибольшем пике.
