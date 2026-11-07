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
