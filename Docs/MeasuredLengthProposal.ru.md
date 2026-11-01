# Предложение длины по измеренной временной ошибке

Состояние: feature implementation, серверная приёмка не завершена. Это изменение
порядка кандидатов общего V2 BoundedTrainingSession; математическая цель,
CapturedPerInput, допуски и штатный Rdk scheduler не меняются.

После завершения полного кандидата (включая обязательные повторы N0) контроллер
сохраняет signed timing interval и полный вектор параметров. Сравниваются только
измерения активного входа с теми же Ns/Rs и всеми прочими параметрами. При смене
активного входа/начала sweep эта история очищается. Каждый исходный sample
остаётся связан со своими trial/model revision; прежний пик не подтверждает
новый граф. Shared Branch изменения остальных attachments не смешиваются
с неизменной геометрией.

Минимум три различные измеренные длины определяют наблюдаемый тренд. Повторы
с несовместимыми интервалами, немонотонность и неопределённый знак не создают
прогноз. При измеренном bracket между соседними длинами предлагается внутреннее
целое среднее; без bracket односторонний тренд предлагает ограниченную целую
оценку нуля по секущей. Предсказание не доказывает глобальную монотонность,
существование пика, физическую достижимость или синхронность.

```text
consume complete valid candidate, after all required repetitions
retain timing interval and full parameters with original trial/revision
select same-input, same-Ns/Rs, otherwise-identical geometry measurements
require three distinct measured lengths and consistent monotone trend
propose legal unmeasured integer (bracket midpoint or bounded secant estimate)
prioritize next length without increasing candidate/probe/edit/time budgets
N1: finish current geometry, then assess proposed native physical domain
apply native edit, measure fresh anchor/input, then fresh all-input final gate
```

Уже использованная длина не вставляется повторно перед текущей позицией;
оставшийся конечный перебор сохраняется. N0 full-vector dedup и обязательные
повторы остаются действующими. N1 предложение не обходит native domain action,
интегратор, bounds или полный effective readback. Восстановление best не
порождает новый поиск. Missing/ambiguous/incomplete peaks сюда не передаются:
общий validator отклоняет их до proposal policy.

Приёмка требует исходного отрицательного контроля, production helper/controller
checks normal/ASan, реальных native burst/edit/domain regressions, cold model
experiments с полными наблюдениями и читаемыми временными отчётами. До этого
feature не является develop. Начальные EstDelayPerSeg/gamma остаются отдельным
вопросом явной V2 конфигурации; этот измеренный тренд не делает legacy property
действующим фактором V2 и не закрывает полный этап7.
