# FAQ - Nmsdk-PulseLib

## RU

### Общие вопросы

#### Что такое Nmsdk-PulseLib?

Nmsdk-PulseLib - это библиотека компонентов для моделирования импульсных нейронных сетей (Spiking Neural Networks, SNN). Она включает различные модели нейронов, синапсов, каналов, механизмы обучения и классификации.

#### Какие модели нейронов поддерживаются?

Библиотека поддерживает множество моделей:
- **Integrate-and-Fire (IaF)** - простая модель
- **Izhikevich** - биологически правдоподобная модель
- **Биоинспирированные модели** - более сложные модели с каналами
- **Hebb** - модели с правилом Хебба

См. [Component-Catalog.md](Component-Catalog.md) для полного списка.

#### Как выбрать подходящую модель нейрона?

Выбор зависит от задачи:
- **IaF** - для простых экспериментов и обучения
- **Izhikevich** - для более реалистичного моделирования
- **Биоинспирированные** - для детального моделирования биологических процессов
- **Hebb** - для обучения с правилом Хебба

### Компоненты

#### Как создать простую импульсную сеть?

См. [Getting-Started.md](Getting-Started.md) или [Config-Templates.md](Config-Templates.md) для примеров.

**Краткий ответ:**
1. Создайте генератор импульсов (`NPulseGenerator`)
2. Создайте нейроны (`NPulseNeuron` или другие)
3. Создайте синапсы (`NPulseSynapse`) для соединений
4. Настройте параметры и запустите выполнение

#### Как настроить STDP обучение?

Используйте компоненты `NSynapseStdp` или `NPulseSynapseStdp`:
1. Создайте синапс с STDP
2. Настройте параметры STDP (A+, A-, tau+)
3. Подключите к нейронам
4. Запустите обучение

См. [Usage-Examples.md](Usage-Examples.md) для детальных примеров.

#### Как создать классификатор на основе импульсных сетей?

Используйте компоненты `NSpikeClassifier` или `NPCAClassifier`:
1. Создайте сеть нейронов
2. Подключите классификатор к выходу сети
3. Настройте обучение
4. Обучите на данных

### Конфигурации

#### Где найти примеры конфигураций?

Примеры конфигураций находятся в:
- `Bin/Configs/User/CognitiveNavigation/` - когнитивная навигация
- `Bin/Configs/!OldConfigs/NM-Neurons/` - модели нейронов
- `Bin/Configs/!OldConfigs/STDP-Simple-01/` - эксперименты по STDP

См. [Config-Overview.md](Config-Overview.md) для детального описания.

#### Как использовать шаблоны конфигураций?

См. [Config-Templates.md](Config-Templates.md) - там описаны шаблоны типовых экспериментов.

### Производительность

#### Как оптимизировать производительность импульсных сетей?

- Используйте более простые модели (IaF вместо Izhikevich) где возможно
- Минимизируйте количество нейронов в сети
- Используйте кэширование результатов
- Избегайте избыточного логирования

### Интеграция

#### Как интегрировать импульсные сети с другими библиотеками?

- Используйте компоненты из Rdk-BasicLib для ввода-вывода данных
- Используйте Nmsdk-MotionControlLib для управления движением
- См. [Usage-Examples.md](Usage-Examples.md) для примеров интеграции

### Устранение неполадок

#### Нейрон не генерирует импульсы

- Проверьте входные импульсы (подключены ли они)
- Проверьте параметры нейрона (порог, мембранный потенциал)
- Проверьте готовность компонента (`IsReady()`)

#### STDP не работает

- Убедитесь, что синапс правильно подключен
- Проверьте параметры STDP
- Убедитесь, что нейроны генерируют импульсы

### Дополнительные ресурсы

- [Architecture.md](Architecture.md) - архитектура библиотеки
- [Component-Catalog.md](Component-Catalog.md) - каталог компонентов
- [Scientific-Background.md](Scientific-Background.md) - научный фон
- [Docs/Troubleshooting/Troubleshooting-Guide.md](../../../Docs/Troubleshooting/Troubleshooting-Guide.md) - общее руководство по устранению неполадок

---

## EN

### General Questions

#### What is Nmsdk-PulseLib?

Nmsdk-PulseLib is a component library for modeling Spiking Neural Networks (SNN). It includes various neuron models, synapses, channels, learning mechanisms, and classification.

#### What neuron models are supported?

The library supports many models:
- **Integrate-and-Fire (IaF)** - simple model
- **Izhikevich** - biologically plausible model
- **Bio-inspired models** - more complex models with channels
- **Hebb** - models with Hebb's rule

See [Component-Catalog.md](Component-Catalog.md) for complete list.

#### How do I choose the right neuron model?

Choice depends on the task:
- **IaF** - for simple experiments and training
- **Izhikevich** - for more realistic modeling
- **Bio-inspired** - for detailed biological process modeling
- **Hebb** - for learning with Hebb's rule

### Components

#### How do I create a simple spiking network?

See [Getting-Started.md](Getting-Started.md) or [Config-Templates.md](Config-Templates.md) for examples.

**Quick answer:**
1. Create a pulse generator (`NPulseGenerator`)
2. Create neurons (`NPulseNeuron` or others)
3. Create synapses (`NPulseSynapse`) for connections
4. Configure parameters and start execution

#### How do I configure STDP learning?

Use `NSynapseStdp` or `NPulseSynapseStdp` components:
1. Create synapse with STDP
2. Configure STDP parameters (A+, A-, tau+)
3. Connect to neurons
4. Start training

See [Usage-Examples.md](Usage-Examples.md) for detailed examples.

#### How do I create a classifier based on spiking networks?

Use `NSpikeClassifier` or `NPCAClassifier` components:
1. Create neuron network
2. Connect classifier to network output
3. Configure training
4. Train on data

### Configurations

#### Where can I find configuration examples?

Configuration examples are in:
- `Bin/Configs/User/CognitiveNavigation/` - cognitive navigation
- `Bin/Configs/!OldConfigs/NM-Neurons/` - neuron models
- `Bin/Configs/!OldConfigs/STDP-Simple-01/` - STDP experiments

See [Config-Overview.md](Config-Overview.md) for detailed description.

#### How do I use configuration templates?

See [Config-Templates.md](Config-Templates.md) - templates for typical experiments are described there.

### Performance

#### How do I optimize spiking network performance?

- Use simpler models (IaF instead of Izhikevich) where possible
- Minimize number of neurons in network
- Use result caching
- Avoid excessive logging

### Integration

#### How do I integrate spiking networks with other libraries?

- Use components from Rdk-BasicLib for data I/O
- Use Nmsdk-MotionControlLib for motion control
- See [Usage-Examples.md](Usage-Examples.md) for integration examples

### Troubleshooting

#### Neuron doesn't generate spikes

- Check input pulses (are they connected)
- Check neuron parameters (threshold, membrane potential)
- Check component readiness (`IsReady()`)

#### STDP doesn't work

- Ensure synapse is properly connected
- Check STDP parameters
- Ensure neurons generate spikes

### Additional Resources

- [Architecture.md](Architecture.md) - library architecture
- [Component-Catalog.md](Component-Catalog.md) - component catalog
- [Scientific-Background.md](Scientific-Background.md) - scientific background
- [Docs/Troubleshooting/Troubleshooting-Guide.md](../../../Docs/Troubleshooting/Troubleshooting-Guide.md) - general troubleshooting guide
