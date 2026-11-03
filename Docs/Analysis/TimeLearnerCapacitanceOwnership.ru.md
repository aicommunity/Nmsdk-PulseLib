# C₀ owner: подтверждённый normal/ASan scope, 03.11

Production Core30495d143795022ad444bf290d2ac30b7e140199 проверен:

| Контроль | Результат | Evidence root commit |
|---|---|---|
| Normal native + independent readers |43/43 +26/26 PASS|7ec395554f0f8d506c08b587ba130025d0af1719|
| ASan native + independent readers |43/43 +26/26 PASS|82f205d733dc64513ff3dd22ede7df3f9ab7a445|

Normal primary1062 members и corrected reader11 members проверены независимо;
normal C++/CMake/gitlinks совпадают с rootc63d1b7b9c08c7542398ca944bcc7cf22e5ea2b4.
ASan выполнен непосредственно на этом root:1073 members/7814429 compressed bytes
проверены, libasan.so.8 подтверждён actual loaded-files mapping. detect_leaks=0;
проверка утечек не заявляется. Исправленный отрицательный Python control удаляет
реальную nonunit correction; прежний неуспех сохранён, numerical data не исправлялись.

Scope: Classic/Branch N0/N1,2k/20k native edits/bounds, joint C, rollback, dormant
history, completed/stopped transfer, frozen/fresh-process responses в заявленных43
cases. Это готовность к отдельной приёмке C₀ ownership/transport, не полные этапы
4–8, не automatic C-search и не качество распознавания. Новые facade/witness/service
APIs, tracked history и model/configuration schema3 после30495 имеют собственную
неоконченную проверку и этим результатом не принимаются. Report/cache/selected
waveform материалы имеют отдельный source scope в корневом CapacitanceOwnership.

Ниже сохранён технический контракт; CapturedPerInput, tolerances и lifecycle
Build/Reset/Calculate этим подэтапом не меняются.

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

Это кандидат ветки codex/timelearner-capacitance-owner, ещё не принятый develop.
В baseline Core8b41 owner state после bound edit пуст; исходный A1 смешал этот
факт с двумя ошибками fixture. Corrected baseline A2 завершён отдельно без
изменений Core:8/8 отрицательных cases, root evidence
`a054904f31160271a87ea327353890f052d540f1`.

Normal C2 на Pulse `f899ef024698afa7bc8e9a17f2307116368048da`, root
`3a1b7fe1f0a2a958d034736613f2793abe16c5ec` завершил8 native и19 independent
readers. Root evidence `a928fb92edd0880728a697da0fed6d7ede3e7d4a`: проверены
все259 members, compressed archive10 491 579 bytes. Эта адресная проверка
подтверждает C₀/scales и фактические L/Ns/Rs после Reset/growth/shrink/regrowth;
она не требует равенства полного графа после разрешённого освобождения owned
synapse banks. Реальные graph identities сохранены. Строгий full-graph
preview/live и export/import contract остаётся обязательным.

ASan owner8, transport43 (включая fresh-process dormant-image import), frozen
и failure/rollback regressions, полный temporal render и visual QA ещё pending.
До их завершения полной приёмки owner нет. Bounded C-trim search, native
XML/Copy persistence всей private metadata и вся physics/mode матрица не
подтверждены этим срезом. Подробный статус, handles и evidence находятся в
корневом Docs/StructTrain/TimeLearner/CapacitanceOwnership.ru.md.
