# FeedbackGain в immutable storage-профилях

Статус: адресная normal-проверка на M40 пройдена; ASan и второй сервер открыты.

MembraneProfile.feedbackGain — optional<double>. Отсутствие значения сохраняет
FeedbackGain выбранного базового образа, включая значения, отличающиеся от2.
Явное значение должно быть конечным и неотрицательным;0 допустим. Оно применяется
однократно к создаваемому membrane image до регистрации. Параметр не добавляется
в tick loop learner и не меняет общий prototype или работающий экземпляр.

Body и необязательная LT membrane имеют отдельные рецепты. Создание LT membrane
не становится обязательным. Новые сегменты получают FeedbackGain из образа;
индивидуальные обученные Rs/C принадлежат model state и не заменяются этим полем.
Предусмотрены проверки custom base3.5, explicit0, роста/Build/Reset/XML,
коллизий имён и NaN/Inf/negative до публикации зависимостей.

Динамическая проверка использует native single-pulse executor и scheduler,
две топологии и2000/20000Гц. Для инженерного возбуждения LT явно задан один
и тот же порог1e-6. Сравниваются inherited gain и explicit2 (ожидается точное
равенство волн и LT timestamps), затем explicit0 (ненулевые LT события при
нулевом membrane Feedback и изменённом отклике). Все параметры считываются
после штатного построения; одного возвратаtrue недостаточно.

Эти fixtures не являются причинным вмешательством в опубликованные stock
модели с порогом0.0117 и не показывают качество классификации. Для такого
вмешательства сначала требуется восстановить весь эффективный physical profile
и доказать эквивалентность baseline, затем изменить только объявленный фактор.
Рост, подключение, source/reset policy, target, морфология и индивидуальные
параметры должны сохраняться. Полная heterogeneous/lifecycle матрица ещё открыта.


Проверенный SDK: rootb54330482a2e4f3a67763301e40dc067374d1974,
Pulsed09613cc8b7b2aebfe27d0f851fd9a2a481ce201. Native18/18 за95.934с,
readers13/13 без skips. Все161 members primary365876B и781 hashes исходников
сверены с Git, процессы закрыты и lease released. Primary commit корня
080d21dcff3940df3ce72bf4a21d1c0ceb003e26; подробная приёмка в корневом
Docs/StructTrain/TimeLearner/StorageFeedbackProfilesNormalVerification.json.
Проверка инженерного порога1e-6 не заменяет paired stock intervention0.0117.


## Наследование полной физики существующих образов — scoped normal проверка

RegisterFeedbackStorageProfile использует FeedbackStorageProfileSpec:
name/frequencyHz/neuronBaseClass и optional bodyFeedbackGain/ltFeedbackGain.
Создаётся цепочка существующий membrane image → новый membrane image с gain →
новый neuron image. Канальные/синаптические C/R/кинетика и role class names
не заменяются значениями ChannelProfile/SynapseProfile. UseElementDefaults
и остальные родительские правила также сохраняются. При отсутствии gain
соответствующий образ используется без переопределения. При отсутствии LT
просьба задать её gain отклоняется до публикации образов; LT не добавляется.
Explicit StructureBuildMode0 требует отдельного native model import.

Это конструктор образов, а не операция над живым обученным нейроном. Полный
learned state и индивидуальные параметры по-прежнему переносятся штатным
import с проверкой actual graph. Научный baseline должен подтвердить все
поля effective graph с явными class renames и waveform, а не только gain.
Ранее принятые18/13 checks проверяли другой API RegisterStorageProfile;
новая реализация проверена отдельно, как указано ниже.


Inherited-image SDK rootabf13d2b2955354cd5f12322c2ea3e3b5a84799c /
Pulse73f03657e45ce8b29a943093b48c6f1c3400f295 прошёл M40 normal22/22 native
за110.494с и15/15 readers без skips. Два reader tests применили production
полный graph delta auditor к12 actual inherited-profile control identities.
В native controls исходный образ и explicit2 имеют одинаковые sample arrays
и LT timestamps; explicit0 имеет нулевой Feedback при существующих LT events
и изменённый отклик. Остальные effective поля графа совпадают с явными class
renames. Нестандартные C1.7e-9 и DissociationTC0.007 сохраняются при growth;
коллизия, invalid gain и попытка добавить отсутствующую LT отклоняются.

Все200 primary members/588892B и783 source hashes сверены с Git, процессы
закрыты, lease released; три прежних runtime не изменились. Primary root
commit b157fd3f9b6dd6ec3f82e48f68c5ff0a57e41a4f; приёмка в корневом
Docs/StructTrain/TimeLearner/InheritedFeedbackProfileNormalVerification.json.
Это engineering threshold1e-6 и fixture-морфология, не перенос опубликованных
stock stopped-state0.0117. ASan данного API, второй сервер, XML roundtrip
новых образов и полный learned-state intervention остаются открытыми.
