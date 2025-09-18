# FastEngine

Кроссплатформенный 2D-движок на C++ для Android, iOS и Desktop платформ.

## Особенности

- 🚀 **Высокая производительность** - написан на C++17
- 📱 **Кроссплатформенность** - Android, iOS, Windows, Linux, macOS
- 🎮 **ECS архитектура** - Entity Component System для гибкой разработки
- 🎨 **2D рендеринг** - OpenGL ES для мобильных, OpenGL для десктопа
- 🔊 **Аудио система** - поддержка звуков и музыки
- 🎯 **Простой API** - легко изучать и использовать

## Требования

### Для всех платформ
- CMake 3.16+
- C++17 компилятор

### Android
- Android NDK r21+
- Android SDK

### iOS
- Xcode 12+
- iOS 12.0+

### Desktop
- OpenGL 3.3+
- SDL2 (автоматически подключается)

## Быстрый старт

### 1. Клонирование репозитория
```bash
git clone <repository-url>
cd FastEngine
```

### 2. Сборка для Desktop
```bash
chmod +x scripts/build_desktop.sh
./scripts/build_desktop.sh
```

### 3. Сборка для Android
```bash
# Установите Android NDK и установите переменную ANDROID_NDK
export ANDROID_NDK=/path/to/android-ndk

chmod +x scripts/build_android.sh
./scripts/build_android.sh
```

### 4. Сборка для iOS
```bash
chmod +x scripts/build_ios.sh
./scripts/build_ios.sh
```

## Использование

### Базовый пример
```cpp
#include "FastEngine/Engine.h"
#include "FastEngine/Components/Transform.h"
#include "FastEngine/Components/Sprite.h"

int main() {
    // Создаем движок
    FastEngine::Engine engine;
    
    // Инициализируем
    if (!engine.Initialize("My Game", 800, 600)) {
        return -1;
    }
    
    // Получаем мир
    auto* world = engine.GetWorld();
    
    // Создаем игрока
    auto* player = world->CreateEntity();
    player->AddComponent<FastEngine::Transform>(100.0f, 100.0f);
    player->AddComponent<FastEngine::Sprite>("player.png");
    
    // Запускаем игру
    engine.Run();
    
    return 0;
}
```

## Архитектура

### Структура проекта
```
FastEngine/
├── src/                    # Исходный код
│   ├── core/              # Ядро движка (ECS)
│   ├── platform/          # Платформенная абстракция
│   ├── render/            # Система рендеринга
│   ├── audio/             # Аудио система
│   └── input/             # Система ввода
├── include/               # Заголовочные файлы
├── examples/              # Примеры использования
├── scripts/               # Скрипты сборки
└── thirdparty/            # Внешние библиотеки
```

### Основные компоненты

#### Entity Component System (ECS)
- **Entity** - игровой объект
- **Component** - данные (Transform, Sprite, etc.)
- **System** - логика (Rendering, Physics, etc.)

#### Платформенная абстракция
- **Platform** - единый интерфейс для всех платформ
- **Window** - управление окном/поверхностью
- **FileSystem** - работа с файлами
- **Timer** - управление временем

#### Система рендеринга
- **Renderer** - основной рендерер
- **Texture** - загрузка и управление текстурами
- **Sprite** - отрисовка 2D спрайтов
- **Camera** - управление камерой

## Разработка

### Добавление нового компонента
```cpp
class MyComponent : public FastEngine::Component {
public:
    MyComponent(int value) : m_value(value) {}
    
    int GetValue() const { return m_value; }
    void SetValue(int value) { m_value = value; }
    
private:
    int m_value;
};

// Использование
auto* entity = world->CreateEntity();
entity->AddComponent<MyComponent>(42);
auto* comp = entity->GetComponent<MyComponent>();
```

### Добавление новой системы
```cpp
class MySystem : public FastEngine::System {
public:
    MySystem(FastEngine::World* world) : System(world) {}
    
    void Update(float deltaTime) override {
        // Логика системы
    }
};

// Регистрация
world->AddSystem<MySystem>();
```

## Лицензия

MIT License - см. файл LICENSE для подробностей.

## Вклад в проект

1. Форкните репозиторий
2. Создайте ветку для новой функции
3. Внесите изменения
4. Создайте Pull Request

## Поддержка

Если у вас есть вопросы или проблемы, создайте Issue в репозитории.