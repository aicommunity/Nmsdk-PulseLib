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
