# [ComponentName] — [RU название]

**Каталог компонентов:** [Component-Catalog.md](Component-Catalog.md).

## RU

### Назначение

**Класс**: `[ComponentName]` — [краткое описание назначения].
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("[ComponentName]", ...)`.
**Storage-инстансы**: `ClassName = "[ComponentName]"` в `Bin/Configs/*/Model_*.xml`.

[Подробное описание компонента, назначения и области применения.]

**Использование:** `Bin/Configs/SpikeSamples/...`, `Bin/Configs/!OldConfigs/...`, `Bin/Configs/User/...`

### UML-диаграмма классов

```mermaid
classDiagram
    [BaseClass] <|-- [ComponentName]
    class [ComponentName] {
        +Property1 : Type
        +New() [ComponentName]*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
```

**Иерархия наследования:** [описание.]

**Ключевые свойства:** [список свойств.]

### UML-диаграмма последовательности

[По необходимости.]

### Свойства

| Свойство | Тип | Описание |
|----------|-----|----------|
| `Property1` | `Type` | [описание] |

### Методы

- **`ADefault()`** — [описание]
- **`ABuild()`** — [описание]
- **`AReset()`** — [описание]
- **`ACalculate()`** — [описание]

### Примеры использования

[Код C++ и/или XML по необходимости.]

## Источники

- [Literature-References.md](Literature-References.md): [A], [B], [C], [номера публикаций] — [краткое пояснение по тематике компонента]. (В `Docs/Components/*.md` используйте `../Literature-References.md`.)

---

## EN

### Purpose

**Class**: `[ComponentName]` — [brief description].
**Registration**: `NPulseLibrary.cpp` → `UploadClass("[ComponentName]", ...)`.
**Instances**: `ClassName = "[ComponentName]"` in `Bin/Configs/*/Model_*.xml`.

[Detailed description.]

**Usage:** `Bin/Configs/SpikeSamples/...`, `Bin/Configs/!OldConfigs/...`, `Bin/Configs/User/...`

### UML Class Diagram

[Same structure as RU section.]

### Properties

[Same structure as RU section, in English.]

### Methods

[Same structure as RU section, in English.]

### References

See [Literature-References.md](Literature-References.md): **[A]**, **[B]**, **[C]**, [publication numbers].

### See Also

- `RelatedComponent.md` — [brief note] (replace with an actual component)
- [Architecture.md](Architecture.md) — library architecture
- [Scientific-Background.md](Scientific-Background.md) — scientific background

```mermaid
classDiagram
    [BaseClass] <|-- [ComponentName]
    class [ComponentName] {
        +Property1 : Type
        +New() [ComponentName]*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
```
