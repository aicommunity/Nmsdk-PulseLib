# CandidateWindowPolicy: STEP1 pure arithmetic

Статус: source preparation only. Локально не выполнялись C++ compilation, syntax-only build или numerical/unit tests. Новый модуль ещё не включён в native bridge/service/config/Train/defaults; runtime acceptance отсутствует.

## Контракт

`CandidateWindowPolicy.h/.cpp` реализует policy `reference-anchored-candidate-window-v1`, schema1, seed_rule=`immutable_reference_full_before`, coordinate_mode=`relative_native_frame_interval`, proposals_per_logical_request=1. Все другие version/strategy tags отвергаются, включая два одинаковых неизвестных grid/source-phase/reset tag. Числовые counts ограничены exact binary64 range 2^53−1; source/frequency bindings непустые и совпадают точно.

Первая функция `CalculateCandidateWindowArithmetic(policy, untrustedSeedFacts, untrustedCurrentFacts)` возвращает только арифметические DATA с enum status/reason/optional window, не measured response и не proof. Вход не содержит waveform, amplitude, residual, candidate score или valid=true. Положительный результат на придуманных фактах не подтверждает ни source, ни пик, ни исполнение.

`ProposeCandidateWindow` требует отдельный opaque `CandidateWindowReferenceSeed` с private raw constructor. STEP1 не добавляет никакого minting factory: будущий actual reference bridge обязан создать его из проверенного captured reference/fullBefore. Static assertions проверяют невозможность default/aggregate/raw construction и преобразования arithmetic result в seed. Copyable seed не снимает будущую проверку expected owner family/reference proof. Даже typed Propose всё ещё возвращает arithmetic DATA: candidate facts до native/map проверки недоверенные.

## Арифметика и бюджеты

Используется неизменный seed plateau [firstFrame,lastFrame], исходный full InputParameters и отдельные current facts. Shift зависит только от signed ΔL закреплённого input и явно заданного delayFramesPerSegment. Uncertainty суммирует все ΔL, ΔNs, число exact Rs changes и 0/1 изменения C correction по existing EquivalentTrainingCapacitance. Branch не получает предположения независимости других inputs. Никаких log/EMA/FMA-правил или изменения scalar train tolerances нет.

Рабочий бюджет maximumFeatureTerms считает 3*N структурных terms плюс размеры initial/current C0 maps. Container sizes и dimension проверяются до сканирования/умножения. Затем отдельно проверяются additions/products/signed frames, hard envelope, inclusive maximumWindowFrames и declaredMaximumSamples. Ошибка оставляет window пустым. Negative/outside interval не обрезается и не сдвигается к удобным bounds.

Ctrim Off не требует C property/C0. Оба optional C states могут отсутствовать; новые unit paths эквивалентны прежнему correction. Потеря/изобретение C0 либо смена origin/maximumRelativeChange отклоняются, а не превращаются в larger uncertainty. Используется существующий C-state validator; новые physics limits на old components не вводятся.

## Grid и source phase — точные ограничения scope

Поддерживаются frameOriginPolicy=`native-full-before-frame-zero-v1`, sourcePhasePolicy=`same-declared-source-native-events-measured-v1`, resetBinding=`native-neuron-dataset-reset-before-children-v1`. Frame0 означает первый сохранённый sample fullBefore после объявленного native reset/before-children protocol. Frequency/source origin/pattern/profile declarations должны совпадать между initial seed и current facts.

Равенство строк не доказывает реальный reset/routing или равенство quantized event frames между независимыми группами. Actual generator phase/ISI/event timestamps ещё не проверяются STEP1. Будущий bridge должен повторить общие N+2 event/grid/source/model/reset checks, проверить actual candidate ordinal map и наличие объявленных retained frames. Внутри одной N+2 группы retained event/frame equality остаётся строгой. Прогноз не может разрешить one-tick drift.

Между независимыми reference/candidate группами source quantization может изменить положение actual event. STEP1 не корректирует окно по наблюдённой phase и не добавляет dt к SyncTol; возможный промах остаётся honest Missing/constraint. Если более поздняя policy требует equal cross-group source phase, ей понадобится explicit actual check, не совпадение этого declaration ID.

Числовые frame endpoints фиксируются до первой candidate full trace. Будущий bridge переводит их в actual timestamps только через сохранённые samples с этими индексами, без интерполяции, padding или сдвига waveform. Pure API не утверждает, что эти samples реально захвачены.

## Что не реализовано

Нет создания validated seed из native data, policy/source hashes/recipe-instance proof codec, prepared/consume admission, candidate physical feature/map witness, protocol5/cold7/new archive/package schemas, CLI activation или новой final verification. Старые fixed protocol4/cold6 pathways и raw schemas1–6 не меняются. Build/Calculate/Reset/Default не вызываются, их bool не используется; storage physics и scheduler не затронуты.

N4/N6 в pure suite проверяют размерность фактов `[N..1]`, а не actual distinct-bank routing. Для полного native integration обязательны отдельные Classic/Branch фактические controls, Branch distinct attachments `[N,N−1,...,1]`, с сохранёнными real ordinal/bank readbacks. Shared-bank source206 controls сохраняют самостоятельный отрицательный/plumbing scope. Single-input arithmetic допустима математически, но не объявляет поддержки нового native N=1 Train/multi-peak контракта.

Следующий шаг после source review — scoped remote normal/ASan pure gate, затем отдельный review bridge/service и runtime prerequisites. Подготовка чистого source STEP1 не ждёт всех240; допуск native integration/defaults по-прежнему требует принятых соответствующих evidence gates.
