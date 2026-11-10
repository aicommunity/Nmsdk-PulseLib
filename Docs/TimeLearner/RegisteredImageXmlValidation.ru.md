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

Серверная проверка исправления пока не выполнена. Необходимы оба отрицательных
контроля, положительные model/package roundtrip и перенос в свежий процесс.
