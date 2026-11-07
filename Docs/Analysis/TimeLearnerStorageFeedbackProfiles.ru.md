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


## Полный physical graph import через derived images — scoped normal проверка

ImportFeedbackProfileGraphForEdit принимает immutable native snapshot,
готовое storage/environment, явные class substitutions, optional uniform gain
и штатные boundary ports. Source XML сначала проходит собственный native
Load/Init и полную identity-проверку. Затем штатный USerStorageXML заменяет
class attributes/role selectors; прежние FeedbackGain overlays удаляются
только там, где задан новый membrane image с проверенным коэффициентом.
Значение наследуется при TakeObject/Load, без правок живых мембран.

Индивидуальные параметры, структура и связи остаются в полном source XML.
Новый объект проходит native initialization/reset и строгую проверку всего
expected graph; разрешены только объявленные классы/gain. Source объект,
образы, контроллер и сертификат не редактируются. Это не controller checkpoint,
не обещание Train→Test для всех моделей и не scientific verdict.

Предусмотрены Classic/Branch2000/20000Гц, индивидуальные C/Rs, полный native
roundtrip, запрет unmapped gain/неверного image gain и противоречивого source
XML. Приёмка исправленной функции и fixtures приведена ниже; первая неудачная сборка сохранена отдельно.


Первая сборка root8302e8f76/Pulse1d2a703 завершилась тремя compile errors:
временный UEPtr<UNet> нельзя преобразовать в UEPtr<UContainer> через
неконстантную ссылку существующего Rdk. Native tests не запускались. В77adb66
использованы именованные указатели; Rdk/UEPtr не менялся. Failed primary
корня afb5c7d144ad92c701e294447180e802fe52741a сохранён;25 members и783
source hashes сверены, группа закрыта/lease released.

До runtime статически выявлено, что общий CheckSnapshot требует каждого
параметра в XML. Его обычный контракт не ослаблен. Только внутренний import
проверенных derived images допускает отсутствие FeedbackGain по конкретным
уже проверенным membrane paths; после native Load сравнивается весь expected
graph. Source/rehearsal и обычный ModelArchive остаются strict. Добавлены
negative fixtures для неполного source XML с правильной checksum. Это отдельная
предварительная правка, не причина трёх фактических ошибок первой сборки.
Приёмка исправленной версии приведена ниже; это описание исходного ограничения и первой сборки.


Исправленный SDK root8978f104a1cef31fe520df4d0a63cf9c56b06c1e /
Pulse32ea8ae8cc0f79dac6d0992fe87c117e41a8c809 прошёл M40 normal24/24 native
за129.462с и27/27 readers без skips. Восемь actual full-profile import records
покрывают Classic/Branch N4,2000/20000Гц, baseline и gain0; source threshold
задан0.0117, индивидуальные C1.3e-9/Rs93e6. Production graph auditor подтвердил
полный source→candidate delta и строгий обычный export/import roundtrip.
Ошибочный image gain, отсутствие derived membrane image, противоречивый и
неполный source XML отклонены. Обычная completeness-проверка не ослаблена.

Все225 members primary849936B и783 source hashes сверены с Git. Группа закрыта,
lease released; пять прежних runtime, включая failed-a1, не изменились.
Primary commit корня c4e37e764891429e186f84d33e6f60186879338a; приёмка в
Docs/StructTrain/TimeLearner/ProfiledGraphImportNormalVerification.json корня.
Это перенос физического графа fixture, не научный результат: candidate waveform
не рассчитывался, controller checkpoint и certificate не переносились.
Архивные stopped-model probes, новый Console protocol, ASan данного API,
второй сервер и полная физическая матрица остаются открытыми. Первая compile
failure сохраняется отдельно и не переписана под PASS.
