## NPulseNeuronIzhikevich — импульсный нейрон (модель Ижикевича)

**Класс**: `NPulseNeuronIzhikevich` — модель спайкового нейрона по Ижикевичу.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPulseNeuronIzhikevich", ...)`.  
**Storage-инстансы**: `ClassName = "NPulseNeuronIzhikevich"` в ClDesc/Configs, параметры: a, b, c, d, timestep, входные токи.

```mermaid
classDiagram
    NNeuron <|-- NPulseNeuron
    NPulseNeuron <|-- NPulseNeuronIzhikevich
    class NPulseNeuronIzhikevich {
        +a : double
        +b : double
        +c : double
        +d : double
        +v : double
        +u : double
    }
```

### Жизненный цикл
- **ADefault**: задаёт параметры модели (a,b,c,d), сбрасывает v,u.
- **ABuild**: связывает входные токи/каналы.
- **ACalculate**: интегрирует уравнения Ижикевича, генерирует спайк при пороге.

### Входы/выходы (UProperty)
- Вход: ток/сигнал (double), опционально состояния каналов.
- Выход: спайк (bool/int) и обновлённый потенциал.

```mermaid
sequenceDiagram
    participant Cfg as Config
    participant Net as NNet
    participant Neuron as NPulseNeuronIzhikevich
    Cfg->>Net: UploadClass + params(a,b,c,d)
    Net->>Neuron: ADefault + ABuild
    loop timestep
        Net->>Neuron: ACalculate(input current)
        Neuron-->>Net: spike / membrane potential
    end
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    current[Input current] --> neuron[NPulseNeuronIzhikevich]
    neuron --> spike[Spike output]
    neuron --> vstate[Membrane potential v]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

---

## NPulseNeuronIzhikevich — spike neuron (Izhikevich model)

Spike neuron with parameters a,b,c,d; integrates membrane equations and emits spikes. Inputs: current; outputs: spike flag and membrane state.
