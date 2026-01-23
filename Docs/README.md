# Nmsdk-PulseLib - Документация

## RU

### Назначение

**Nmsdk-PulseLib** предоставляет компоненты для моделирования импульсных нейронных сетей (Spiking Neural Networks, SNN). Библиотека включает различные модели нейронов, синапсов, каналов, механизмы обучения и классификации.

### Целевые сценарии использования

Библиотека предназначена для следующих основных задач:

1. **Моделирование импульсных нейронных сетей (SNN)**
   - Создание сетей с различными моделями нейронов (Ижикевича, Integrate-and-Fire, биоинспирированные)
   - Настройка синаптических соединений и механизмов пластичности (STDP, правило Хебба)
   - Моделирование динамики мембранных потенциалов и генерации спайков

2. **Эксперименты по обучению нейросетей**
   - Исследование механизмов STDP (Spike-Timing Dependent Plasticity)
   - Обучение классификаторов на основе паттернов спайков
   - Формирование ассоциативных связей и условных рефлексов

3. **Управление движением и эффекторы**
   - Моделирование мышц и моторных нейронов
   - Управление движением через импульсные сети
   - Интеграция с системами управления роботами

4. **Когнитивная навигация и пространственная память**
   - Моделирование пространственной навигации
   - Формирование когнитивных карт
   - Распознавание паттернов движения

5. **Биоинспирированные модели**
   - Моделирование афферентных нейронов
   - Обработка сенсорных сигналов (sEMG, EEG)
   - Реализация биологически правдоподобных механизмов

### Реальные примеры использования

Практические примеры использования компонентов библиотеки можно найти в конфигурационных проектах в директории `Bin/Configs`:

- **Когнитивная навигация**: `Bin/Configs/User/CognitiveNavigation/` — примеры сетей для пространственной навигации
- **Модели нейронов**: `Bin/Configs/!OldConfigs/NM-Neurons/` — различные конфигурации нейронов с обучением
- **Эксперименты по STDP**: `Bin/Configs/!OldConfigs/STDP-Simple-01/` — простые эксперименты по пластичности
- **Классификация**: `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/` — примеры классификаторов

Подробное описание структуры конфигураций и примеры использования см. в разделе [Config-Overview.md](Config-Overview.md).

### Краткий обзор

Библиотека включает компоненты для:
- Моделирования нейронов (различные модели)
- Моделирования синапсов и соединений
- Работы с каналами ионов
- Генерации импульсов
- Классификации на основе импульсных сетей
- Обучения нейронных сетей

### Связь с корневой документацией

Для обзорной информации см. корневую документацию проекта:
- `Docs/Libraries/Nmsdk-PulseLib.md` - обзор библиотеки (в корневом репозитории)

### Детальная документация

- [Architecture.md](Architecture.md) - архитектура библиотеки
- [Usage-Examples.md](Usage-Examples.md) - примеры использования
- [API-Overview.md](API-Overview.md) - обзор API
- [Component-Catalog.md](Component-Catalog.md) - каталог компонентов
- [Config-Overview.md](Config-Overview.md) - обзор конфигурационных проектов
- [Config-Templates.md](Config-Templates.md) - шаблоны типовых экспериментов
- [Scientific-Background.md](Scientific-Background.md) - научный фон и связанные публикации
- [CONTRIBUTING-docs.md](CONTRIBUTING-docs.md) - руководство по расширению документации

---

## EN

### Purpose

**Nmsdk-PulseLib** provides components for modeling Spiking Neural Networks (SNN). The library includes various neuron models, synapses, channels, learning mechanisms, and classification.

### Brief Overview

The library includes components for:
- Neuron modeling (various models)
- Synapse and connection modeling
- Ion channel operations
- Pulse generation
- Classification based on spiking networks
- Neural network training

### Link to Root Documentation

For overview information see root project documentation:
- `Docs/Libraries/Nmsdk-PulseLib.md` - library overview (in root repository)

### Detailed Documentation

- [Architecture.md](Architecture.md) - library architecture
- [Usage-Examples.md](Usage-Examples.md) - usage examples
- [API-Overview.md](API-Overview.md) - API overview
