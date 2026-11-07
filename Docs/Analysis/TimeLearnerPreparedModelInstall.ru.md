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
Отклик после установки этими fixtures не проверяется; отдельный Console
saved-state/baseline waveform gate описан ниже.


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
N0/другие режимы, ASan нового API и второй сервер остаются открытыми;
полный этап не закрыт. Console scope отдельно подтверждён ниже.


## Console saved-state gate и waveform proof

Корневой compiled SDK844b2789ea0c087c1a0f676d9584ce7b8a7a4d36 /
Pulse1f97e4facc5bf03887bc337450190cc7b24c3ebf прошёл M40 normal26 native
controls и29 прежних readers. Console job5 установил полный imported physical
state в fresh owner, не копируя references/history/certificate. Выполнены12
проб: Classic/Branch × burst/single × original/baseline/FeedbackGain0. Все4
baseline pairs дали побитовое совпадение input/soma waveforms; только после
них разрешены gain0 trials. В исходных archived states4 физических входа,
2000Гц, threshold0.0117 и нет дополнительной LTMembrane. N2 fixtures выше
проверяют2000/20000Гц; это не расширяет actual stock scope на20000Гц.

Primary корня fea18ada6b6446ac422fb728d119ec06bc257ae5:1555 members/789 source
hashes сверены. Initial suite11 сохранил1 test-only array/object loader error
после аудита всех12 executions. Исправленные2 readers прошли отдельно на
неизменных данных, без native repeat; evidence2127a534ac5272031b14ea612020cd2628572ec7.
Связанные9 admission controls прошли. Original failed terminal не переписан.
Восемь прежних runtime не изменились, группы закрыты/leases released.

Корневые maintained receipts: Docs/StructTrain/TimeLearner/
FeedbackProfileConsoleNormalVerification.json иFeedbackProfileConsoleReaderVerification.json.
Научный отчёт FeedbackProfileScientificReport-2026-11-07.ru.md содержит6
индивидуально просмотренных PNG; полный cache остаётся на M40. Gain0 изменяет
не только число максимумов, но время/амплитуду/LT Output; это не разрешение
поменять default physics или математическую цель обучения. Получение training
certificate и классификация этим gate не проверялись. Recognition/PostTune
state, C0/controller checkpoint и остальные import modes требуют своих gates.

## Второй сервер: Elder, normal SDK

Root `ac9ccc607dece61c0ade1900133654e05f10e62a` / PulseLib
`8702d264bc7b4f4c464e200f4a1092e55c6b44ff` собраны на Elder в отдельный
runtime `runtime-feedback-storage-elder-normal-a1`. Native набор 163/163 без
skips включает те же 26 storage/single-pulse/import/prepared-owner controls
и заранее объявленные 137 search/verification/transport regressions.
Прошли также 19 fixture readers. Первичные данные опубликованы в корневом
evidence `2d8c76cf319e49d71fc09af69e93acd3b68fdc87`; сверены 1018 members,
493 root и 265 PulseLib source hashes, все JUnit IDs, actual loaded files и
шесть generated compiler/link command files. Прежние 59 runtime bindings
не изменились; whole worker group закрыта, lease освобождена после closure.

Это Release, не ASan. Console на Elder только собрана: M40 Console12 не
объявляются повторёнными на втором сервере. Старый single-pulse ASan37
предшествует новому prepared API; его доказательства не переносятся на этот
API автоматически. Адресный ASan нового API проверяется отдельно.
Maintained корневые отчёт и receipt: `Docs/StructTrain/TimeLearner/`
`FeedbackStorageElder-2026-11-07.ru.md`, `FeedbackStorageElderVerification.json`.
Ни классификация, ни полная physics/modes матрица данным gate не принимаются.
