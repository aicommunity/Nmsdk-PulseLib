# Примеры использования Nmsdk-PulseLib

## RU

### Пример 1: Создание импульсного нейрона

```cpp
// Создание нейрона по модели Ижикевича
auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
neuron->A = 0.02;
neuron->B = 0.2;
neuron->C = -65.0;
neuron->D = 8.0;
neuron->Build();
```

### Пример 2: Создание синапса с STDP

```cpp
// Создание синапса с обучением STDP
auto synapse = storage->CreateComponent<NSynapseStdp>();
synapse->PreNeuron = preNeuron;
synapse->PostNeuron = postNeuron;
synapse->Weight = 0.5;
synapse->LearningRate = 0.01;
synapse->Build();
```

### Пример 3: Классификация на основе спайков

```cpp
// Создание классификатора
auto classifier = storage->CreateComponent<NSpikeClassifier>();
classifier->InputNeurons = inputLayer;
classifier->OutputClasses = numClasses;
classifier->Build();

// Классификация
classifier->Calculate();
auto classLabel = classifier->ClassLabel;
```

---

## EN

### Example 1: Creating Spiking Neuron

```cpp
// Creating Izhikevich model neuron
auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
neuron->A = 0.02;
neuron->B = 0.2;
neuron->C = -65.0;
neuron->D = 8.0;
neuron->Build();
```

### Example 2: Creating STDP Synapse

```cpp
// Creating synapse with STDP learning
auto synapse = storage->CreateComponent<NSynapseStdp>();
synapse->PreNeuron = preNeuron;
synapse->PostNeuron = postNeuron;
synapse->Weight = 0.5;
synapse->LearningRate = 0.01;
synapse->Build();
```

### Example 3: Spike-Based Classification

```cpp
// Creating classifier
auto classifier = storage->CreateComponent<NSpikeClassifier>();
classifier->InputNeurons = inputLayer;
classifier->OutputClasses = numClasses;
classifier->Build();

// Classification
classifier->Calculate();
auto classLabel = classifier->ClassLabel;
```
