# Примеры использования Nmsdk-PulseLib

## RU

### С чего начать

Если вы новичок в работе с Nmsdk-PulseLib, рекомендуем начать с следующих готовых экспериментов:

**Актуальные примеры (SpikeSamples):**

1. **Простой STDP-эксперимент**: `Bin/Configs/SpikeSamples/STDP/STDP-Simple-01/`
   - Демонстрирует базовый механизм STDP-обучения между двумя нейронами
   - Простая структура для понимания основ

2. **Обучение структур / тренер**: `Bin/Configs/SpikeSamples/StructTrain/SpikeAnsTrainer/`
   - Примеры обучения с тренерами спайковых ответов

3. **Модели нейронов**: `Bin/Configs/SpikeSamples/NM-Neurons/` (LIF-Neuron, CableModel, NeuronComparation и др.)
   - Различные конфигурации нейронов и сравнение моделей

4. **Мышцы**: `Bin/Configs/SpikeSamples/MC-Muscles/MC-M-01-EyeMuscle/`
   - Пример с NEyeMuscle и управлением движением

5. **Классификация**: `Bin/Configs/SpikeSamples/Classifier/SpikeIrisClassifier/`
   - Пример классификации паттернов спайков

**Дополнительно (OldConfigs/User):** `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`, `Bin/Configs/!OldConfigs/SpikeClassifier/`

Эти проекты можно запустить напрямую в Nmsdk Engine и модифицировать параметры для экспериментов.

### Примеры по типам задач

#### Базовый нейрон/синапс/STDP

##### Пример 1: Создание импульсного нейрона

```cpp
// Создание нейрона по модели Ижикевича
auto neuron = storage->CreateComponent<NPulseNeuronIzhikevich>();
neuron->A = 0.02;
neuron->B = 0.2;
neuron->C = -65.0;
neuron->D = 8.0;
neuron->Build();
```

**Соответствующая конфигурация:** `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`

В конфигурационном проекте нейрон Ижикевича настраивается через XML:

```xml
<Neuron1 Class="NPulseNeuronIzhikevich">
    <Parameters>
        <A>0.02</A>
        <B>0.2</B>
        <C>-65.0</C>
        <D>8.0</D>
    </Parameters>
</Neuron1>
```

Параметры A, B, C, D соответствуют свойствам класса `NPulseNeuronIzhikevich` в коде.

##### Пример 2: Создание синапса с STDP

```cpp
// Создание синапса с обучением STDP
auto synapse = storage->CreateComponent<NSynapseStdp>();
synapse->PreNeuron = preNeuron;
synapse->PostNeuron = postNeuron;
synapse->Weight = 0.5;
synapse->LearningRate = 0.01;
synapse->Build();
```

**Соответствующая конфигурация:** `Bin/Configs/SpikeSamples/STDP/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/STDP-Simple-01/`

В конфигурации STDP-синапс настраивается так:

```xml
<Synapse1 Class="NSynapseStdp">
    <Parameters>
        <PreNeuron>Neuron1</PreNeuron>
        <PostNeuron>Neuron2</PostNeuron>
        <Weight>0.5</Weight>
        <LearningRate>0.01</LearningRate>
        <TauPlus>0.02</TauPlus>
        <TauMinus>0.02</TauMinus>
    </Parameters>
</Synapse1>
```

Эти параметры можно изменять в `Parameters_*.xml` для экспериментов с различными скоростями обучения.

#### Классификация паттернов спайков

##### Пример 3: Классификация на основе спайков

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

**Соответствующая конфигурация:** `Bin/Configs/!OldConfigs/SpikeANPA3/`, `Bin/Configs/!OldConfigs/SpikeClassifier/`

В конфигурационных проектах классификатор обычно используется вместе с входным слоем нейронов:

```xml
<InputLayer Class="NNeuronsLayer">
    <Parameters>
        <NeuronsClassName>NPulseNeuronIzhikevich</NeuronsClassName>
        <LayerWidth>20</LayerWidth>
    </Parameters>
</InputLayer>

<Classifier Class="NSpikeClassifier">
    <Parameters>
        <InputNeurons>InputLayer.Neurons</InputNeurons>
        <OutputClasses>5</OutputClasses>
    </Parameters>
</Classifier>
```

Входные данные подаются на нейроны входного слоя, которые кодируют их в паттерны спайков для классификатора.

#### Управление движением/мышцами

##### Пример 4: Создание сложной импульсной нейросети с обучением

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

**Соответствующая конфигурация:** `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/OldExperiments/SimplestMotionControl/`

В конфигурациях для управления движением используются:

- **Моторные нейроны** (`NMotoneuron`): преобразуют сигналы от других нейронов в команды для мышц
- **Мышцы** (`NMuscle`, `NEyeMuscle`): эффекторы, выполняющие движение
- **Афферентные нейроны** (`NAfferentNeuron`): получают сенсорную информацию

Пример структуры:
```xml
<ControlNeuron Class="NPulseNeuronIzhikevich">
    <!-- Параметры управляющего нейрона -->
</ControlNeuron>

<Motoneuron1 Class="NMotoneuron">
    <!-- Параметры моторного нейрона -->
</Motoneuron1>

<Muscle1 Class="NMuscle">
    <!-- Параметры мышцы -->
</Muscle1>
```

Связи: `ControlNeuron.Output` → `Motoneuron1.Input` → `Muscle1.Input`

#### Когнитивная навигация/пространственная память

##### Пример 5: Классификация паттернов спайков

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

**Соответствующая конфигурация:** `Bin/Configs/User/CognitiveNavigation/Ivan_VKR_3-4_13/`

Этот проект демонстрирует сложную сеть для когнитивной навигации с:

- **Логическими операциями**: NOT, AND, OR через комбинации нейронов
- **Пространственными сигналами**: Forward, Back, Left, Right, LeftDanger, RightDanger
- **Биоинспирированными компонентами**: `NSPNeuronGen`, `NPMembraneBio`, `NPExcChannelBio`, `NPInhChannelBio`
- **LT-зонами**: для формирования долгосрочной памяти о пространственных паттернах

Ключевые особенности:
- Нейроны с иерархической структурой (сома, дендриты, LT-зоны)
- Обратные связи для поддержания состояния
- Генераторы входных сигналов (`NPulseGeneratorTransit`) для различных направлений движения

#### Анализ временных рядов/предсказание

**Соответствующая конфигурация:** `Bin/Configs/!OldConfigs/OldExperiments/TimeSeriesTest/`, `Bin/Configs/!OldConfigs/TSNETest/`

Для работы с временными рядами используются:

- **Генераторы из файла** (`NFileGenerator`): загрузка данных из файлов
- **Предсказатели** (`NPredictor`, `NStatePredictor`): предсказание следующих значений на основе истории
- **Экстраполяторы движения** (`NMExtrapolator`): предсказание траектории движения

Пример использования предсказателя:
```cpp
// Создание предсказателя
auto predictor = storage->CreateComponent<NPredictor>();
predictor->InputNeurons = historyNeurons;
predictor->Build();

// Предсказание следующего состояния
predictor->Calculate();
auto predictedValue = predictor->PredictedOutput;
```

### Сопоставление кода и конфигураций

При работе с конфигурационными проектами важно понимать соответствие между кодом C++ и XML-описаниями:

| C++ код | XML конфигурация |
|---------|------------------|
| `neuron->A = 0.02;` | `<A>0.02</A>` в `Parameters_*.xml` |
| `synapse->Weight = 0.5;` | `<Weight>0.5</Weight>` |
| `synapse->PreNeuron = preNeuron;` | `<PreNeuron>Neuron1</PreNeuron>` (ссылка по имени) |
| `classifier->OutputClasses = 5;` | `<OutputClasses>5</OutputClasses>` |
| `network->AddComponent(neuron);` | Компонент добавляется в `<Components>` секцию `Model_*.xml` |
| Связь через `ULink` | Элемент в секции `<Links>` с `<Item>` и `<Connector>` |

### Рекомендации по модификации конфигураций

1. **Изменение параметров**: Редактируйте `Parameters_*.xml`, сохраняя структуру `Model_*.xml`
2. **Добавление компонентов**: Добавьте новый компонент в `<Components>` и создайте связи в `<Links>`
3. **Эксперименты**: Создавайте новые версии (`Model_01.xml`, `Parameters_01.xml`) для сравнения
4. **Визуализация**: Настройте графики в `Interface.xml` для наблюдения за интересующими величинами

### См. также

- [Config-Overview.md](Config-Overview.md) - подробное описание структуры конфигураций
- [Config-Templates.md](Config-Templates.md) - шаблоны типовых экспериментов
- [API-Overview.md](API-Overview.md) - описание API компонентов

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
