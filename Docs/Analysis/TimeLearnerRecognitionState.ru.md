# Native recognition state

`NativeRecognitionState.h` задаёт преобразование immutable completed solo
package в отдельный полный recognition package. Identity и исторический
Branch N1 factor rule имеют явный spec; individual Rs/clipping и активация
owned input tips фиксируются в conversion record. Physics images и исходный
learner не изменяются. Common native scheduler не вызывается преобразованием.

Recognition metadata schema 1 отличает исходный solo training certificate от
ещё не проверенного recognition graph. Новый importer выводит ожидаемый state
из source solo, затем проверяет и загружает сохранённый native archive. Он
возвращает frozen neuron/dataset без training service, не применяя factor к
уже преобразованным Rs при повторном load.

Контракт, текущая приёмка и оставшиеся conversion/PostTune/calibration задачи:
корневой `Docs/StructTrain/TimeLearner/NativeRecognitionState.ru.md`.

Адресная проверка source 74ab08f: Elder normal и M40 ASan — по 11 native /
23 Python tests; 8 recognition packages / 108 source trials / 3528 полных
paired samples. Root ca2f095b: по 8 настоящих fresh-process imports и 29 Python
tests, все 3528 samples byte-exact к Git-пакету Elder, включая Qt5→Qt6 replay.
Fresh root получает штатные Init/Reset всего дерева до первого Calculate.
Ошибки прежних fixtures сохранены отдельно. Эти данные подтверждают transport,
не научную разделимость, реальную калибровку или завершённый PostTune.
