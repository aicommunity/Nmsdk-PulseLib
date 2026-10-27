# ModelRevisionObserver

Process-local read-only ускоритель IdentifyModel сохраняет его семантический
hash. Каждый Read проверяет actual typed scalar/vector/matrix parameters,
bindings/slots, named membership/order, property pointers/types/masks, aliases,
native caches, frequency и lifecycle. Изменение заставляет пересчитать полный
identity; invalid/nonfinite/unsupported не возвращает прежний valid hash.

UEPtr не владеет объектом. Lifetime и membership проверяются до чтения сохранённых
typed pointers. Удаление/замена свойства и declared source проверяются отдельно.
Общие mutable prototypes не изменяются. Model identity не является checkpoint
динамики или регистрацией definitions всех storage images.

Native training adapter использует observer на каждом тике; optional
readFullModelRevision принудительно вызывает полный capture на границах
подготовки/reset/routing/закрытия пробы. Старые providers без callback сохраняют
полный путь. Build/Reset/Calculate и их возвращаемые bool не изменены;
вложенных Calculate и записи исправляющих параметров нет.

Снимок root 1f1e0352bce24bc538d888e20f4e20ebc4906339: 85/85 addressed C++ на
каждом сервере, включая native Classic/Branch A/B 2000/20000 Гц с одинаковыми
полными stimulus/soma arrays и model hashes. Test snapshot
87e444b67ef471d79cb8277630472ee1a090226c: 12/12 replacement/lifetime cases
и 20/20 старых provider regressions. Root auditor: 18/18 Python без skips,
56 actual service traces и четыре отрицательных сертификата. M40 ASan: 59/59,
detect_leaks=0, включая native A/B на 2000 Гц и все 13 service controls.
Native A/B на 20000 Гц покрыты обычной A1; отсутствие утечек не заявляется.

Read-only benchmark на 169 nodes даёт ускорение самого чтения 83.52/100.01,
native двухвходовые пробы — 6.36–8.35. Это адресная оценка overhead, не научная
сходимость и не универсальный коэффициент скорости. Configured V2, trained-state
import/export и полная physics/mode матрица ещё не завершены.

Подробный контракт, результаты и Git artifacts:
корневой Docs/StructTrain/TimeLearner/ModelRevisionObserver.ru.md.
