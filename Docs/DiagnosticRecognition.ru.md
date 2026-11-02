# Диагностическое распознавание остановленного обучения

Рабочий API, ещё не принятый по серверной матрице. Обычный
ConvertNativeSoloToRecognition по-прежнему требует completed solo. Новый
ConvertNativeStoppedToDiagnosticRecognition принимает только stopped package
с state_kind=diagnostic и certificate=null, через штатный проверенный импорт.
Измерение не продолжается и новый сертификат обучения не создаётся.

Результат имеет отдельные kind=timelearner_diagnostic_recognition_state и
state_kind=diagnostic_recognition. Сохраняется весь неизменяемый source_solo,
его статус/причина остановки, наблюдения и неполнота references. Применяется
тот же код подключения всех входов, per-input Rs bounds, readback, проверки
интегратора и storage archive, что у обычного conversion. Повторное преобразование
начинается с исходного stopped solo, не масштабирует предыдущий результат.
Historical Branch N1 требует полных положительных references; partial capture
не дополняется вымышленными значениями. Identity conversion references не требует.

Отдельные PrepareNativeDiagnosticRecognitionState и методы обоих learner
ImportNativeDiagnosticRecognitionState устанавливают frozen hold через общую
native transaction. Обычные import/threshold-calibration validators эту схему
отклоняют. Диагностический import не устанавливает training service/certificate.
QueueNativeRecognitionTrial остаётся штатным full-pattern исполнителем с
before/after hooks и native scheduler, не вложенным Calculate. Build/Reset/Init
и их bool-контракты не изменяются. Train target и transient stimulus различаются.

ModelArchive явно различает диагностическое распознавание и completed
recognition: stopped statuses/partial references/null certificate разрешены
только диагностическому виду. Conversion metadata требует unverified отдельного
recognition certificate и реального model identity. Exact claim не допускается.

В корневом integration fixture заявлены8 новых cases Classic/Branch × N0/N1 ×
partial/model-budget: native conversion/import, неизменный источник, idempotence,
запрет обычного import и threshold candidate, попытка подделки kind, реальные
full-pattern target/foil/reset/continuous/partial traces и frozen model hash.
Два ordinary completed controls используют явно именованный tolerance0.2,
не меняя исходный default0.02 или критерий основных отрицательных cases.
Публичный Console/CLI и отдельный диагностический анализ результатов — следующий
обязательный слой; этот API сам по себе их не реализует и не является завершением
этапов5/8/10/11. Научная успешность распознавания здесь не утверждается.
