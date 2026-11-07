# Установка готового физического графа в свежий learner

Адресно проверенный подэтап4/5/6. InstallNativePreparedModel доступен у Classic и Branch;
общая реализация InstallNativePreparedChildren находится в NativeModelOwner.h.
Адресная normal-приёмка M40 выполнена; ASan и второй сервер открыты.

Вход: полный NativeModelSpec, готовые не принадлежащие owner компоненты Neuron
и DatasetMatrix, фактические per-input параметры. Контекст должен быть свежим
после TakeObject и до Init. Уже owned/initialized/configured controller,
references/search/certificate/observations и незавершённые phases запрещены.
Ранний отказ не меняет существующее owned состояние или физический граф.

Компоненты устанавливаются штатным CommitNativeLearnerChildren с проверкой
preview/readback, порядка и численной допустимости. Native ownership включено
на время commit, чтобы legacy AAddComponent не переписывал импортированный
Dataset. Затем общий adoption валидирует декларацию и mirrors, сбрасывает
probe control и создаёт пустое состояние обучения. Промежуточная холодная
модель не строится: её физика могла бы иметь иные C и допустимость интегратора.

Нет вложенного Calculate, исправляющих per-tick записей или смены lifecycle
bool-контракта. Это не controller checkpoint, не сертификат и не утверждение
о cold-start: физическое состояние могло быть получено обучением, происхождение
фиксирует вызывающий протокол. Ошибка после commit означает Failed и
modelMayHaveChanged=true; такой свежий контекст следует целиком отбросить.

Подготовлены actual storage/Environment fixtures Classic/Branch на2000/20000Гц:
полный derived-image graph и Dataset, source/repeat refusal, сохранность
identity и индивидуальных Rs, пустые references/search/history/certificate.
Отклик после установки этим ещё не проверен; следующий gate — Console probes
полных saved states с отдельным baseline waveform comparison.


Проверенный SDK rootc5a32b9a33ce5d5c76d2299ed44ffd4bd3edcba0 /
Pulse2726bdff6d9ac3bff424429a809ce39d669da10d: M40 normal26/26 native
за128.899с и29/29 readers без skips. Четыре actual prepared-owner records
подтверждают полный source/candidate graph delta, сохранность индивидуальных
Rs86e6/91e6 и пустые references/search/history/certificate/request нового
контроллера. Initialized source и повторная установка отклонены без изменения
графа. Проверены обе топологии и2000/20000Гц; cold substitute не строился.

Все234 members primary859964B и785 source hashes сверены с Git. Группа закрыта,
lease released, семь прежних runtime не изменились. Primary корня
339fc74f5aedf80690a12b91b8ebb3ad8acd9c00; сводка приёмки — корневой
Docs/StructTrain/TimeLearner/PreparedModelInstallNormalVerification.json.
Первая fixture compile failure сохранена отдельно: три temporary UEPtr
выражения в двух instantiations и две cascading QJsonObject diagnostics;
исправлено чтение общего указателя, Rdk не менялся. Native tests в a1 не шли.
Console saved-state probes, candidate waveform, N0/другие режимы, ASan нового
API и второй сервер ещё не проверены; полный этап не закрыт.
