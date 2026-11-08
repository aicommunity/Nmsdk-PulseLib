# Private scalar metadata preparation

Механический отдельный шаг перед portable final replay: effectiveModelRevision передаётся в private AcceptValidatedScalar явно из уже проверенного legacy/typed admission. Арифметика residuals, targets, tolerances, порядок запросов, anti-replay и старый wire не изменены. Это позволяет следующему закрытому portable replay использовать общий private scalar path без фиктивных raw observations или live lease. Статический обратный transform воспроизводит f925 FinalVerification.cpp полностью; это не runtime acceptance. Локальных сборок/численных проверок нет.
