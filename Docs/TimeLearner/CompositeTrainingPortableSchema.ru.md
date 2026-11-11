# Composite Train portable contract: schema draft for coordinated implementation

Implementation branch: `codex/timelearner-composite-service`, base Pulse269/Root27ba. Status: source contract for the next service/codec stage; no acceptance or enabled-default claim. Existing cold1–5/protocol1–3/package1–4/certificate1 remain unchanged. Counts mean finite integral JSON numbers, nonboolean, within binary64 exact range, normalized before indexing. Canonical encoding is `length-framed-utf8-binary64-v1`; JSON spelling/whitespace is not a semantic hash algorithm.

## Cold configuration 6

The existing flat cold configuration is preserved with `schema_version=6`, `base_schema_version` in 1..4, and required `composite_measurement`. Base5/recovery nesting is unsupported. The composite object has exactly:

`schema_version=1`, `method_id="declared-window-basin-necessity-v1"`, `context_policy`, `geometry_policy`, `attempt_limits`, `maximum_group_model_seconds`, `maximum_group_wall_seconds`, `maximum_total_primary_bytes`.

`context_policy` keys: schema_version, comparison_radius_ticks, maximum_samples, maximum_total_samples, maximum_native_probes, time_comparison_tolerance, baseline_absolute, repeatability_absolute, repeatability_relative, influence_absolute, influence_relative. The radius/influence fields retain their diagnostic-only role; they are not the new Train criterion.

`geometry_policy` keys: schema_version=1, maximum_inputs, maximum_samples, maximum_total_samples, repeatability_absolute.

`attempt_limits` keys: maximum_groups, maximum_native_probes, maximum_total_samples. Attempt model/wall limits are the existing top-level maximum_model_seconds/maximum_wall_seconds. Existing top-level windows are the declared per-input offsets. Multi-input N>=2, record_spikes=false, peak_selection=isolated_window. N=1 retains the existing path; its eventual multiple-peak contract remains pending.

## Verification protocol 4

All existing protocol keys remain, with schema_version=4, routing=classic_burst/branch_solo_burst, wave_assignment=window_only, peak_selection=isolated_window. Required composite_measurement keys: schema_version=1, method_id, context_policy, geometry_policy, input_windows. Each input window has exactly begin_offset/end_offset, mapping the configured windows. Raw executions remain Recorder6; there is no fabricated Recorder4 or legacy Train observation.

## Source and actual-model identities

`source_origin` hashes the immutable initial target-family `source_binding`; it is not the trained model hash or scientific condition ID. Source binding schema2 retains actual initial_model identity, initial_native_snapshot (existing native XML/identity snapshot), initial_dataset snapshot, initial_epoch, model_spec, full storage_images/storage_images_sha256, applied_source_isi/pattern_hash, actual_source_profile and optional_C0_origin/optional_C0_witness. Its kind remains composite_train_initial_source_v1. Source data are captured before reference acquisition and never recaptured from the learned current graph during warm Reset.

`owner_family_binding` is an opaque target-family identifier. `live_attempt_binding` is a distinct historical attempt identifier. Neither reconstructs process-local live authority when imported. Current native authority receives a fresh Rdk observation and live lease.

The actual trained graph is always package.neuron.graph.identity.sha256 == package.model_revision, verified by actual native import/readback. The initial source snapshot and configuration.model are not substitutes for the trained graph.

## Measurement archive/index 1

The sidecar is `composite-measurements/index.json`. Its exact top-level keys are:

kind=timelearner_composite_measurement_archive, schema_version=1, method_id, protocol, source_origin, owner_family_binding, source_binding, maximum_total_primary_bytes, current_attempt_id, records, reference_group_ids, final_group_ids, counters, primary_blobs, artifact_document_sha256.

`records` contains measurement objects below. Reference IDs index the immutable captured input order; final IDs index the current fresh all-input final set. No partial group may fill either accepted set. `primary_blobs` maps SHA256 strings to objects with exactly bytes/base64. The package is self-contained. If an external resolver is supplied, each primary_ref must also resolve to the same bytes; absence/mismatch is an error, never a silent fallback.

`counters` has attempt={logical_groups,native_probes,sampled_frames} and archive={groups,native_probes,sampled_frames,primary_bytes}. Attempt counters count newly executed work; archive counters include retained reference evidence from older attempts. Execution-wide streaming counts are separately reported by the Console writer.

The archive artifact_document_sha256 hashes the complete object except itself, including relative paths. Semantic measurement IDs exclude primary relative paths. Renaming only CSV paths requires updated artifact hashes, preserving semantic measurement/certificate IDs. Run/trial/group/attempt identities remain part of semantic measurement identity and must not be normalized away.

## Measurement record 1

Exact keys:

kind=timelearner_composite_measurement, schema_version=1, logical_request, purpose, group_id, attempt_id, source_origin, owner_family_binding, live_attempt_binding, method_specification, geometry_policy, candidate_binding, candidate_witness_id, group_state, complete_group, native_group, raw_trials, native_packets, selection, measurement, consumed, training_requested_at_consume, rejection_reason, budgets, counters, clock_observation, capacitance_witnesses, native_trial_witness_ids.

logical_request keys: run_id, trial_id, input_id, sample_id, pattern_hash, protocol_revision, base_model_revision, routing, model_epoch, input_index. purpose is reference/search/final. group_state is idle/running/completed/rejected/cancelled/budget_exhausted. Method specification uses the existing NativeDeclaredWindowBasinSpecification keys: schema_version, method_id, group_id, input_id, assigned_input, channel, window_begin_offset, window_end_offset.

candidate_binding retains kind=composite_train_candidate_binding_v1, source_origin, actual model_identity, storage_images_sha256 and optional_C0_witness. candidate_witness_id is its canonical fingerprint. No late recapture supplies missing historical data.

`native_group` is existing matched-omission portable codec1 only when strict native N+2 closure/context replay succeeds; otherwise null. Its old v1 assignment remains diagnostic and is not a Train criterion. `complete_group` must equal recomputed native closure, not a supplied validity boolean. A complete diagnostic group may have Missing/Ambiguous geometry or be unconsumed after stop.

`raw_trials` contains actual prepared callbacks, including the interrupted final trial. Each has exactly native_trial_id, prepared_context, actual_specification, primary_ref, actual_stimuli. prepared_context is the native prepared declaration, actual_specification is the existing actual native specification. primary_ref has exactly relative_path/bytes/sha256. actual_stimuli entries have id/time/frame_index. Native packets (existing codec2) are separately retained in native_packets and matched by trial ID. An unprepared cancellation may have a packet and no raw-trial/CSV. A prepared interrupted trial may have no closed packet; it cannot supply a complete group, accepted measurement or certificate.

selection is the existing native declared-basin diagnostic result, including canonical status/reasons/actual_assigned_onset and full geometry, or null when not analyzable. measurement is null or {raw_potential,arrival,plateau_end,actual_peak,actual_origin_time,semantic_proof_id}. actual_peak has first_sample,last_sample,amplitude,raw_potential,time,plateau_end. Arrival/plateau are relative to the ACTUAL declared origin event of fullBefore. `consumed` is a service event; training_requested_at_consume is boolean or null if never observed. A diagnostic result alone does not create this event.

budgets keys: attempt_limits (maximum_groups,maximum_native_probes,maximum_total_samples,maximum_model_seconds,maximum_wall_seconds), maximum_group_model_seconds, maximum_group_wall_seconds, maximum_total_primary_bytes. counters keys: native_probes,sampled_frames,retained_frames,primary_bytes. clock_observation keys: start_model_time,start_wall_time,end_model_time,end_wall_time,wall_enforcement. Only actual producer clock observations may be written. Wall enforcement is live_checked_before_mint/not_completed; imported old declarations are not an elapsed-time attestation.

capacitance_witnesses maps witness IDs to existing encoded TrainingCapacitanceWitness objects; native_trial_witness_ids maps actual queued native trial IDs (not logical group IDs) to those witness IDs. Each actual raw trial has before/after native C0 verification when C0 exists. Both maps are empty when actual C0 is absent; this is not an invented unit baseline. Candidate/source bindings retain full actual current/initial witnesses separately.

## Package 5 and certificate 2

Package5 is a new flat schema, not a legacy scalar packet envelope. Required common fields: kind (timelearner_completed_state/timelearner_stopped_state), schema_version=5, dynamic_checkpoint=false, state_kind (learned_solo/diagnostic), implementation_id, controller_version, calculate_mode, structure_build_mode, configuration(immutable initial cold6 planning declaration), runtime_plan, protocol4, trained_inputs(actual), model_revision, model_epoch, session_attempt, training_outcome, reason, certificate, capture_revision, capture_epoch, reference_complete, measurement_archive, dataset(actual), neuron(actual native archive), capacitance_state(nullable), sha256. Complete/stopped distinction must match certificate presence and verified accepted evidence. No mid-probe continuation or serialized live callbacks are represented.

Certificate2 exact keys: schema_version=2, method_id, exact, model_revision, model_epoch, protocol_revision, source_origin, owner_family_binding, reference_model_revision, reference_model_epoch, reference_semantic_proof_ids, final_semantic_proof_ids, tolerances, residuals, semantic_certificate_id. Tolerances have amplitude_absolute/amplitude_relative/sync; residual fields retain the old names/math. Certificate IDs hash the object except their own ID, containing semantic proof IDs but no filesystem paths. Protocol4 has its own decoder; it must not reuse characterization ProbeSpec4.

## Console files

Existing train_learned_state.json/train_stopped_state.json and corresponding summary filenames remain. New group sidecars are composite-measurements/group-000001.json, etc.; primary files are composite-measurements/group-000001/full-before.csv, omit-0.csv ... full-after.csv (only actually prepared traces exist). Group numbering is a filesystem counter, not group ID; arbitrary IDs never become paths. Existing native flat trial files remain raw execution evidence, not scalar Train measurements.

Initial-state schema1 and training-job schema1/2 keep their existing meanings. New training execution schema3 is a versioned envelope over base schema1/2, with measurement_method, measurement_archive (path/bytes/sha256), model_semantic_hash, source_origin, composite_counters. composite_counters has attempt/archive above plus execution={native_trials,sampled_frames}. New summary schema6 similarly carries method, source/actual-model identities, references/final semantic IDs and distinct counters. Exact summary field list is finalized with its writer, not guessed from the C++ fixture.

`attempt_id` and `current_attempt_id` are explicit accounting identities. The service attempt ID is stable across owned edits, changes on warm restart, and binds the exact logical run/trial namespace and purpose. Current-attempt counters sum only records with that exact attempt_id; old reference groups remain archive work. These IDs do not attest native execution or recreate live authority.

## Runtime plan1

The package retains immutable initial planning configuration separately from the current runtime plan. Exact runtime_plan keys: schema_version=1, run_id, initial_model_revision, initial_model_epoch, search, maximum_reference_probes, maximum_invalid_reference_probes, maximum_model_seconds, maximum_wall_seconds, record_capacitance_history, composite_measurement. Exact search keys: normalization(N0/N1), maximum_probes, maximum_edits, maximum_candidates_per_input, maximum_sweeps, maximum_invalid_observations, resistance_subdivision_depth, n0_candidate_repeats, bounds, tolerances, capacitance_search(nullable existing policy), invalid_measurement_recovery=retry_same_candidate. Bounds/tolerance shapes match cold declaration.

Initial configuration is a planning declaration whose input parameters are captured from actual initial readback; source model_spec remains the owner's creation policy. Neither replaces actual initial native snapshot/storage or the actual trained native archive. Existing C0 can be present with C-trim Off and record_capacitance_history=false; raw actual C0 evidence is still retained.

## Summary6

Exact fields: kind=native_training_state_summary, schema_version=6, state, outcome, reason, model_state_kind, actual_model_revision, model_epoch, actual_parameters, tolerances, runtime_plan, measurement_method, source_origin, owner_family_binding, reference_complete, capture_revision, capture_epoch, reference_group_ids, final_group_ids, reference_semantic_proof_ids, final_semantic_proof_ids, composite_counters, certificate, certificate_matches_actual_model, contains_evaluation_verdict=false, search_history_retained, candidates, rejections, resistance_domains, accepted_parameters, applied_edits, search_probes, references. Imported boundaries without retained search use empty history arrays and null accepted_parameters/applied_edits/search_probes. References retain measurement_group_id/semantic_proof_id; candidate records retain measurement_group_id/semantic_proof_id and anchor_group_id/anchor_semantic_proof_id in addition to old numeric fields. This summary is optional for standalone package5 audit; actual pipeline execution separately checks complete writer artifact closure.
