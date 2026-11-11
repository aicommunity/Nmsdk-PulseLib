# Runtime2: декларация фактического плана service5

Статус: исходники и серверные проверки подготовлены; сборка и исполнение ещё
не выполнены. Это отдельная версия декларации, не cold7, не checkpoint и не
разрешение на native execution. Существующий runtime1 codec не изменён.

## Владение и момент сохранения

`EncodeCandidateWindowRuntimePlan(const NativeTrainingPlan&)` читает план,
который `NativeTrainingService::StartCandidateWindowComposite` публикует только
после подготовки actor, проверки boundary/clock и успешного commit. Для service
producer вызывается с `service.Plan()`, а не с декларацией до `Start`.
`run_id` уже содержит фактическую session, `initial_model_revision/epoch`
описывают начало текущего runtime attempt. Исходный cold7 хранится отдельно в
`service.CompositeInitialConfiguration()` и не переснимается с обученного графа
при warm restart. Runtime search, tolerances и caps могут отличаться от cold7;
это не основание их нормализовать к первоначальным значениям.

Exporter не вызывает `Build`, `Reset`, `Calculate`, callbacks, native readback,
обновление clocks или выдачу новой lease. Он не меняет service, graph, C0 или
историю. Закрытый service можно сохранить как DATA, не оживляя его полномочия.

## Закрытая схема

Top-level поля строго следующие:

```text
schema_version = 2
protocol_binding = CanonicalModelDataSha256(EncodeVerificationProtocol(protocol5))
run_id, initial_model_revision, initial_model_epoch
search
maximum_reference_probes, maximum_invalid_reference_probes
maximum_model_seconds, maximum_wall_seconds
record_capacitance_history
composite_measurement
```

`search` сохраняет normalization `N0`/`N1`, maximum_probes, maximum_edits,
maximum_candidates_per_input, maximum_sweeps, maximum_invalid_observations,
resistance_subdivision_depth, n0_candidate_repeats, bounds, tolerances,
capacitance_search или null, invalid_measurement_recovery
`retry_same_candidate`. Bounds содержат minimum/maximum length, synapses и
resistance; tolerances — amplitude_absolute, amplitude_relative, sync.

`composite_measurement` содержит общий policy schema2: method_id, context_policy,
geometry_policy, attempt_limits, maximum_group_model_seconds,
maximum_group_wall_seconds, maximum_total_primary_bytes, candidate_window_policy.
Последний берётся из реального protocol5 и при decode должен совпасть с ним.

Counts конечны, целочисленны, не bool и не превышают 2^53−1; integer bounds
помещаются в int. Search limits/ranges, повторения N0/N1, recovery и C-search
проходят соответствующие проверки деклараций. Native probe cap не меньше N+2,
sample cap не меньше context maximum_total_samples, reference cap не меньше N;
group time limits положительны и не превышают attempt time limits. Это разные
единицы, равенство search probes и raw native probes не требуется. Нулевые
invalid allowances допустимы. Никакие сравнения с первоначальными search/caps
cold7 не заменяют проверку текущих runtime declarations.

Флаг `record_capacitance_history` сохраняется независимо от C-search declaration:
низкоуровневый service5 проверяет фактический owned C0 отдельно и допускает
план без записи истории. Более строгое правило cold7 builder не переносится
в runtime codec. Само наличие C-search в JSON не доказывает наличие C0.

## Декодирование и предел доказательства

`DecodeCandidateWindowRuntimePlan(object, protocol5)` проверяет exact fields,
protocol digest, полный policy2 и числовые ограничения. Как runtime1 decoder,
он возвращает только сериализуемую часть `NativeTrainingPlan`: protocol,
search и budgets. Probe executor, inputWindows, callbacks, sinks, Rdk owner,
native observation, lease и certificate не восстанавливаются. Результат нельзя
выдавать за готовый исполняемый training plan или live-authority token.

Low-level `NativeCompositeTrainingAttempt` владеет protocol/limits/attempt
declaration, но не service search plan/cold7. Его исторические archive2 нельзя
постфактум дополнить runtime2 из index. Manual-runtime contract здесь не введён.
Первый полный CLI gate должен использовать настоящий service5 exporter.

## Проверки после source review

Подготовлены 11 `CandidateWindowRuntime2.*` declaration tests: positive roundtrip,
владение JSON numbers после сериализации, независимость runtime от cold7,
protocol/policy tamper, unknown/schema/type/count rejection, search/ranges,
budget inequalities, unbound/recovery rejection, C-search/history DATA и сохранение
runtime1. Они синтетические, без waveform/native claims.

Actual `SaveWindowService` отдельно проверяет encode/decode/encode на настоящем
service plan. Последующий серверный gate обязан сохранить original cold7,
runtime2, archive2 и все CSV, затем выполнить независимый Python full auditor и
публичный CLI. Codecs и synthetic tests сами по себе не принимают actual data.
