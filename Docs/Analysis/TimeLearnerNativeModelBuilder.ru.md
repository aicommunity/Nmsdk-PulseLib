# Явное создание и приём native-модели TimeLearner

Оба learner предоставляют `ConstructNativeModel(NativeModelSpec)` для явного
холодного построения и `AdoptNativeModel(NativeModelSpec)` для read-only приёма
существующего графа. Это C++ API; configured `ControllerVersion=2` и перенос
полного обученного состояния остаются отдельной незавершённой работой.

Общие физические параметры берутся из immutable storage images. Builder создаёт
частный Neuron/Dataset, устанавливает частоту до финального построения, задаёт
морфологию, Ns/индивидуальные Rs и реальные стимульные/тормозные связи. Проверяются
actual graph, uniform frequency, готовность, native caches, integration assessment
и semantic identity. Только затем выполняется замена owned children. Внешний
listener запрещает cold replacement до удаления старого графа. Отсутствующая Rs
означает реальное значение storage image; parent fallback не используется.

Дополнительных Default и вложенных Calculate нет. Дочерние компоненты рассчитывает
штатный scheduler. Для neuron mode 0 структура должна присутствовать в самом
образе; modes 1/2 используют штатное scalar/vector построение. Learner
StructureBuildMode 0/1 задаёт отдельную политику сохранения выделенного кабеля.

После передачи native-владения forced Build проверяет текущий граф, не пересоздаёт
Neuron/Dataset и не стирает learned Rs/heterogeneous C. Reset сохраняет модель.
Состояние пробы и typed diagnostics отделены от public bool Build/Reset/Calculate.
После активного lifecycle-вмешательства управление инвалидируется. Равнозначное
присваивание UProperty может не вызвать setter; проверка forced Build должна
действительно переводить owner в unready через штатный setter.

Adoption проверяет фактические параметры, target, классы и связи без overlays.
Legacy Branch muted baseline разрешается явно и остаётся в model hash; частичный
bank или чужой source отвергаются. Новое cold-построение подключает declared
banks. Тормозные offsets и scaling описываются в NativeModelSpec, а не скрытыми
legacy flags. При retarget меняется Dataset и инвалидируется прежняя цель.

Root `c1f90650bf7e5e01b848fbb5a278950ae927e641`, PulseLib
`37e8cc47a50712daf98186dae933836b7218a974`: на каждом сервере 29/29 новых и
68/68 регрессионных проверок, 18/18 Python evidence tests без skips. Проверены
Classic/Branch, 2000/20000 Гц, neuron 0/1/2, learner 0/1, N=6, Ns=3,
heterogeneous C, individual Rs, forced Build/Reset/UnInit/Init, listeners и
реальные Branch solo/inhibition routing до/после growth. Это адресные проверки,
не полная декартова матрица и не доказательство научной сходимости.

M40 ASan: 21/21 addressed cases, detect_leaks=0. Проверены 17 builder cases
при 2000 Гц и четыре cold native searches. Все A1/A2/A3 попытки, включая исходные
failures, сохранены через Git в корневых Reports/StructTrain/TimeLearner/
s06-native-builder-elder1 и s06-native-builder-m40. Полная CTest matrix после
этого снимка не заявляется; отсутствие всех утечек не доказано.

Process-local `NativeModelDeclaration` не является переносимым checkpoint.
Copy/XML всего learner и trained-state codec пока не обещаются. Полный журнал
A1/A2/A3, приёмка и артефакты находятся в корневом
`Docs/StructTrain/TimeLearner/NativeModelBuilder.ru.md`.
