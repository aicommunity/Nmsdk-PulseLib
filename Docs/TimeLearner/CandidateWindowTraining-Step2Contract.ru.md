# Candidate-window STEP2: typed boundary и wire contract

Первый review pin `5e4faf0` содержал только types/schema. Текущий source-подэтап добавляет strict protocol5/contract2/spec2 declaration codecs, diagnostic native spec2 projection и общий proposal DATA codec/replay над opaque complete inputs. Native/portable reference-set minting, archive2 import, native5 admission, service/CLI/defaults ещё отсутствуют. Численные проверки этой реализации не выполнены. Успех pure40 Rootdf06/Pulsee7 подтверждает арифметику, а не право создать reference seed или обучающее измерение.

## Версии и совместимость

Новая declaration ветвь реализована только как VerificationProtocol5 / CompositeMeasurementContract2 и basin specification2. Protocol4/contract1 продолжает означать прежние фиксированные seconds windows; его поля и смысл не меняются, `schema>=4` не используется. Следующие точные envelopes: cold7, candidate binding2, measurement record2/archive2, learned6, summary7, certificate3, Console execution4 — пока не декодируются и не активируются. Raw ProbeRecorder1–6, native packet2 и matched portable1 остаются прежними. Protocol5 declaration roundtrip не разрешает старую raw/schema3 или composite4 Train admission: текущие typed factories/consumers5 fail closed.

Новый protocol5 сохраняет остальные существующие поля protocol4 и использует `composite_measurement` с ТОЧНЫМИ ключами:

| Ключ | Тип / значение |
|---|---|
| `schema_version` | count2 |
| `method_id` | `declared-window-basin-necessity-v1` |
| `context_policy` | полная existing matched context policy1 без изменения критерия |
| `geometry_policy` | полная existing basin policy1 |
| `reference_input_windows` | N объектов с точными ключами `begin_offset`, `end_offset`; seconds относительно actual fullBefore begin |
| `candidate_window_policy` | полная policy1, указанная ниже |

`input_windows` в contract2 запрещён: такой alias мог бы скрыть смешение fixed и reference-only смысла. Reference windows неизменны для target family. Все unknown/missing/duplicate keys и неизвестные enum/version отклоняются. На JSON-границе counts — конечные integral-valued JSON numbers, не bool, 0..2^53−1 и в диапазоне целевого C++ типа; 1.0 эквивалентно1 после строгой проверки. Physics numbers остаются finite numeric, а не count. Никаких значений по умолчанию для пропущенных proof parameters.

`candidate_window_policy` имеет точные ключи `schema_version`, `policy_id`, `seed_rule`, `coordinate_mode`, `proposals_per_logical_request`, `maximum_feature_terms`, `inputs`. Известные значения и арифметика совпадают с `CandidateWindowPolicy-Step1.ru.md`: schema1, `reference-anchored-candidate-window-v1`, `immutable_reference_full_before`, `relative_native_frame_interval`, одно предложение на logical request. Каждый элемент `inputs` содержит ТОЛЬКО `delay_frames_per_segment`, `base_left_guard_frames`, `base_right_guard_frames`, `length_guard_frames`, `synapse_count_guard_frames`, `resistance_change_guard_frames`, `capacitance_change_guard_frames`, `hard_first_frame`, `hard_last_frame`, `maximum_window_frames`.

## От observed reference к complete seed set

Native attempt вправе staged-сохранить seed только после полного общего N+2 context/native replay, actual basin selection и typed reference admission. Проверяются purpose=Reference, исходные model revision/epoch, input/stimulus ordinal, actual source/reset/grid, selected fullBefore plateau sample indices и actual original P0/C0. Target raw potential остаётся в `CapturedReferences`; он не заменяется новым пиком и не участвует в proposal arithmetic.

Staged seed не возвращается search/final. `CapturedReferences::CaptureComposite` сначала принимает ВСЕ N разных inputs одной immutable family/original model, после чего может выпустить `CandidateWindowReferenceSet`. Частичный prefix, diagnostic result, raw DATA, `valid=true`, copied arithmetic или arbitrary paths не конструируют этот закрытый тип. Future minting code должен сверить ordered proof IDs с complete captured reference set. Header пока не реализует ни одного minting пути.

Warm использует только заново проверенные portable primary reference groups и их первоначальные P0/C0/source/family. Чтение текущего обученного графа не может заменить immutable reference seed. `Reset` отзывает inflight live generation, сохраняя допустимую complete target family; Default/pool/retarget инвалидируют family. После warm новый owner проверяется по фактическому current graph/source/storage/C0, но targets/P0 не переснимаются.

## Actual candidate, proposal и границы допуска

Native bridge читает actual graph через существующие owner/model APIs и независимый full semantic identity, а не только `NativeModelDeclaration.inputs` или learner arrays. Для каждого input сохраняет actual parameters и membership его excitation synapses в нужном bank/cable/channel; проверяет реальные property links к source boundary и физический ordinal. Branch one-hot `IsConnected` после подготовки проверяется относительно именно этого bank; совпадение protocol input count не доказывает routing. Shared bank сам по себе не объявляется автоматически запрещённой моделью; он не заменяет distinct-bank acceptance для N4/N6.

`CandidateWindowInputBankReadback` соответствует точным ключам `input_ordinal`, `input_id`, `stimulus_id`, `receiver_path`, `bank_path`, `attachment_segment`, `excitation_synapse_paths`. Paths используют semantic root `$`; `bank_path` обозначает membrane (`$.Dendrite1_2`), `receiver_path` — её actual excitation receiver (`$.Dendrite1_2.ExcChannel`), а не измеряемую soma. Measured soma остаётся отдельно связана через native physicalChannelPaths/routing. Membership проверяет property-pointer bindings каждого owned synapse Output к receiver SynapticInputs. Другие inputs/дубли receiver не скрываются и остаются в полном identity; это не требование эксклюзивного/линейного канала. `CandidateWindowPhysicalReadback`: `parameters`, `input_banks`, `capacitance`. Parameters имеют existing точные ключи `length`, `synapses`, `resistance`. Capacitance — null либо existing `{origin, maximum_relative_change, membrane_scales}`; CtrimOff не требует искусственного C property/C0. Полные graph identity/links, actual C0 witness и storage image hashes сохраняются в candidate binding2 наряду с readbacks, не заменяются path strings.

Candidate binding2 имеет точные ключи `kind`, `schema_version`, `source_origin`, `model_identity`, `storage_images_sha256`, `optional_C0_witness`, `physical_readback`. Kind=`composite_train_candidate_binding_v2`, schema2. `physical_readback.capacitance` должен точно соответствовать parameters existing optional C0 witness; отсутствие обоих допустимо. Base semantic identity описывает восстановленную declared baseline routing candidate structure; existing `allow_muted_input_baseline` поддерживается. Effective per-mask identity/routing отдельно проверяются native packets. Binding2 не объявляет transient Branch mute новым P0 или structural edit.

`NativeCandidateWindowSnapshot` может создать только фактический native owner factory. Он связывает проверенные facts с source/family, candidate revision/epoch и live identity. `PreparedCandidateWindowProposal` замораживается до ПЕРВОГО fullBefore queue; ровно один proposal, одна группа N+2, один logical Arm/Consume. До prepare, после closure и перед consume повторно проверяются actual owner lifetime/Need/source/storage/model/C0/input bank map. Owned structural edit создаёт новую revision/epoch/proposal; rollback к старому semantic hash не возвращает старую live authority.

Даже private prepared proposal не разрешает scalar admission: требуется отдельный строгий portable proposal replay из реальных reference primary + candidate binding. `ReplayedCandidateWindowProposal` имеет отдельный private factory, недоступный native attempt напрямую. No public bool, decoded diagnostic whitelist или live callback/address не подменяет replay. `CandidateWindowProposalReplayFactory` уже реализован как общий DATA replay, но требует opaque complete ReferenceSet; native/portable factories полного reference set и действующий native5 admission ещё отсутствуют.

## Proposal1: точные поля и hashes

Будущий proposal document имеет точные ключи:

```
kind schema_version policy_id source_origin owner_family_binding reference_proof_id
protocol_binding policy_binding candidate_binding reference_group_id
attempt_id run_id trial_id group_id input_id input_index channel model_epoch purpose
candidate_model_revision window_recipe_id measurement_instance_id
reference candidate computation
```

`kind=composite_candidate_window_proposal_v1`, schema1; purpose только `search`/`final`. Reference groups не притворяются candidate proposal: в record2 их `window_proposal=null`, coordinate=`reference_time`, и сохраняется исходная spec. Search/Final требуют non-null proposal, complete reference set и соответствующий actual input. Reference seed snapshots в record2 не являются публичной minting authority.

`reference` имеет точные ключи `input_index`, `first_frame`, `last_frame`, `frame_count`, `grid`, `initial_parameters`, `initial_capacitance`. Plateau indices взяты из replay actual reference fullBefore selected maximum, frame_count равен числу реально сохранённых samples. `candidate` имеет `grid`, `declared_maximum_samples`, `parameters`, `capacitance`. `grid` в обоих содержит `frequency_hz`, `source_origin`, `source_pattern_binding`, `source_profile_binding`, `frame_origin_policy`, `source_phase_policy`, `reset_binding`; известные tags — прежние STEP1 tags, frequency positive integral count. Actual source profile/ISI берутся из source binding/primary, не из суммы intended ISI.

`computation` содержит `status`, `first_frame`, `last_frame`, `shift_frames`, `uncertainty_frames`, `length_change`, `synapse_count_change`, `resistance_changes`, `capacitance_changes`, `feature_terms`. Для admitted proposal status ТОЛЬКО `computed`, frame endpoints non-null, shift signed exact integer в ±(2^53−1), остальные counts nonnegative exact. Failed arithmetic может сохраняться как отдельная diagnostic DATA с reasons; оно не создаёт подготовленную группу/proposal proof и не требует fabricated native record.

Все semantic hashes используют existing `CanonicalModelDataSha256` / `length-framed-utf8-binary64-v1`, а не Qt JSON byte formatting. Нормативные hashing objects:

- `policy_binding`: полный `candidate_window_policy`.
- `protocol_binding`: полный protocol5; primary filesystem paths не входят.
- `candidate_binding`: полный actual candidate binding2, включая physical readbacks/full identity/C0/storage/source.
- `window_recipe_id`: объект `{kind:"candidate_window_recipe_v1", policy_binding, reference_proof_id, source_origin, owner_family_binding, protocol_binding, input_id, input_index, channel, candidate_model_revision, model_epoch, candidate_binding, reference, candidate, computation}`.
- `measurement_instance_id`: объект `{kind:"candidate_window_instance_v1", window_recipe_id, attempt_id, run_id, trial_id, group_id, purpose}`.

Recipe не содержит live addresses/callbacks/generation, текущую амплитуду/residual, artifact document hash или CSV путь. Instance намеренно связывает anti-replay logical IDs. Смена path меняет artifact document SHA, не semantic recipe/instance. Смена logical IDs не является простым переименованием primary file и меняет instance. Record/archive2 SemanticProofId обязан включать всю spec2 и recipe/instance bindings; старый proof1 не меняется. Empty/mismatched hashes отвергаются после fresh recomputation; проверки одного строкового equality недостаточно.

Hash bindings/recipe/instance IDs в proposal1 имеют canonical 64 lowercase hex формат. DATA codec проверяет exact shape/counts/grid, self-binding recipe/instance hashes и strict duplicate-free JSON byte boundary (existing parser, max128MiB). Он не утверждает arithmetic/native validity по одному self-consistent документу: полный policy и настоящие immutable references требуются общему Replay. `Compute` возвращает DATA; `Replay` заново выводит current features из полного binding/graph, пересчитывает арифметику/IDs и сравнивает весь документ. Closed archived candidate binding не содержит живых callbacks/addresses и не подтверждает исполнение. Controlled Rs соответствует существующему `ReadNativeInputParameters` control knob, включая его прежний запрет heterogeneous Rs В ОДНОМ controlled bank; остальные physics/weights/links сохраняются целиком в identity. Новых child writes/uniformity repair нет.

Перед обходом C0 maps проверяются фактические размеры относительно feature budget; current unit paths остаются допустимы по existing `EquivalentTrainingCapacitance`. Native scope current length/Ns проверяется по existing model declaration limit/128-member contract. Membership требует присутствия каждого controlled synapse в actual receiver bindings; не вводится лишний запрет на другие известные receiver inputs/fanout/duplicates. Base muted routing допускается только по прежнему explicit native model declaration.

`NativeCandidateWindowBindings.h/.cpp` — фактическое read-only основание следующего snapshot factory. `ReadNativeCandidateWindowPhysical` читает existing actual control knobs через ReadNativeInputParameters, затем независимо проверяет bank/receiver/source/member pointers и сохраняет DATA. `ValidateNativeCandidateWindowPreparedSelector` вызывается внутри actual raw onPrepared после маршрутизации, проверяет declared input ordinal, настоящий vector soloSynapses (включая declared inhibition) и source connections всех mapped banks. Общий/native lifetime/revision/clock guard остаётся у вызывающего actor. Эти DATA functions сами не создают ReferenceSet/NativeSnapshot/Train proof; не вызывают Calculate/Build/Reset/Default и не меняют свойств/links/storage.

Новая record2 сохраняет ТОЧНЫЕ existing record1 keys, меняет schema_version на2, требует method_specification2 и candidate_binding2 и добавляет единственный обязательный ключ `window_proposal` (null только для Reference). Никаких неявных optional extension keys. Archive2 сохраняет existing archive1 keys, меняет schema_version на2 и требует protocol5/records2; новая политика находится в полном protocol, поэтому второй несвязанный policy overlay или caller-supplied seed array не добавляется. Reference group IDs указывают на все N настоящих immutable reference records с их первичными fullBefore/omit/repeat. Portable replay заново вычисляет seed из этих records, сверяет P0 с source binding2 и текущие features с actual candidate identity/readback. Полный recorded proposal и метод selection пересчитываются до создания replay wrapper. Partial records могут хранить prepared proposal, но без complete group/actual frames не имеют admitted measurement/semanticproof/certificate. Полные learned6/summary7/certificate3/execution4 field sets ещё не заморожены этим STEP2 commit и потребуют отдельного review; они не должны падать в старые версии.

## Frame specification2 и реальные timestamps

Spec2 имеет точные ключи `schema_version`, `method_id`, `group_id`, `input_id`, `assigned_input`, `channel`, `coordinate_mode`, `reference_time`, `candidate_frame`.

- `reference_time`: coordinate_mode=`reference_time`, объект `{begin_offset,end_offset}`, `candidate_frame=null`; только Reference purpose.
- `candidate_frame`: coordinate_mode=`candidate_frame`, объект `{first_frame,last_frame}`, `reference_time=null`; только Search/Final и interval точно равен computed proposal.

Window для ОДНОЙ группы фиксируется до queue. При closure native/context validator сначала доказывает одинаковые actual frame count/order, retained stimulus ordinal/frame, source waveform, model/reset/baseline и repeatability. Только затем требуют `last_frame < actual sample count` и берут временные границы строго из actual fullBefore.samples[first/last].time. Никаких `begin+index/f`, round-to-tick, interpolation, padding, автоматической one-tick tolerance или post-hoc подстройки. Если frames не захвачены, отказ `window_frames_not_captured` без scalar/certificate. Форма выделяется на полном waveform, а не на обрезанном окне; existing basin missing/merged/tie/boundary правила сохраняются. Actual selected peak должен быть не раньше actual assigned event sample/time.

Между независимыми reference/candidate groups STEP1 source-phase tag означает одинаковую декларацию source, не теорему exact quantized onset. Нельзя скрыто требовать cross-group frame equality либо сдвигать predictor по увиденному onset: explicit known policy использует immutable seed и actual current structural facts. Внутригрупповая strict retained event equality остаётся обязательной. Честный Missing при source phase/predictor промахе не заменяется выбранным peak. Arrival для scalar math считается относительно ACTUAL protocol origin event ЭТОЙ группы.

## Последовательность реализации и адресные проверки

Следующий обязательный code подэтап: общий live/portable proposal validator; actual reference staging/complete release; actual physical parameter+bank readback; strict recipe replay, затем owned native bridge. Spec2 projection уже использует общий context validator и вынесенный неизменный native envelope validator; он остаётся diagnostic selector без authority. Service/CLI/default dispatch не добавляется. Пока protocol5 native factory/admission отсутствует, запрос5 к старому actor отклоняется, а не падает в protocol4.

Необходимые настоящие controls: Classic/Branch N2 growth peak вне initial window и внутри deterministic proposal; unchanged/warm references; CtrimOff без C0; actual C0 edit; wrong seed/proof/family/currentattempt/revision/epoch; stage prefix не usable; source/storage edit доprepare/closure/consume; Need=false/lifecycle invalidation между closure и consume; tampered recipe/interval/readback/hash/primary; insufficient actual frames; shared/merged/boundary honest refusal. N4/N6 Branch positive routing использует distinct attachments `[N,N−1,...,1]`, сохраняет реальные banks/IsConnected/source ordinal; прежний shared-bank counterexample остаётся отдельным negative/plumbing контролем. Не обещать Exact при неизвестной физической сходимости и не увеличивать `.02`/амплитудные tolerances ради теста.

Все реальные fixture groups/raw CSV/соматические каналы/actual input, исходный и текущий model snapshot, proposal и primary source должны сохраняться через production writer для независимого replay. Локальные C++/numerical tests не запускались. Сборки/численные проверки следующего code выполняются только отдельными pinned server gates; pure40, frozen206/214/229/240 и их evidence остаются исторически точными.
