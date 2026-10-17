# Варианты нейрона для временных паттернов

## RU

Это регистрации конфигурированных NPulseNeuron, а не отдельные C++ классы. Источник: [NPulseLibrary.cpp](../../Core/NPulseLibrary.cpp), блоки около строк 1190–1397. Применение параметров: [NPulseNeuron::ApplyElementDefaults](../../Core/NPulseNeuron.cpp). Значения ниже — явные настройки регистраций; XML может их переопределить.

| ClassName | Capacity, Ф | ExcRm, Ом | ExcRsyn, Ом |
|-----------|-------------|-----------|------------|
| NSPNeuronGenAsymRmD001C25e12 | 2.5e-11 | 1e7 | 8.6e7 |
| NSPNeuronGenAsymRmD001C25e11 | 2.5e-10 | 1e7 | 8.6e7 |
| NSPNeuronGenAsymRmD001C1e9 | 1e-9 | 1e7 | 8.6e7 |
| NSPNeuronGenPreinh2_5AsymRmD001C1e9 | 1e-9 | 1e7 | 8.6e7 |
| NSPNeuronGenPreinh2_5AsymRmD001C25e12 | 2.5e-11 | 1e7 | 8.6e7 |
| NSPNeuronGenAsymRmD001C1e11 | 1e-11 | 1e7 | 5e7 |
| NSPNeuronGenPreinh2_5AsymRmD001C1e11 | 1e-11 | 1e7 | 5e7 |
| NSPNeuronGenAsymRmD001C25e12Rm5e6 | 2.5e-11 | 5e6 | 8.6e7 |
| NSPNeuronGenPreinh2_5AsymRmD001C25e12Rm5e6 | 2.5e-11 | 5e6 | 8.6e7 |

У всех AsymRm выше: UseElementDefaults=true, DissociationTC=SecretionTC=0.001 с, InhRm=10×ExcRm, InhRsyn=10×ExcRsyn, LTZoneClassName=NPulseLTZoneThreshold, Threshold=0.0117. Preinh2_5 использует NPMembraneBioPreinh2_5, остальные NPMembraneBio. Вызов ApplyElementDefaults явный.

Отдельный NSPNeuronGenPreinh2_5D002C25e11 задаёт MembraneCapacity=2.5e-10 и SynapseDissociationTC=0.002 после первого Build, затем вызывает второй Build. В текущем lifecycle Build возвращается сразу при Ready, а setters этих параметров Ready не сбрасывают. Поэтому немедленное применение к уже созданным дочерним элементам этим кодом не обеспечено; перед экспериментом проверять фактические параметры каналов/синапсов. [Дефект и план проверки](../../../../Docs/Audit/TimeLearner-2026-09-22/README.md).

## EN

These names register configured NPulseNeuron prototypes. The table lists explicit registration values, not values inferred from name suffixes; XML may override them. All AsymRm variants explicitly apply element defaults. The separate Preinh2_5D002C25e11 registration changes defaults after Build and calls Build again while Ready; it does not explicitly propagate them to existing children. Verify child parameters before using this variant for comparisons.
