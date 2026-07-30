# Руководство по расширению документации Nmsdk-PulseLib

## RU

### Назначение

Этот документ описывает, как добавлять и обновлять документацию библиотеки Nmsdk-PulseLib. Следование этим рекомендациям поможет поддерживать согласованность и полноту документации.

### Структура документации

Документация библиотеки организована следующим образом:

```
Docs/
├── README.md                    # Главная страница документации
├── Architecture.md              # Архитектура библиотеки
├── API-Overview.md              # Обзор API компонентов
├── Usage-Examples.md            # Примеры использования
├── Component-Catalog.md         # Каталог компонентов
├── Config-Overview.md          # Обзор конфигурационных проектов
├── Config-Templates.md         # Шаблоны типовых экспериментов
├── Scientific-Background.md    # Научный фон и публикации
├── CONTRIBUTING-docs.md        # Это руководство
├── Favorites-Curation-Guide.md # Курация ClDesc Favorites (primary vs alias)
├── Components/                 # Детальная документация компонентов
│   ├── Neurons.md
│   ├── SynapsesAndTrainers.md
│   ├── ChannelsMembranesLT.md
│   ├── GeneratorsIOClassifiers.md
│   └── [ИмяКомпонента].md      # Для каждого компонента
└── Doxygen/
    └── Doxyfile                # Конфигурация Doxygen
```

### Добавление документации для нового компонента

#### Шаг 1: Создание файла компонента

1. Создайте файл `Docs/Components/[ИмяКомпонента].md` (например, `NPulseNeuronIzhikevich.md`)

2. Используйте следующий шаблон:

```markdown
# [ИмяКомпонента]

## RU

### Назначение

Краткое описание назначения компонента (1-2 предложения).

### Основные свойства

Список ключевых свойств/параметров, которые чаще всего настраиваются:

- `Property1` (тип) - описание
- `Property2` (тип) - описание

### Использование в конфигурациях

Укажите, где компонент используется в `Bin/Configs`:

- `Bin/Configs/путь/к/проекту/` - описание использования

### Пример использования

```cpp
// Пример кода на C++
auto component = storage->CreateComponent<[ИмяКомпонента]>();
component->Property1 = value;
component->Build();
```

### Пример конфигурации XML

```xml
<Component1 Class="[ИмяКомпонента]">
    <Parameters>
        <Property1>value</Property1>
        <Property2>value</Property2>
    </Parameters>
</Component1>
```

### Связанные компоненты

- [`СвязанныйКомпонент1`](СвязанныйКомпонент1.md) - описание связи
- [`СвязанныйКомпонент2`](СвязанныйКомпонент2.md) - описание связи

### См. также

- [Научный фон](Scientific-Background.md) - если есть связанные публикации
- [API-Overview.md](API-Overview.md) - обзор API

---

## EN

### Purpose

Brief description of component purpose.

### Main Properties

- `Property1` (type) - description
- `Property2` (type) - description

### See Also

- [Scientific-Background.md](Scientific-Background.md)
- [API-Overview.md](API-Overview.md)
```

#### Шаг 2: Добавление ссылки в Component-Catalog.md

Добавьте ссылку на новый компонент в соответствующий раздел `Component-Catalog.md`:

```markdown
- **[ComponentName]** — [`[ComponentName]`](Components/[ComponentName].md)  
  *Usage:* `Bin/Configs/path/to/project/`
```

#### Шаг 3: Обновление групповых обзоров (если применимо)

Если компонент относится к группе (нейроны, синапсы, каналы и т.д.), обновите соответствующий групповой файл в `Components/`:

- `Neurons.md` — для нейронов
- `SynapsesAndTrainers.md` — для синапсов и тренеров
- `ChannelsMembranesLT.md` — для каналов, мембран и LT-зон
- `GeneratorsIOClassifiers.md` — для генераторов, IO и классификаторов

### Обновление существующей документации

#### Обновление API-Overview.md

При добавлении нового приоритетного компонента:

1. Добавьте описание в соответствующий раздел (`Нейроны`, `Синапсы`, и т.д.)
2. Укажите основные свойства, которые настраиваются в конфигурациях
3. Добавьте ссылку на детальную документацию компонента
4. Укажите примеры использования в `Bin/Configs`

#### Обновление Config-Overview.md

При добавлении нового типа конфигурационного проекта:

1. Добавьте описание в раздел "Типы конфигурационных проектов"
2. Укажите расположение проектов
3. Перечислите типичные компоненты, используемые в таких проектах
4. Приведите примеры конфигураций

#### Обновление Config-Templates.md

При создании нового шаблона эксперимента:

1. Добавьте новый раздел "Шаблон N: [Название]"
2. Опишите назначение шаблона
3. Перечислите структуру компонентов
4. Приведите пример конфигурации XML
5. Укажите связи между компонентами

#### Обновление Usage-Examples.md

При добавлении нового примера использования:

1. Определите, к какому типу задачи относится пример
2. Добавьте пример в соответствующий раздел
3. Свяжите пример с конкретной конфигурацией в `Bin/Configs`
4. Покажите соответствие между кодом C++ и XML-конфигурацией

#### Обновление Scientific-Background.md

При добавлении информации о научных публикациях:

1. Определите тематику (модели нейронов, STDP, биоинспирированные модели и т.д.)
2. Добавьте раздел с описанием научной основы
3. Укажите ключевые публикации с полными ссылками
4. Свяжите публикации с компонентами PulseLib
5. Укажите использование в конфигурациях

### Стиль документации

#### Двуязычность

Все основные документы должны содержать разделы на русском (RU) и английском (EN) языках:

```markdown

## RU

[Contents in Russian]

---

## EN

[Content in English]
```

#### Формат ссылок на конфигурации

Используйте следующий формат для ссылок на конфигурационные проекты:

```markdown
*Usage:* `Bin/Configs/path/to/project/`
*Used в:* `Bin/Configs/path/to/project/`
*Frequently used в:* `Bin/Configs/path/to/project/`
```

#### Формат ссылок на компоненты

Используйте относительные ссылки:

```markdown
[`ИмяКомпонента`](Components/[ComponentName].md)
(шаблон имени файла — как в [Component-Documentation-Template.md](Component-Documentation-Template.md))
```

#### Формат ссылок на другие документы

Используйте относительные ссылки:

```markdown
[Config-Overview.md](Config-Overview.md)
[API-Overview.md](API-Overview.md)
```

### Работа с конфигурационными проектами

При документировании использования компонентов в конфигурациях:

1. **Изучите реальные конфигурации**: Откройте `Model_*.xml` и `Parameters_*.xml` в `Bin/Configs`
2. **Определите типичные параметры**: Какие параметры чаще всего настраиваются
3. **Опишите структуру**: Какие компоненты используются вместе
4. **Приведите примеры**: Покажите фрагменты XML с реальными значениями

### Проверка документации

Перед добавлением документации проверьте:

- [ ] Все ссылки работают (относительные пути корректны)
- [ ] Документ содержит разделы RU и EN
- [ ] Компонент добавлен в `Component-Catalog.md`
- [ ] Обновлены групповые обзоры (если применимо)
- [ ] Указаны примеры использования в `Bin/Configs`
- [ ] Форматирование Markdown корректно

### Примеры хорошей документации

#### Пример 1: Документация компонента с примерами конфигураций

```markdown
### Usage in configurations

Component `NPulseNeuronIzhikevich` widely used в experiments:

- `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/` — demonstration various types neurons
- `Bin/Configs/!OldConfigs/STDP-Simple-01/` — base experiment по STDP-training

### Example configuration XML

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

Parameters A, B, C, D match properties class `NPulseNeuronIzhikevich` в code.
```

#### Пример 2: Связь с научными публикациями

```markdown
### Scientific basis

Model Izhikevich described в following publications:

- Izhikevich, E. M. (2003). "Simple model of spiking neurons." IEEE Transactions on Neural Networks, 14(6), 1569-1572.

**Link с components:**
- Parameters `A`, `B`, `C`, `D` в `NPulseNeuronIzhikevich` match parameters models из publications

**See also:** [Scientific-Background.md](Scientific-Background.md)
```

### Часто задаваемые вопросы

**Q: Где размещать документацию для нового компонента?**  
A: Создайте файл `Docs/Components/[ИмяКомпонента].md` и добавьте ссылку в `Component-Catalog.md`.

**Q: Нужно ли документировать все компоненты?**  
A: Приоритет отдается компонентам, часто используемым в `Bin/Configs`. См. раздел "Приоритет описания" в `Component-Catalog.md`.

**Q: Как связать компонент с конфигурационными проектами?**  
A: Изучите `Bin/Configs`, найдите проекты, использующие компонент, и укажите пути в разделе "Использование в конфигурациях".

**Q: Нужно ли обновлять Doxygen?**  
A: Doxygen автоматически обрабатывает Markdown-файлы из `Docs/`, если они указаны в `INPUT`. Убедитесь, что `USE_MDFILE_AS_MAINPAGE` указывает на `README.md`.

### See Also

- [README.md](README.md) - главная страница документации
- [Component-Catalog.md](Component-Catalog.md) - каталог компонентов
- [Config-Overview.md](Config-Overview.md) - описание конфигураций

---

## EN

### Purpose

This document describes how to add and update documentation for the Nmsdk-PulseLib library. Following these guidelines will help maintain consistency and completeness of the documentation.

### Documentation Structure

The library documentation is organized as follows:

```
Docs/
├── README.md                    # Main documentation page
├── Architecture.md              # Library architecture
├── API-Overview.md              # Component API overview
├── Usage-Examples.md            # Usage examples
├── Component-Catalog.md         # Component catalog
├── Config-Overview.md           # Configuration projects overview
├── Config-Templates.md          # Typical experiment templates
├── Scientific-Background.md     # Scientific background and publications
├── CONTRIBUTING-docs.md        # This guide
└── Components/                 # Detailed component documentation
```

### Adding Documentation for a New Component

#### Step 1: Create Component File

1. Create file `Docs/Components/[ComponentName].md`

2. Use the following template:

```markdown
# [ComponentName]

## RU

### Purpose

Brief description of component purpose (1-2 sentences).

### Main Properties

List of key properties/parameters that are most commonly configured:

- `Property1` (type) - description
- `Property2` (type) - description

### Usage in Configurations

Specify where the component is used in `Bin/Configs`:

- `Bin/Configs/path/to/project/` - usage description

### See Also

- [API-Overview.md](API-Overview.md)
- [Scientific-Background.md](Scientific-Background.md)

---

## EN

### Purpose

Brief description of component purpose.

### Main Properties

- `Property1` (type) - description
- `Property2` (type) - description

### See Also

- [API-Overview.md](API-Overview.md)
- [Scientific-Background.md](Scientific-Background.md)
```

#### Step 2: Add Reference in Component-Catalog.md

Add a link to the new component in the appropriate section of `Component-Catalog.md`.

#### Step 3: Update Group Overviews (if applicable)

If the component belongs to a group (neurons, synapses, channels, etc.), update the corresponding group file in `Components/`.

### Documentation Style

#### Bilingual Content

All main documents should contain sections in Russian (RU) and English (EN).

#### Configuration References Format

Use the following format for references to configuration projects:

```markdown
*Usage:* `Bin/Configs/path/to/project/`
```

### See Also

- [README.md](README.md) - main documentation page
- [Component-Catalog.md](Component-Catalog.md) - component catalog
- [Config-Overview.md](Config-Overview.md) - configuration description
