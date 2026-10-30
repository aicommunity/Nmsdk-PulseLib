# Native threshold candidates и измеренное предложение порога

`Core/TimeLearner/NativeThresholdCandidate.h` создаёт immutable candidate из
полного `timelearner_recognition_state`. Единственное разрешённое физическое
изменение — реальный `LTZone.Threshold`. Образы storage, topology, bindings и
solo proof сохраняются. Вложенные candidates, nonfinite/неположительный порог
и подменённые records отклоняются. Import заново выводит ожидаемую модель
через native archive path; дополнительный Calculate не вызывается.

`NativeRecognitionTrialRequest.recordThresholdPotential=true` включает schema 2:
каждый отсчёт содержит фактический `LTZone.Potential` после штатного scheduler.
Источник описан path/class/sample_phase/units. SI units не подтверждены.
Существующая schema 1 при выключенном флаге не меняется. Внешний source у
Potential и nonfinite значения отклоняются. Soma.SumPotential и LTZone.Potential
не подменяют друг друга.

`ProposeNativeThreshold` принимает явно ожидаемый сбалансированный набор
завершённых nonspiking проб, исходный training pattern hash, response window
и допустимые bounds. Нижняя граница учитывает весь foil window и запрещённые
target-outside ответы, верхняя — минимальный target peak внутри response window.
Результат содержит первичные observation digests, extrema и причину Proposed,
NoMeasuredGap либо NoRepresentableThreshold. Это предложение, а не калибровка:
обязательны реальные spiking search и отдельно rebuilt confirmation пробы.

Изменение порога не переносит solo certificate Exact на recognition model.
Candidate metadata помечает `calibrated=false`; source graph не меняется.
Повторный import восстанавливает тот же порог, не выполняя повторного conversion.
Используется штатная idle boundary: перед заменой модели необходимо завершить
пробу и прочитать её результат. Busy guard нельзя обходить через Reset.

На 31.10 выполнены адресные 2000-Гц пробы Classic/Branch и независимые первичные
проверки порогов; полная 2/20-кГц normal/ASan матрица ещё проверяется. Controller
всех PostTune modes, calibrated-state export и independent frozen evaluation
не объявлены готовыми. Новый dispersed two-pattern контроль не является
историческим августовским результатом. Подробные версии, failed attempts,
CLI select/confirm и compact artifacts: корневой документ
`Docs/StructTrain/TimeLearner/NativeThresholdCalibration.ru.md`.

Рабочее усиление следующего подэтапа: общий
`ValidateNativeRecognitionObservation` сверяет записанную request с объявленной,
число и точную native grid отсчётов, закрытие окна, finite soma/stimulus/LT values,
покадровые stimulus/spike histories и итоговые event arrays. Данные continuous
stream до начала окна не становятся событиями текущей пробы. Предложение порога
теперь вызывает эту проверку до использования экстремумов. Изменение требует
отдельного серверного gate; прежняя приёмка Core 7f17735 его не подтверждает.

`AssessNativeThresholdResponses` оценивает закрытую заранее объявленную матрицу
одной модели/частоты на одном clock. Порядок recorded rows не влияет на ответ;
порядок предъявлений сверяется с expected requests и реальными временами.
Для target требуется spike внутри response window, а при соответствующей
политике запрещены ранние/поздние spikes. Для foil проверяется всё окно.
Сохраняются TP/FP/FN/TN, каждый spike, нарушения target и отдельные reset policies.
Silent target даёт FN, а не успешное распознавание. Schema-1 и schema-2 данные
проверяются одним validator. Измеренная готовность не означает calibrated commit
или training certificate. Независимый Python scorer должен подтвердить native
assessment перед приёмкой нового контура. API ещё не собран на servers.
