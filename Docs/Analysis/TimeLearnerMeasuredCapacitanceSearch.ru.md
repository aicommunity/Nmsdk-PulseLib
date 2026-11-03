# Измеряемые предложения C-кандидатов

Core/TimeLearner/MeasuredCapacitanceSearch содержит production kernel конечной
сетки абсолютных scales C/C₀ и предложения по signed timing intervals.
Refinement требует минимум три различных реально измеренных scale, одинаковый
comparisonScope остальных физических условий и согласованный измеренный trend.
Повторы объединяются полным интервалом; NaN, mixed scopes, nonmonotonicity и
неразрешённая неоднозначность не создают вымышленную сходимость. Прогноз — только
кандидат для последующей native проверки, не свидетельство пика или интегратора.

Pulse functional commit `0832d3ea17fa0a224a332d802f0cee00514c7cec` проверен
unit gates на обоих workers:12 новых cases +11 прежних length/resistance
regressions,23/23 normal и23/23 ASan. Компилировались полные production modules
и штатные test files root `b951fb3d44eb713a21d5aefd69b13f0e9440ac77`.
Root proofs `e311c97eeb0eb44270a027b9e2aec68ca16fdeb6` и
`6da55382f2600db42ed69073da281cb67e5626a5` содержат compiler/source/dependency
hashes, stdout/JUnit и loader diagnostics. Все members и9 source hashes
проверены. Это изолированные numerical unit tests, не полный SDK target.

Модуль пока не вызывается NativeTrainingService и не принят в develop.
ControllerVersion2 с SyncCapacitanceTrim On по-прежнему явно отклоняется.
Следующий обязательный шаг — полное C-состояние в action/candidate/evidence,
native comparisonScope provider, joint L/Ns/Rs/C preview/domain и bounded
поиск с исходными CapturedPerInput references и свежей all-input проверкой.
Полные contracts/tests/remaining steps находятся в корневом
Docs/StructTrain/TimeLearner/MeasuredCapacitanceSearch.ru.md. Принятие automatic
C₀ owner идёт отдельно; данный kernel не подтверждает его матрицу или physics.

## Подготовленный native bridge полного C-состояния

После unit130/130 normal и ASan (root proofs8214f206/88e96182) pure candidate
state уже переносит полный absolute scale map. Подготовлены новые методы обоих
learners ReadNativeTrainingCapacitance и ApplyNativeTrainingParameters,
делегирующие общей реализации NativeTrainingCapacitance. Отсутствие bound C₀
возвращает nullopt, а не выдуманную unit baseline. Владелец проверяет signed state,
реальные C/physics и applied model revision. Origin вычисляется из immutable
capture model revision, ModelSpec и storage-images fingerprint; расширение C₀
witnesses после роста не должно менять origin.

Полная controller map объединяет active/dormant scales и доказанные unit paths.
Применение требует неизменных origin/политики и сохранения всей известной истории;
новые paths и их C₀/active-dormant partition проверяет существующий совместный
native L/Ns/Rs/C preview. Смена origin, потеря path, NaN и unbound baseline
отклоняются до live mutation. Методы не вызывают Calculate/Default и не меняют
prototypes. Четыре реальные SDK проверки Classic/Branch × N0/N1 при2000Hz
подготовлены в root NativeTrainingCapacitanceBridge; runtime результат pending.

Это пока механический API, не dispatch NativeTrainingService, не schema3 summary
и не C-trim On. Следом обязательны подключение service/adapters, joint N1 domain
с тем же C, versioned codecs/readers, физические witnesses истории, C policy и
ограниченный измеряемый поиск. CapturedPerInput, anchor/default tolerances и
legacy absolute-F DendriteCapacitance не меняются. В develop bridge не принят.
