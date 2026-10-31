# Native PostTune candidates

`Core/TimeLearner/NativeRecognitionState` поддерживает необязательный typed
PostTune edit в conversion spec schema 2. Исходные schema-1 преобразования
не меняются. Кандидат всегда выводится из immutable solo и промежуточной
recognition identity; повторное scaling уже изменённых Rs исключено.

Off/KeepDone сохраняют recognition Rs; CanonRmin/FlatLastR используют явные
константы с индивидуальными bounds; SearchSyntheticCandidate хранит один
предложенный вектор. Это не полный SearchSynthetic controller. Candidate
имеет unmeasured/unconfirmed, null recognition certificate. Старый training
certificate сохраняет scope source_solo. Новая модель требует новых проб.

Изменяются только индивидуальные Rs частного native import. Эффективные
границы, совместное владение банками Branch, bindings, численная устойчивость
и полный identity проверяются. Никакого вложенного Calculate, изменения
storage prototype, fallback C или самоприсваиваний по тикам нет.

Рабочая серверная проверка и полный остаток:
`Docs/StructTrain/TimeLearner/NativePostTuneCandidates.ru.md` корневого репозитория.
До публикации execution receipts реализация считается непроверенной. Полный
PostTune driver, commit/rollback и публичный dispatch остаются открытыми.
