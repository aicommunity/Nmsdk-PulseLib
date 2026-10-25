# Native Copy/XML и порядок сохранённого графа

В NPulseNeuron режим StructureBuildMode=0 означает явный сохранённый граф.
UpdateComputationOrder в этом режиме сохраняет порядок именованных детей.
Режимы 1/2 продолжают устанавливать порядок distal dendrites → soma → LT.
Это свойство вложенного нейрона; его нельзя смешивать с режимами learner 0/1.

До исправления mode 0 пропускал перестановку дендритов, но передвигал Soma/LT
в начало. XML Load с последующим native Init/Build тем самым менял порядок
расчёта при том же множестве property bindings и индивидуальных C/Rs. Native
Copy в проверенном сценарии сохранял порядок; универсальный дефект Copy
из этого наблюдения не следует.

Корневой fixture Test_PulseLib_ModelRoundtrip.cpp проверяет несимметричный
NPulseNeuron с двумя soma и длинами {1,2}, индивидуальными C и Rs, полным графом
(UseElementDefaults=false), native Copy и SaveComponent/LoadComponent. Сравнивает
граф, внутренние ссылки, частные параметры и ненулевой импульсный отклик через
общий scheduler после штатного Reset. Это не checkpoint динамики посреди пробы.

UseElementDefaults=true и рост структуры с сохранением обученных поправок
требуют отдельного контракта storage/learned state. Этот тест явно отключает
равномерное повторное применение parent defaults и не доказывает сохранность
поправок при таком legacy режиме. Версия 2 learner пока не реализована.
