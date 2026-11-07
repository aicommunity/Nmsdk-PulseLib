# Максимум отклика одного предъявленного импульса: кандидат

Ветка `codex/timelearner-impulse-maximum`, исходная Core из storage/prepared SDK
`8702d264bc7b4f4c464e200f4a1092e55c6b44ff`. Изменения ещё требуют серверной
приёмки. Прежние native/ASan receipts этого SDK не подтверждают новый код.

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
независимым Python reader и графиками. До этих проверок кандидат не принят.
