# Исходный запрос Train и физическая граница finalization

NativeTrainingRequest() обоих TimeLearner возвращает value-copy исходного запроса
успешно начатой native-сессии: публичные параметры/их источники, run/session,
initial model revision, epoch, protocol/pattern hash и время начала. Снимок
формируется по controls, прочитанным перед Start, и содержит фактические session
поля после Start. SHA вычисляет production canonical serializer.

Позднее изменение controls не переписывает снимок. Это provenance исходного
запроса, не разрешение игнорировать текущий внешний stop или новые настройки.
Будущий dispatcher должен сравнить original/current controls перед переходом.
Сейчас изменение controls только отражается отдельно; автоматическое выполнение
финальных фаз ещё не подключено, optional guards сохранены.

Mode 0 может штатно снять локальный Need при завершении Train; начальное true
остаётся в intent. При обычном подключении Rdk SetDataDirect не пишет в source
и не снимает внешний true. Mode 1 сохраняет внешнее управление. Снимок сохраняет
исходный true после external stop, не приписывая такому останову сходимость.

Explicit Reset, Default, retarget, новый Start и успешная замена графа очищают
предыдущий intent. Неизменённый target и отклонённый import сохраняют его.
Внутренний Reset под NativeLifecycleDepth не стирает запрос. Intent не входит
в существующий versioned model export/import: восстановленный исторический
Solo не получает выдуманный train-time запрос. Возобновление intent через Git
или model checkpoint пока не реализовано; отдельный физический boundary остаётся
обязательным. Copy не является checkpoint.

Нативные сценарии добавлены для Classic/Branch × CalculateMode 0/1 × local/
connected request: старт, поздняя смена fraction, честный BudgetExhausted,
Mode-0 completion, отклонённый import, Reset без изменения learned parameters,
новая сессия, внешний stop, no-op target и retarget. Серверная приёмка ожидается.
Это control-only пробы с малым declared budget, не независимое успешное Train и
не all-pattern recognition; завершённые waveform-ответы не выдумываются.
