# Обзор API Nmsdk-PulseLib

## RU

### Основные классы

#### PulseLibrary

Главный класс библиотеки, наследник `ULibrary`.

#### NPulseNeuron

Базовый импульсный нейрон.

**Основные свойства:**
- `MembranePotential` - мембранный потенциал
- `Threshold` - порог срабатывания
- `OutputSpike` - выходной спайк

#### NPulseNeuronIzhikevich

Нейрон по модели Ижикевича.

**Основные свойства:**
- `A`, `B`, `C`, `D` - параметры модели
- `V` - мембранный потенциал
- `U` - переменная восстановления

#### NPulseSynapse

Базовый синапс.

**Основные свойства:**
- `PreNeuron` - пресинаптический нейрон
- `PostNeuron` - постсинаптический нейрон
- `Weight` - вес синапса

#### NSynapseStdp

Синапс с STDP обучением.

**Основные свойства:**
- `LearningRate` - скорость обучения
- `TauPlus` - временная константа для LTP
- `TauMinus` - временная константа для LTD

#### NSpikeClassifier

Классификатор на основе спайков.

**Основные свойства:**
- `InputNeurons` - входные нейроны
- `OutputClasses` - количество классов
- `ClassLabel` - метка класса

### См. также

- [Reports/06-Nmsdk-PulseLib.md](../../../Reports/06-Nmsdk-PulseLib.md) - детальное описание
- Исходный код: `Libraries/Nmsdk-PulseLib/Core/`

---

## EN

### Main Classes

#### PulseLibrary

Main library class, inherits from `ULibrary`.

#### NPulseNeuron

Base spiking neuron.

**Main Properties:**
- `MembranePotential` - membrane potential
- `Threshold` - firing threshold
- `OutputSpike` - output spike

#### NPulseNeuronIzhikevich

Izhikevich model neuron.

**Main Properties:**
- `A`, `B`, `C`, `D` - model parameters
- `V` - membrane potential
- `U` - recovery variable

#### NPulseSynapse

Base synapse.

**Main Properties:**
- `PreNeuron` - presynaptic neuron
- `PostNeuron` - postsynaptic neuron
- `Weight` - synapse weight

#### NSynapseStdp

Synapse with STDP learning.

**Main Properties:**
- `LearningRate` - learning rate
- `TauPlus` - LTP time constant
- `TauMinus` - LTD time constant

#### NSpikeClassifier

Spike-based classifier.

**Main Properties:**
- `InputNeurons` - input neurons
- `OutputClasses` - number of classes
- `ClassLabel` - class label

### See Also

- [Reports/06-Nmsdk-PulseLib.md](../../../Reports/06-Nmsdk-PulseLib.md) - detailed description
- Source code: `Libraries/Nmsdk-PulseLib/Core/`
