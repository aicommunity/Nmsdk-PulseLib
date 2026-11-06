# Schema4: явная C-search policy в configuration и model boundary

Статус: код подготовлен; native/reader проверки ещё не выполнены. Это не
приёмка C-trim, новой версии2 или полной матрицы плана. Нужен также probe
ownership fix54516bcc: automatic Start не должен захватывать C0 при Queued probe.

Schema4 cold configuration содержит обязательные capacitance_history и
capacitance_search. Последний объект закрыт: enumeration=
round_robin_absolute_C0_v1, minimum_scale, maximum_scale, subdivision_depth,
maximum_contexts, maximum_membranes, maximum_cursor_elements. Неизвестные поля,
boolean вместо чисел/счётчиков, нечисла, дубликаты, неизвестный enum и потеря
обязательных полей отвергаются. Политика валидируется отдельно и относительно
геометрии, общего candidate budget и declared owned C0 bounds до Train.
Новые C0 не придумываются parser-ом: native Start сверяет реального владельца.

Один EncodeCapacitanceSearchPolicy используется для cold configuration и
NativeTrainingState summary. Summary4 добавляет тот же объект к configuration.
Если C-search отсутствует, прежние форматы1/2/3 сохраняются с прежними полями.
Schema4 всегда хранит n0_candidate_repeats, включая1; для N1 допустимо только1.

Completed/stopped model4 сохраняет прежний полный model export, реальный C-state
и полный необходимый C0 witness history (capture/current, reference/final и
остановленные observed trials). Дополнительно политика хранится в configuration
и training metadata neuron archive. Import требует их точного совпадения,
совместимости с schema4 и подтверждённым native graph. Понижение schema не
должно оставлять скрытую policy. Semantic model fingerprint и все фактические
параметры/связи проверяются существующим native import путём.

Это boundary restart. Динамика, середина пробы, курсоры незавершённого поиска
и уже израсходованные бюджеты не объявляются точным checkpoint. Reset очищает
временное управление; trained model/C0 сохраняются. Экспорт остановленного
terminal boundary берётся до общего Reset, а последующие frozen probes
проверяют неизменность физики и сохранность policy/C0. История не выдается
за продолженный поиск с того же шага.

Root fixtures готовят4 codec cases Classic/Branch N0/N1,4 completed и4 stopped
same/fresh-process transfers, а также4 native activation controls с явным
нулевым sync tolerance и фиксированными L/Ns/Rs. Последние обязаны реально
изменить C и измерить native graph; ожидаемый отрицательный training outcome
не является scientific classification result. Все временные CSV сохраняются.
Тесты сохранения используют широкий явно заданный sync tolerance0.2 и не
доказывают математическую необходимость C-trim для достижения цели.

Остаются server normal/ASan, independent primary readers, визуальные отчёты,
согласованная Git closure, measured-C comparison scope/refinement и вся
исходная mode/physics/dimension matrix.
