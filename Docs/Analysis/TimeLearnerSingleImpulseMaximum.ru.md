# Максимум отклика одного предъявленного импульса: кандидат

Ветка `codex/timelearner-impulse-maximum`, исходная Core из storage/prepared SDK
`8702d264bc7b4f4c464e200f4a1092e55c6b44ff`. Адресные gates ниже подтверждают
часть нового кода; Console/whole-burst и полная матрица ещё открыты. Прежние
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
