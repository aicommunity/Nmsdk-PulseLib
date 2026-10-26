# Полная последовательность волн: внутренняя schema 3

ProbeSpec schema 3 использует OrderedWholeBurst и typed stimulus profile. На
каждом измеряемом канале число полностью наблюдаемых положительных максимумов
должно соответствовать всей пачке. Порядок, реальные stimulus timestamps и
измеренные plateau times связывают выбранный peak с его stimulus ID. Missing,
extra, clipped либо неподходящее окно запрещают принятие; model delay не
восстанавливает отсутствующий peak. Полная спецификация, исходные counterexamples
и проверенные версии: [BurstAssignment](../../../../Docs/StructTrain/TimeLearner/BurstAssignment.ru.md).

Для schema 3 input order совпадает с chronological stimulus order. Classic input
i принадлежит soma channel i; baseline имеет размерность N. Branch использует
одну сому/channel 0 и native prepared selectedInputIndex. Verifier проверяет
фактический selector; NativeProbeExecutor отклоняет неверные channel, selector
или число ожидаемых стимулов до предъявления. VerificationRequest.inputIndex
передаёт индекс действия без необходимости извлекать его из текста input ID.

Schemas 1/2 сохраняют WindowOnly для прежних критериев. Их observations нельзя
объявлять полной burst assignment задним числом. Default NativeProbeRequest
пока остаётся schema 2; новый путь выбирается явно. Configured ControllerVersion=2
и основной train/search loop этим API не включаются.

Order-preserving one-wave-per-stimulus — явное предположение протокола. Равное
число максимумов не доказывает причину для любой нелинейной/осциллирующей физики.
Широкая physics/inhibition applicability требует отдельных проверок до включения
нового контроллера. На проверенных frozen fixtures covered N=1/2/4/6; это не
научный результат распознавания и не отмена исторических августовских 4/8–6/8.
