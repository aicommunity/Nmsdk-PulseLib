# Portable matched-omission evidence, schema1

Новый codec `Core/TimeLearner/MatchedOmissionEvidenceJson.h/.cpp` отделён от
исторического `native_matched_omission_fixture`. Критерий v1 и native schemas1–6
не меняются. Закрытая составная запись имеет kind
`timelearner_matched_omission_evidence`, schema_version1 и method
`matched-omission-assigned-maximum-v1`.

API принимает/возвращает `NativeMatchedOmissionResult`, дополненный полной
policy, planned sourceIsi, actual appliedSourceIsi/appliedPresentedIsi и
объявленными maximumModelSeconds/maximumWallSeconds. Actual readbacks native
group читает из MatrixData в Start и onPrepared; они не синтезируются из плана.
Primary map — `std::map<QString,QByteArray>`, filesystem I/O остаётся caller.

## Закрытые JSON поля

```text
TOP = schema_version kind method complete_group diagnostic_only
      training_certificate_eligible scientific_verdict specification policy
      source time_budget traces assignment attestation

SPECIFICATION = schema_version run_id group_id sample_id protocol_revision
      pattern_hash input_id base_model_revision effective_model_revision
      reset_binding model_instance_binding model_epoch assigned_input channel
      routing stimulus_ids physical_channel_paths stimulus_profile duration
      minimum_amplitude

PROFILE = generator_class amplitude pulse_length frequency_hz
      frequency_deviation use_transit_signal use_pattern_output

POLICY = schema_version comparison_radius_ticks maximum_samples
      maximum_total_samples maximum_native_probes time_comparison_tolerance
      baseline_absolute repeatability_absolute repeatability_relative
      influence_absolute influence_relative

SOURCE = planned_isi applied_isi planned_sha256 applied_sha256 canonical_encoding
TIME_BUDGET = maximum_model_seconds maximum_wall_seconds elapsed_model_seconds
      elapsed_wall_seconds enforcement_verified

TRACE = role omitted_input presented_ordinals planned_presented_isi
      applied_presented_isi presented_pattern_sha256 relative_path bytes sha256
      native_evidence binding
BINDING = group_id source_pattern_hash reset_binding model_instance_binding
      current_base_revision native_completed full_mask_restored physical_channel_paths

ASSIGNMENT = validity reasons maxima selected_full_maximum measured_maximum
MAXIMUM = first_sample last_sample amplitude raw_potential time plateau_end
SUPPORT_MAXIMUM = first_sample last_sample amplitude raw_potential time plateau_end
      comparison_first_sample comparison_last_sample
      influence_by_input supported_inputs unknown precedes_assigned_stimulus clipped

ATTESTATION = native_execution_verified physical_paths_independently_verified
      reset_instance_ownership_independently_verified
```

Все schema/count/index поля — конечные неотрицательные целые, точно
представимые в JSON (<=2^53-1); bool ими не является. Другие числовые значения
конечны; unknown fields запрещены. QJsonObject уже не содержит raw duplicate
keys: для внешнего JSON использовать byte overload строгого parser.

`complete_group=true`, `diagnostic_only=true`,
`training_certificate_eligible=false`, `scientific_verdict=null` обязательны.
Порядок traces фиксирован: full_before (omitted_input=null), N omit controls
(role=omit, omitted_input=ordinal0..N-1), full_after (omitted_input=null).
Каждый native_evidence — существующий canonical schema2 packet со schema6
полным waveform observation. Все CSV уникальны и внешний map содержит ровно
заявленные безопасные относительные пути.

Source vectors planned/applied должны совпасть; их hashes вычисляются
`CanonicalModelDataSha256` с encoding
`length-framed-utf8-binary64-v1`. Для каждой маски план вычисляется
`PlanStimulusSubset`, сверяется с реальным applied readback и с pattern hash
requested/observed native packet. Это не утверждает равенство cumulative
planned ISI и actual event timestamps: quantization учитывается отдельными
неизменёнными frame/event проверками selector v1.

`nativeCompleted/fullMaskRestored` и context bindings сохраняются в binding.
Импорт проверяет packet и CSV повторным Arm/Append/CloseNative, а затем
заново вычисляет assignment. Valid/Missing/Ambiguous/Clipped/NonRepeatable
являются законченными operational outcomes. Invalid context, incomplete,
budget exhaustion, unsupported scope и частичные группы не выдаются за
complete portable composite.

CSV использует существующий native-probe-csv-v1: header
`time,stimulus,soma0,...`, absolute native timestamps, classic locale,
binary6417g, LF. Bytes/hash привязываются к exact canonical CSV. Assignment
максимума содержит actual rawPotential/time из fullBefore, не difference max.

Time budgets записываются как declarations. Elapsed model/wall values здесь
отсутствуют и равны null, enforcement_verified=false. Все ATTESTATION flags
равны false: portable packet replay не является независимым доказательством
живого scheduler, принадлежности объектов или reset ownership. Samples/probe
budgets и source/context consistency проверяются по полной записи.

В фиксированном native order следующий actual spec.begin строго больше
предыдущего spec.end. Наблюдаемый span `last.spec.end-first.spec.begin` должен
быть конечным, положительным и не превышать объявленного model budget. Это
только нижняя граница фактического времени группы; полное model elapsed и
тем более wall elapsed из неё не восстанавливаются.

Серверные codec/native normal/ASan gates и Python cross-language reader
принимаются отдельно. Локальные численные тесты не выполняются.
