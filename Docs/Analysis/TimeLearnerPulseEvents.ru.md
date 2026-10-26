# Фактические события генератора и пустая история

NPulseGenerator (включая Transit в собственном generator-режиме) и
NPulseGeneratorMulti публикуют OutputPulseTimes как историю фактических emission
timestamps в секундах. Размер — 1×N; отсутствие событий — 1×0. Настоящее событие
в момент Environment time=0 представляется одной записью 0, а не пустой матрицей.

TL-019: прежние Default/Reset и Frequency=0 оставляли [0] либо несколько нулей
от прежнего размера. Чтение их как событий создавало несуществующие импульсы.
Теперь эти операции очищают историю; Build согласует её с уже очищенным
AvgFrequencyCounter. Пространственная/временная динамика генератора не меняется.
Альтернативный NPulseSimpleLTZone::ACalculate2 копирует матрицу целиком вместо
безусловного чтения первого элемента. Это не изменение всех LTZone histories:
их собственные выходы необходимо проверять отдельно.

Клиент обязан проверять размер, а не обращаться к (0,0) без условия. У Multi
равные timestamps могут означать разные одновременные emissions; удалять такие
дубликаты без знания каналов нельзя. История ограничена AvgInterval и очищается
при Reset/выключении, поэтому не заменяет persistent event log.

NDatasetBase перезапускает одиночный generator перед каждым FireOneShot;
PulseCounter поэтому не является общим монотонным счётчиком предъявлений.
Для первого uniform-rate TimeLearner adapter события забираются после каждого
native tick, с отдельным cursor для каждой пробы. UseTransitSignal, multirate и
Multi требуют собственных проверенных правил чтения. В режиме Transit собственная
история генератора не доказывает передачу внешнего сигнала.

Native controls находятся в корневом Test_PulseLib_NativeProbe.cpp:
NativeDatasetEvents проверяет четыре близких предъявления без synthetic t=0;
NativeGeneratorPulseTimes различает пустую историю и реальное событие t=0
для Single/Multi при 2000/20000 Гц. NativeProbeRestart проверяет повторные
пробы Classic/Branch на ненулевом времени без нового Build или learner Reset.
Сведения о серверной проверке публикуются в корневом
Docs/StructTrain/TimeLearner/ProbeMeasurement.ru.md.
