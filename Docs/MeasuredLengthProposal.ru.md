# Предложение длины по измеренной временной ошибке

Состояние15.11: базовая measured-length policy адресно принята; новое широкое
покрытие rejected-window прошло142 normal/native checks и сравнительный cold Train.
Это изменение
порядка кандидатов общего V2 BoundedTrainingSession; математическая цель,
CapturedPerInput, допуски и штатный Rdk scheduler не меняются.

После завершения полного кандидата (включая обязательные повторы N0) контроллер
сохраняет signed timing interval и полный вектор параметров. Сравниваются только
измерения активного входа с теми же Ns/Rs и всеми прочими параметрами. При смене
активного входа/начала sweep эта история очищается. Каждый исходный sample
остаётся связан со своими trial/model revision; прежний пик не подтверждает
новый граф. Shared Branch изменения остальных attachments не смешиваются
с неизменной геометрией.

Минимум три различные измеренные длины определяют наблюдаемый тренд. Повторы
с несовместимыми интервалами, немонотонность и неопределённый знак не создают
прогноз. При измеренном bracket между соседними длинами предлагается внутреннее
целое среднее; без bracket односторонний тренд предлагает ограниченную целую
оценку нуля по секущей. Предсказание не доказывает глобальную монотонность,
существование пика, физическую достижимость или синхронность.

```text
consume complete valid candidate, after all required repetitions
retain timing interval and full parameters with original trial/revision
select same-input, same-Ns/Rs, otherwise-identical geometry measurements
require three distinct measured lengths and consistent monotone trend
propose legal unmeasured integer (bracket midpoint or bounded secant estimate)
prioritize next length without increasing candidate/probe/edit/time budgets
N1: finish current geometry, then assess proposed native physical domain
apply native edit, measure fresh anchor/input, then fresh all-input final gate
```

Уже использованная длина не вставляется повторно перед текущей позицией;
оставшийся конечный перебор сохраняется. N0 full-vector dedup и обязательные
повторы остаются действующими. N1 предложение не обходит native domain action,
интегратор, bounds или полный effective readback. Восстановление best не
порождает новый поиск. Missing/ambiguous/incomplete peaks сюда не передаются:
общий validator отклоняет их до proposal policy.

Приёмка требует исходного отрицательного контроля, production helper/controller
checks normal/ASan, реальных native burst/edit/domain regressions, cold model
experiments с полными наблюдениями и читаемыми временными отчётами. До этого
feature не является develop. Начальные EstDelayPerSeg/gamma остаются отдельным
вопросом явной V2 конфигурации; этот измеренный тренд не делает legacy property
действующим фактором V2 и не закрывает полный этап7.

## Адресная приёмка

Численный Core a728d97110b706bd7869039557de957ea299c6ea проверен на обоих
workers. Normal после исправления Bin worktree:67/67 в одном XML, без skips;
root proof1701991807a10db5609a3656c468bb18bb1f30c3. ASan:39 kernel cases в XML,
четыре NativeBoundedTraining cases в исходном stdout до7200s timeout,
затем24/24 remaining cases в отдельном XML на том же runtime;
proofs0608878fa3570698fa53571c66fdddcd7e59d8c2 и
d6110479ffc5deb5f496eb6e36ae10e6ce4e857a. Наборы67 имён совпадают;
actual mappings сохранены, ASan mappings содержат libasan. Первый timeout
и последующий missing-XML launcher exception не скрыты.

Реальный cold corpus Classic/Branch×N0/N1×baseline/current при2000Hz сохраняет
одинаковые исходные параметры, CapturedPerInput references и tolerances.
Новая последовательность измеряет2/3/4/9 вместо nearest2/3/4/5; все8 runs
на каждом worker заканчиваются SearchUnresolved с отрицательной свежей
all-input verification. Старый normal corpus имеет обнаруженное впоследствии
ограничение Bin source provenance; repair не исправляет его задним числом.
ASan primary и нормальный повтор native67 являются отдельными доказательствами.
Прежние два положительных N1 jobs на каждом worker сохраняют Exact и84/84
byte-identical CSV относительно принятой версии. Это не классификация.

Полные временные отчёты всех проб:240 trial-soma pairs,4146 PNG в M40 cache.
В Git остаются688 JSON metadata records и16 directly reviewed originals1920×960.
Independent ASan rerender восьми выбранных проб дал16/16 byte-identical PNG
(root proofbd3ead63b019f8ad2cdbf4223143470f9e25930f). Все160 crossworker CSV
побайтно равны, observation JSON семантически равны160/160; различие JSON
форматирования не называется byte equality. По рисункам не объявляется Exact.
Полные этапы6/7/11, legacy/V2 parity и полная физическая матрица остаются открытыми.


## Branch N0: покрытие после typed rejected-window (15.11)

Production a82a7cd08a4334c5924839b7136ae9df5884da53 вводит только порядок
предложений в исходном Ns/Rs-семействе. Штатный native edit/readback, свежие
anchor/input measurements и бюджеты не меняются. Ambiguous/missing никогда
не создают scalar residual или certificate; Classic/N1/legacy/default Retry
не переключаются на этот поиск.

Новый SDK normal142/142 (113 algorithm/codec и29 отдельными native gates)
завершён с полным закрытием процессов. Четыре helper и два широких native
регрессионных сценария входят в этот actual inventory. Восемь cold-reference
CSV до/после scoped test logger-mask change совпали побайтово.

Холодный Branch N4/N0 при исходных CapturedPerInput/tolerances и том же
local12/global128probes реально проверил input1 L28,100,64,46,82 вместо
28..32; все пять наблюдений ambiguous. Первые13 scalar measurements равны
контролю, измерены только input2. Итог invalid_observation, certificate=null,
terminal L=[82,82,27,1] не имеет consumed scalar measurement.
Production CLI/DATA/physical1/1 завершены и закрыты. Это отрицательный
научный результат ограниченного сравнения, не обоснование общей
PhysicalInfeasible и не классификация. ASan-приёмка именно нового покрытия
и полная режимная/физическая матрица пока не заявляются.

Полный primary во внешнем архиве M40: sha256
0c4f07e6c10a7586768a759ec95d73f7f356aa5daf90b17717a2ae3c72efb55a,
17074560B. Корневой научный отчёт:
Docs/StructTrain/TimeLearner/N0RecoveryCoverageScientificReport-20261115.ru.md.
Исторические адресные normal/ASan доказательства выше сохраняют свой scope.
