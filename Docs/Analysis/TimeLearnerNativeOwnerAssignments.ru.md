# Native model ownership: connected parameter assignments

Рабочая ветка stage04/native-owner-config-bindings; исправление пока не собрано
на workers. Оно касается native cold construction/adoption и learned-state
import, а не общего запрета связанных свойств Rdk.

Baseline root `2df1db4996d4ff25069d3e83f24864d880188095`, PulseLib 8683bbff
(Core c7266dcb): 8/8 native controls на M40 ASan дали ожидаемые test failures.
Construct/Import × Classic/Branch × scalar/vector принимали новый граф при
конфликте owner-параметров с внешними источниками. SetPointer sources не изменились:
скаляр остался 9 при желаемом maximum=8, вектор собирался из двух scalar outputs.
API возвращал Constructed/Adopted, хотя зеркало owner не могло применить значения.

В Rdk SetDataDirect для IsConnected пропускает запись; UsesExternalDataSource
имеет другую семантику и может переслать запись источнику. Их нельзя смешивать.
Первоначальный vector-to-vector SetPointer в test fixture был неверным:
collection input ожидает scalar sources. Авария того fixture сохранена отдельно
и не объявляется дефектом алгоритма. Корректный fixture проверяет настоящий
vector collector через два scalar outputs.

Новая общая проверка использует единственный список десяти ModelSpec/learned-state
mirror fields. Связанный/заимствованный параметр разрешён, если прочитанное значение
уже совпадает с желаемым; запись в такой источник пропускается. Конфликт даёт
native_model_owner_assignment_is_external:<property> до замены живого графа.
Обычные локальные поля обновляются без setter только при изменении значения.

Cold builder проверяет фактические значения private preview, включая полученные
из storage Rs, до DelComponent. Новый overload сохраняет старый API/symbol.
Adoption проверяется до изменения controller state. Import проверяется после
подготовки полного пакета, но до CommitNativeLearnerChildren. Mirror повторяет
проверку перед записью. Позднее изменение источника из callback не превращается
в успешный импорт; modelMayHaveChanged сохраняет реальное значение.

Подготовлены negative и matching read-only controls для обеих топологий и двух
операций. Режим CalculateMode/внешнее управление IsNeedToTrain и остальные
controller/threshold flags этим подэтапом не переопределяются: полный аудит этих
записей и production recognition/PostTune wiring остаются открытыми.
Build/Reset/Calculate bool и scheduler не изменяются; nested Calculate нет.
