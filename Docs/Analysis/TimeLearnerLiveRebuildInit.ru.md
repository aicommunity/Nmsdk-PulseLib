# Инициализация нового нейрона при live Build Classic

NNeuronTimeLearner::BuildStructure заменяет компонент Neuron и создаёт его
параметры, морфологию и связи. При Build уже инициализированного learner новый
корень Neuron должен пройти собственный native Init до следующего расчёта.
Build/Reset сами по себе не заменяют Init нового поддерева.

До исправления после cold Reset и первого scheduler Build наблюдались
Neuron Ready=true, IsInit=false, правильные связи Dataset → tip Input и
инициализированные дочерние синапсы. UContainer::Calculate пропускал такой
корень; Classic сохранял нулевые soma-трассы и references. Branch уже завершал
BuildStructure инициализацией Dataset/Neuron и воспроизводил реальные ответы.

Classic теперь завершает структурную операцию тем же native путём: обновляет
active cache и вызывает Init только для ещё не инициализированных Dataset/Neuron.
Вложенного Calculate, повторного Build ради «пробуждения», принудительного
Default, смены частоты/параметров и сброса всего learner не добавлено.
Возвращаемые bool lifecycle не используются для классификации обучения.

Адресные проверки в корне:
Tests/Integration/Libraries/Test_PulseLib_ControllerCompatibility.cpp.
Есть короткий cold-reset/response regression и ограниченные трёхсекундные
native controls: Classic/Branch, N0/N1 и model/controller clocks 2000/20000.
Это implementation controls, не проверка сходимости или selectivity.
Полные исходные/fixed артефакты и ограничения описаны в корневом
Docs/StructTrain/TimeLearner/LiveRebuildInit.ru.md.

Отдельно остаётся TL-016: частота model/controller 20000 не гарантирует частоту
всех новых legacy-образов. Проверки этой страницы не объявляют такой граф единым
по частоте. Новый проверенный контур требует явных storage-профилей/частот и
effective readback до Train и после growth. Нельзя исправлять это записью
параметров на каждом тике или изменением общего работающего prototype.

Августовские визуальные результаты сохранённых моделей остаются результатами
их протокола. Этот live-rebuild reproducer не отменяет 4/8–6/8 и не является
повторением августовского полного обучения или проверки порядка/ITI.
