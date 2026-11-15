# Проверка XML уже зарегистрированных образов

`PrepareModelArchive` проверяет XML каждой записи class_definitions через
штатные `LoadComponent`, Init/Reset и сравнение эффективной модели
`LoadSnapshot`. Наличие совпадающего образа в storage не заменяет эту проверку.
Для существующего образа также сохраняются проверки kind, base_class и
семантического identity; prototype не изменяется. Новый образ регистрируется
только после успешного native readback.

На PulseLib `60802ab962e1ae8bf091c331f2a2b02642fc9437` две адресные проверки
`TimeLearnerModelArchive/RejectsContradictoryNativeImageXmlWithExistingClass`
и `RejectsContradictoryDerivedImageXmlWithExistingClass` воспроизвели приём
архива с изменённой XML Capacity при неизменённом declared identity.
Контрольные суммы XML и архива были пересчитаны корректно. Дефект затрагивает
проверку содержания, а не обнаружение повреждения checksum.

Кандидат исправления убирает пропуск native readback для существующих
классов. Математическая цель обучения, физика образов и алгоритмы learner
не меняются. Динамические буферы образа не превращаются в checkpoint.

На M40 SDKf39/Pulse991 оба отрицательных контроля прошли; новая группа
Load/XML8 завершилась8/8. Также прошли membrane stages, semantic saved-model
replay и оба fresh-process waveform transfers при2000/20000Hz: полные
actual input/soma окна, max_soma_pairwise_error=0. Это адресное runtime
подтверждение исправления, не приёмка всех образов и физических профилей.

Полный17-case gate не пройден: отдельная Packages5 группа остановлена kernel
OOM при4GiB во время третьего C0 случая, итогового JUnit у неё нет. Два OK
в её stdout не считаются завершённой package-приёмкой. Требуются isolated C0,
полный package/режимный matrix и проверка на Elder; лимит памяти не повышался.

Полные414 originals и выбранные временные графики опубликованы в root
evidence219ceb0c439ef91ce410cf564f9a1dc9f2f63364. Проверены4074 compiled-source
hashes, outcomes, closure и released leases. [Отчёт и точные ограничения](https://github.com/aicommunity/Nmsdk/blob/3fe3fa0105074df4eef0c07ee0e07fb252e92f0c/Docs/StructTrain/TimeLearner/NativeArchiveRepair17Oom-20261110.ru.md).
Изменение документации не меняет измеренный Pulse991 commit и frozen runtime.
