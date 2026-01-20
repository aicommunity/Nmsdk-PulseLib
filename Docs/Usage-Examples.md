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

### Пример 4: Создание сложной импульсной нейросети с обучением

```cpp
// Создание сети из нескольких слоев нейронов
auto network = storage->CreateComponent<NNet>();

// Входной слой
std::vector<NPulseNeuronIzhikevich*> inputLayer;
for (int i = 0; i < 100; i++) {
    auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
    neuron->A = 0.02;
    neuron->B = 0.2;
    neuron->C = -65.0;
    neuron->D = 8.0;
    neuron->Build();
    inputLayer.push_back(neuron);
    network->AddComponent(neuron);
}

// Скрытый слой
std::vector<NPulseNeuronIzhikevich*> hiddenLayer;
for (int i = 0; i < 50; i++) {
    auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
    neuron->A = 0.02;
    neuron->B = 0.2;
    neuron->C = -65.0;
    neuron->D = 8.0;
    neuron->Build();
    hiddenLayer.push_back(neuron);
    network->AddComponent(neuron);
}

// Выходной слой
std::vector<NPulseNeuronIzhikevich*> outputLayer;
for (int i = 0; i < 10; i++) {
    auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
    neuron->A = 0.02;
    neuron->B = 0.2;
    neuron->C = -65.0;
    neuron->D = 8.0;
    neuron->Build();
    outputLayer.push_back(neuron);
    network->AddComponent(neuron);
}

// Создание синапсов с STDP обучением
for (auto* inputNeuron : inputLayer) {
    for (auto* hiddenNeuron : hiddenLayer) {
        auto synapse = storage->CreateComponent<NSynapseStdp>();
        synapse->PreNeuron = inputNeuron;
        synapse->PostNeuron = hiddenNeuron;
        synapse->Weight = 0.5;
        synapse->LearningRate = 0.01;
        synapse->Build();
        network->AddComponent(synapse);
    }
}

for (auto* hiddenNeuron : hiddenLayer) {
    for (auto* outputNeuron : outputLayer) {
        auto synapse = storage->CreateComponent<NSynapseStdp>();
        synapse->PreNeuron = hiddenNeuron;
        synapse->PostNeuron = outputNeuron;
        synapse->Weight = 0.5;
        synapse->LearningRate = 0.01;
        synapse->Build();
        network->AddComponent(synapse);
    }
}

// Обучение сети
network->Build();
for (int epoch = 0; epoch < 100; epoch++) {
    // Подача входных сигналов
    for (int i = 0; i < inputLayer.size(); i++) {
        inputLayer[i]->InputCurrent = inputData[i];
    }
    
    // Расчет сети
    network->Calculate();
    
    // Обновление весов через STDP
    network->Update();
}
```

### Пример 5: Классификация паттернов спайков

```cpp
// Создание классификатора на основе спайков
auto classifier = storage->CreateComponent<NSpikeClassifier>();
classifier->OutputClasses = 5;
classifier->Build();

// Создание входного слоя нейронов для каждого класса
std::vector<std::vector<NPulseNeuronIzhikevich*>> classNeurons(5);
for (int classIdx = 0; classIdx < 5; classIdx++) {
    for (int i = 0; i < 20; i++) {
        auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
        neuron->A = 0.02;
        neuron->B = 0.2;
        neuron->C = -65.0;
        neuron->D = 8.0;
        neuron->Build();
        classNeurons[classIdx].push_back(neuron);
        classifier->InputNeurons.push_back(neuron);
    }
}

// Обучение классификатора на данных
for (const auto& trainingSample : trainingData) {
    // Подача входных данных
    for (int i = 0; i < trainingSample.input.size(); i++) {
        classNeurons[trainingSample.label][i]->InputCurrent = trainingSample.input[i];
    }
    
    // Расчет и классификация
    classifier->Calculate();
    
    // Проверка результата
    if (classifier->ClassLabel != trainingSample.label) {
        // Корректировка весов
        classifier->UpdateWeights(trainingSample.label);
    }
}
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

### Example 4: Creating Complex Spiking Neural Network with Learning

```cpp
// Creating network with multiple neuron layers
auto network = storage->CreateComponent<NNet>();

// Input layer
std::vector<NPulseNeuronIzhikevich*> inputLayer;
for (int i = 0; i < 100; i++) {
    auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
    neuron->A = 0.02;
    neuron->B = 0.2;
    neuron->C = -65.0;
    neuron->D = 8.0;
    neuron->Build();
    inputLayer.push_back(neuron);
    network->AddComponent(neuron);
}

// Hidden layer
std::vector<NPulseNeuronIzhikevich*> hiddenLayer;
for (int i = 0; i < 50; i++) {
    auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
    neuron->A = 0.02;
    neuron->B = 0.2;
    neuron->C = -65.0;
    neuron->D = 8.0;
    neuron->Build();
    hiddenLayer.push_back(neuron);
    network->AddComponent(neuron);
}

// Output layer
std::vector<NPulseNeuronIzhikevich*> outputLayer;
for (int i = 0; i < 10; i++) {
    auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
    neuron->A = 0.02;
    neuron->B = 0.2;
    neuron->C = -65.0;
    neuron->D = 8.0;
    neuron->Build();
    outputLayer.push_back(neuron);
    network->AddComponent(neuron);
}

// Creating synapses with STDP learning
for (auto* inputNeuron : inputLayer) {
    for (auto* hiddenNeuron : hiddenLayer) {
        auto synapse = storage->CreateComponent<NSynapseStdp>();
        synapse->PreNeuron = inputNeuron;
        synapse->PostNeuron = hiddenNeuron;
        synapse->Weight = 0.5;
        synapse->LearningRate = 0.01;
        synapse->Build();
        network->AddComponent(synapse);
    }
}

for (auto* hiddenNeuron : hiddenLayer) {
    for (auto* outputNeuron : outputLayer) {
        auto synapse = storage->CreateComponent<NSynapseStdp>();
        synapse->PreNeuron = hiddenNeuron;
        synapse->PostNeuron = outputNeuron;
        synapse->Weight = 0.5;
        synapse->LearningRate = 0.01;
        synapse->Build();
        network->AddComponent(synapse);
    }
}

// Training network
network->Build();
for (int epoch = 0; epoch < 100; epoch++) {
    // Feeding input signals
    for (int i = 0; i < inputLayer.size(); i++) {
        inputLayer[i]->InputCurrent = inputData[i];
    }
    
    // Network calculation
    network->Calculate();
    
    // Updating weights via STDP
    network->Update();
}
```

### Example 5: Spike Pattern Classification

```cpp
// Creating spike-based classifier
auto classifier = storage->CreateComponent<NSpikeClassifier>();
classifier->OutputClasses = 5;
classifier->Build();

// Creating input layer neurons for each class
std::vector<std::vector<NPulseNeuronIzhikevich*>> classNeurons(5);
for (int classIdx = 0; classIdx < 5; classIdx++) {
    for (int i = 0; i < 20; i++) {
        auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
        neuron->A = 0.02;
        neuron->B = 0.2;
        neuron->C = -65.0;
        neuron->D = 8.0;
        neuron->Build();
        classNeurons[classIdx].push_back(neuron);
        classifier->InputNeurons.push_back(neuron);
    }
}

// Training classifier on data
for (const auto& trainingSample : trainingData) {
    // Feeding input data
    for (int i = 0; i < trainingSample.input.size(); i++) {
        classNeurons[trainingSample.label][i]->InputCurrent = trainingSample.input[i];
    }
    
    // Calculation and classification
    classifier->Calculate();
    
    // Checking result
    if (classifier->ClassLabel != trainingSample.label) {
        // Adjusting weights
        classifier->UpdateWeights(trainingSample.label);
    }
}
```
