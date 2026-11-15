# N1: предложения L/Rs после отвергнутого окна

Это поведение общего V2 BoundedTrainingSession для явно выбранной AdvanceRejectedCandidateWindow. Оно применяется только к аутентифицированному RejectedCompositeMeasurement текущей пробы. Строка ошибки, старый proof, другой model revision, final verification и внешняя остановка не разрешают такой переход. Legacy learner и обычный порядок по valid observations сохраняются.

После отказа списывается прежний общий invalid budget; максимум4 означает четыре восстановления и остановку на пятом отказе. Старый anchor и незавершённые повторы больше не являются текущим измерением. Никакого amplitude/timing residual из отвергнутой вершины не создаётся.

Для первого восстановления данного входа/C-контекста сначала предлагается тот же L, если он допускает независимое изменение Rs; иначе выбирается неиспытанный legal L. Для предложения запрашивается новая фактическая область Rs. В Branch активная точка должна быть дальше downstream; upstream bank отделяется дополнительным сегментом, чтобы изменение Rs не меняло сопротивления уже посещённых входов. Полный вектор проходит PrepareCandidate/Legal и штатный native preview. Последующие восстановления предлагают неиспытанную длину в наибольшем пробеле ограниченного допустимого диапазона. Монотонность отклика не предполагается.

Для предложенного полного графа адаптер заново определяет ResistanceDomainAssessment. Пересекаются индивидуальные declared bounds и фактический интервал данного входа; общего пересечения разных входов не требуется. После отказа первым предлагается свежий нижний Rs endpoint, затем остаются штатная сетка и разрешённое уточнение по валидным измерениям. Endpoint является предложением, а не доказательством улучшения. Уже отвергнутый точный полный вектор повторно не выдаётся этим recovery.

Если первый same-length domain недоступен, вне box либо не содержит новых полных кандидатов (в частности, уже испытанный singleton), допускается один domain-only переход к дальней длине. Сам singleton с другой геометрией может быть новым допустимым кандидатом. Бюджеты проб/edits/candidates/invalid не пополняются. Вся история recovery принадлежит своему C-контексту.

    verify sealed rejection, live control and current revision
    charge existing invalid allowance; retire anchor
    propose same-length independent bank once, otherwise unvisited legal L
    request fresh native Rs domain for the full proposed geometry/C
    try an unissued endpoint/mesh candidate within actual individual bounds
    apply through native adapter between probes; read effective parameters
    measure fresh anchor, then assigned input in the native burst
    accept only validated complete measurements; final all-input gate is unchanged

Контроллер только возвращает действия; scheduler рассчитывает дочерние компоненты. Общая физика остаётся в storage-образах, индивидуальный Rs применяется штатным structural editor. Build/Calculate/Reset не становятся каналом ошибок и возвращают true. CapturedPerInput, anchor N−1, допуски, reset policy и математическая задача не меняются. Любой edit инвалидирует текущие измерения; сертификат нельзя переносить между ревизиями.

Адресная приёмка16.11: baseline Pulse5052cfe1683ca38920b514aedfc3c37f4bd41aef даёт пять ожидаемых native assertion FAIL; candidate42e35b63b6c25f59629f0436b9476a35d20cb2c4 проходит те же пять cases и весь SDK147/147 на M40. Реальные storage/Environment fixtures проверяют Classic/Branch N2 и Branch N4, свежий domain/readback и новую опорную пробу. На Elder один и тот же полный численный corpus109/109 проходит до/после; макро-определённые budget cases учитываются production collector. Все группы закрыты.

Эта приёмка не доказывает научную сходимость и не закрывает матрицу C-trim/physics/20000Гц. Same-condition cold Stock N1 после изменения запущен отдельно; его результат ещё ожидается. Сравнение valid dense-Rs решений описано в [N1GeometryFairness](BoundedTrainingN1GeometryFairness.ru.md). Полные исходные отказы и результаты лежат во внешнем архиве M40; root содержит компактные ссылки: [постановка и границы](../../../../Docs/StructTrain/TimeLearner/N1RecoveryAndFastAmplitude-20261115.ru.md).
