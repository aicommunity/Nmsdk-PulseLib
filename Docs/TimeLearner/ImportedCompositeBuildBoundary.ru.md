# Native Build после импорта composite state

После `ImportNativeLearnedState` learner имеет `Ready=false`: принятый граф
должен пройти обычные `Build/Init` перед запросом native probe. Прямой вызов
probe до этой границы запрещён readiness guard. Возвращаемый bool Build остаётся
интерфейсным артефактом; проверяются Ready, Init и специализированная диагностика.

Обнаруженная ошибка: `ABeforeBuild` обоих TimeLearner безусловно вызывал
`ResetControl` для protocol4/5. Поэтому штатный Build после корректного импорта
терял Finished/certificate/outcome и переводил контроллер в Idle. На M40 это
воспроизведено в `NativeRecoveryPackage7/ClassicN0StoppedRoundtripKeepsResponse`
после добавления обычной post-import инициализации.

Следующее исправление сохраняет только **detached composite boundary**:

```text
protocol is 4 or 5
state is Finished; session attempt is positive
no live composite executor; no search traversal
original composite configuration is retained
```

Это внутреннее производное состояние `HasDetachedCompositeBoundary`, не флаг
из JSON и не новая authority. Build по-прежнему инвалидирует owner-control
lifetime и отменяет raw probe. Для обычного активного composite сохраняются
прежние interruption/reset semantics. Явный Reset learner по-прежнему переводит
контроллер в Idle; вернуть необученную физическую структуру можно только
отдельной операцией.

Сохранение boundary не подтверждает, что модель не изменена: actual revision,
полный graph readback и certificate binding проверяются отдельно. Повторный
export не должен переносить старую Exact на другой physical model.

Root tests усиливают package7 roundtrip проверками detached state до/после
Build, физического revision, certificate/ledger и raw response. Warm cases
проверяют, что explicit Reset всё же очищает transient state. Отдельные четыре
real-component cases проверяют active Build для Classic/Branch и protocol4/5.
Изменение пока не принято compiler/runtime проверками; прежние результаты
Pulse63d7a62 не доказывают правильность этого исправления.

Исторический `NativeCandidateWindowPackage6.ru.md` описывает свой исходный
снимок. Актуальные независимые readers и межмодульная интеграция учитываются
в корневых `ImplementationStatus.ru.md` и `ExperimentReadiness.ru.md`.
