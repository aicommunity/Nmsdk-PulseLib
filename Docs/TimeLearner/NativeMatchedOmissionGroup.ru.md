# Native группа matched omission

`NativeMatchedOmissionGroup` выполняет FullBefore → omit0…omitN−1 → FullAfter
через штатный `NativeProbeExecutor` и существующий scheduler. BeforeTick только
проверяет границы и ставит запрос; AfterTick принимает закрытую пробу. Вложенных
Calculate нет. Все пробы используют schema6 без изменения старых schemas1–6.

Группа сохраняет исходный target, параметры и выбранную Branch точку. Каждый
контроль завершается штатным восстановлением полного предъявления, затем
проверяются MatrixData/classes и базовая модель. Владение живыми компонентами
проверяется native weak/lifecycle handles и readback полного набора путей.
Равенство semantic hash само по себе не заменяет эту проверку экземпляров.

Каждый trace проходит Arm/Append/CloseNative: production recorder воспроизводит
полную native observation по фактическим callbacks. `CloseNative` отделён от
анализа временных domains; закрытие raw trace не создаёт сертификат.

Число native probes, общий sample budget, model/wall time ограничены. Внешняя
остановка отменяет только пробу; новые CancelNativeProbe методы learner не
вызывают Reset learner и не стирают references/target. Они запрещены, когда
пробой управляет действующий training/recognition runner.

Completed означает завершение всей группы. Assignment имеет собственный
Valid/Missing/Ambiguous/NonRepeatable/etc outcome. Нельзя выдавать Completed за
Exact или классификацию. Новая группа ещё не подключена к CapturedReferences,
BoundedTrainingSession или FinalVerification; их schema3 не ослаблена.
Этот следующий интеграционный шаг требует versioned composite evidence и
проверяемого replay, а не преобразования старого diagnostic packet в сертификат.

Первый набор:35 pure cases,2 capture checks и8 real-native cases на Classic/Branch,
2000/20000 Гц, external stop и физический edit между контролями. На момент
подготовки численные проверки ещё не выполнялись. Старые runtime не обновляются.
