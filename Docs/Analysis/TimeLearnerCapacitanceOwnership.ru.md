# C₀ owner: кандидат после explicit API

Первый успешный bound edit сохраняет полное состояние C₀/scales в learner.
Обычные L/Ns/Rs edits и resistance-domain preview используют его автоматически;
subset C edit дополняет карту абсолютных scales, сохраняя прежние поправки.
Другую baseline, включая захват уже trimmed графа, owner отвергает до live edit.
Schema1 unbound primitive остаётся legacy API без автоматического provenance;
им нельзя обойти уже имеющееся bound ownership. CapturedPerInput и допуски не
меняются; автоматический C-trim search пока не включён.

Editor строит и проверяет полное capacitanceState на native preview до live
commit. Owner принимает готовый JSON вместе с проверенным физическим графом.
При rollback прежний state остаётся authoritative, обновляются child aliases.
Если фактический граф нельзя восстановить, сохраняется диагностика quarantine.
После Reset controller/probe boundary сбрасывается; физические параметры и C₀
не становятся необученными. Перед новым edit требуется завершённая новая probe.

Parameter state schema1 остаётся читаемой. Schema2 добавляет непустой
dormant_membrane_scales для ранее существовавших удалённых мембран; active и
dormant maps не пересекаются. Dormant path обязан иметь исходные channel
witnesses и отсутствовать в текущем графе. При regrowth карта заново разделяется
по preview-графу; текущая trimmed C не становится исходной C₀. Существовавшие
каналы восстанавливают индивидуальные C₀, новые пути получают C₀ из verified
storage images. Даже временно неиспользуемые исходные образы остаются в проверке
storage closure. Изменение prototype запрещено, per-tick repair не добавлен.

OwnedNativeCapacitanceState — явный getter; ImportedNativeCapacitanceState
сохранён как compatibility alias. Native learned export/import переносит карту;
полный XML/Copy checkpoint этой private metadata автоматически не обещается.
Все остальные native export/recognition frozen restrictions сохраняются.

Это непроверенный кандидат ветки codex/timelearner-capacitance-owner, не принятый
develop. В baseline Core8b41 owner state после bound edit пуст; исходный A1
смешал этот факт с двумя ошибками fixture. Corrected baseline A2 выполняется
отдельно без изменений Core. После него требуются regression explicit C₀ API,
ownership/reset/growth, shrink/regrowth/dormant codec, completed/stopped transfer,
frozen и failure/rollback controls, normal и ASan actual mappings. До их
завершения реализация не объявляется runtime-подтверждённой.
