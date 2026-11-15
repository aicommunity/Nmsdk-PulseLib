# Кандидат объединения C-search и construction threshold

Подготовленная merge-ветка от Pulse develop9ec4566 объединяет C-search
84e456d6fb551021e0222ebaf51fa313df2623b4 через --no-ff. Текстовых конфликтов
не возникло; это не доказательство семантической совместимости и не runtime PASS.
В develop этот кандидат пока не принимается. Работающий K1 использует прежний
84e456d с отдельным source/runtime и не проверяет новое объединение.

Обе версии NativeModelSpec сохранены: schema1 наследует storage threshold,
schema2 требует явный initial_threshold. Внешняя cold configuration schema4
отдельно включает capacitance_search; codec вызывает общий ModelSpec decoder,
а не подменяет его schema outer-версией. Construction threshold применяется
один раз после сборки, с effective post-Init readback. C-search хранит отдельно
C0 origin/policy и абсолютные per-membrane scales. Общие prototypes не меняются.
Ни repeated Build, ни import не должны возвращать поздний threshold к initial.

До приёмки обязательны реальные совместные проверки на обоих workers:

- прежние18 threshold/archive cases и8 calibration preconditions;
- прежние C-search34/262 и corresponding primary readers, C Off как база;
- явные сочетания outer schema4 × ModelSpec1/2, Classic/Branch ×N0/N1;
- C Off и C On с initial100 и более поздним threshold0.005;
- native Train/source export/import, recorded C0 и dormant heterogeneous
  channels, effective post-Init fields, отсутствие повторного threshold/C scaling;
- final solo/recognition graph и calibration не получают старый Exact
  certificate автоматически после физического изменения;
- единый родительский Rdk10e9, Bin99 и root Console/probe integration.

CapturedPerInput, последний опорный вход и исходные tolerances сохраняются.
Успех сборки или исходного K1 не принимает эти новые сочетания; при отсутствии
полной пробы остаётся explicit unresolved/invalid, без вымышленных пиков.
Root counterpart и новые совместные fixtures ещё предстоит подготовить.
