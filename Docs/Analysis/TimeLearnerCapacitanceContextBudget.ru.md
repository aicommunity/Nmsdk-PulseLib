# Удаление доказанных семантических дублей C-context

Исходный Core198dd5b допускает maximumContexts2, но занимает changed slot
absolute C01 даже при уже unit selected paths. Поэтому при таком бюджете
C0 min/max не исследуются. Это не отменяет scoped J2 27/19 при другом бюджете.

BeginInput теперь получает один запасной seed и пропускает только доказанный
no-op полного C-state на всём возможном диапазоне текущего input. Classic —
[1,maxL_i]. Branch — (текущая downstream position, maxL_i], с консервативным
ограничением maxL_i upstream upper bounds. Downstream/anchor не меняются.
Непустой warm correction внутри диапазона сохраняет действующий reset к1;
поправки снаружи остаются неизменными и не занимают дублирующий slot. Для пустого
Branch interval нет дополнительных C/domain contexts.

Проверка обходит только известные canonical membrane paths, не разворачивает
кабель и не строит фиктивные C0. Неизвестные будущие paths имеют unit multiplier
своего native storage C0. Длинные decimal indices обрабатываются без integer
overflow. Возможный диапазон консервативен: если noop не доказан, context остаётся.
Следовательно, это не доказательство полного обзора всех возможных joint states.

Число live contexts остаётся не выше прежнего preflight upper bound, общий
maximumCursorElements распределяется по фактически сохранённым contexts.
Default Off, цели CapturedPerInput, tolerances, порядок scheduler/lifecycle,
механизм native edits/C0 readback и policy schema4 не меняются.

Три новых original GTest controls: minimal-budget Classic/Branch N0/N1,
warm unit-reset и empty Branch interval. Старый и исправленный production
kernel должны проверяться на одинаковом151-case корпусе; old failures и
corrected positive/Off137 A/B сохраняются отдельно. Native/ASan/mode matrix
остаются обязательной дальнейшей приёмкой, не заменяемой pure test responses.
