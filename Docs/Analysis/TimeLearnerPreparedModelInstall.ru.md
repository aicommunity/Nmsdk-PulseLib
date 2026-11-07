# Установка готового физического графа в свежий learner

Кандидат этапов4/5/6. InstallNativePreparedModel доступен у Classic и Branch;
общая реализация InstallNativePreparedChildren находится в NativeModelOwner.h.
Адресная серверная приёмка ещё не выполнена.

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
