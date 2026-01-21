## NPulseGenerator — генератор импульсов

**Класс**: `NPulseGenerator` (и варианты Multi/Delay) — формирует входные токи/спайки.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPulseGenerator", ...)`, `"NPulseGeneratorMulti"`, `"NPulseGeneratorDelay"`.  
**Storage-инстансы**: `ClassName = "NPulseGenerator*"`; параметры частоты, амплитуды, паттерна, задержек.

```mermaid
classDiagram
    UComponent <|-- NPulseGenerator
    NPulseGenerator <|-- NPulseGeneratorMulti
    NPulseGenerator <|-- NPulseGeneratorDelay
```

### Входы/выходы
- Вход: (опционально) управление частотой/паттерном.
- Выход: последовательность импульсов (спайков) или токов.

```mermaid
flowchart LR
    cfg[Config] --> gen[NPulseGenerator]
    gen --> net[NNet]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant Gen as NPulseGenerator
    participant Net as NNet
    Cfg->>Gen: params (freq, pattern)
    loop timestep
        Gen-->>Net: spike/current
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

---

## NPulseGenerator — pulse generator

Produces spike/current patterns (single, multi-channel, delayed) to drive SNN inputs.
