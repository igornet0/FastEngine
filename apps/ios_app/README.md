# FastEngine iOS App

Оболочка iOS-приложения для запуска примера на движке FastEngine на iPhone (симулятор или устройство).

## Требования

- macOS с Xcode 12+ (рекомендуется 14+)
- iOS 12.0+ (Deployment Target)
- В репозитории должна быть папка `thirdparty/glm` (заголовки GLM). Если клонировали репо с подмодулями: `git submodule update --init --recursive`.

## Сборка и запуск

### 1. Сборка библиотеки FastEngine для iOS

Из корня репозитория FastEngine:

```bash
chmod +x scripts/build_ios.sh
./scripts/build_ios.sh
```

После успешной сборки библиотека будет в `build/ios/src/libFastEngine.a`.

### 2. Открытие проекта в Xcode

```bash
open ios_app/FastEngineApp.xcodeproj
```

Или откройте `ios_app/FastEngineApp.xcodeproj` через Xcode (File → Open).

### 3. Запуск на симуляторе

1. В Xcode выберите схему **FastEngineApp**.
2. В списке устройств выберите любой **iPhone Simulator** (например, iPhone 15).
3. Нажмите **Run** (Cmd+R).

Приложение запустится в симуляторе: фон, управляемый спрайт (игрок) и несколько спрайтов (враги). Управление — касанием (движение к точке касания).

### 4. Запуск на физическом iPhone

1. Подключите iPhone по USB.
2. Выберите устройство в списке устройств Xcode.
3. В **Signing & Capabilities** укажите свой **Team** (Apple ID или Apple Developer аккаунт).
4. При первом запуске на устройстве может потребоваться: **Настройки → Основные → VPN и управление устройством** → доверие разработчику.
5. Нажмите **Run** (Cmd+R).

## Структура

- **main.mm** — точка входа `UIApplicationMain`.
- **AppDelegate.mm** — создание `UIWindow` и корневого `GameViewController`.
- **GameViewController.mm** — после появления экрана вызывает `StartFastEngine()`, подключает `CADisplayLink` для вызова отрисовки каждый кадр.
- **GameBridge.cpp** — C++ мост: инициализация `Platform`, `Engine`, `Game` (пример из `examples/`), цикл кадров через `Engine::RunOneFrame()`.
- **Game.cpp / Game.h** — подключаются из `../examples` (тот же пример, что и ExampleApp на десктопе).

Ресурсы (шейдеры) копируются из `../assets/shaders` в бандл приложения.

## Отладка

- Логи из `std::cout` видны в консоли Xcode при запуске из Xcode.
- При падении проверьте: сборка движка для iOS выполнена (`build/ios/src/libFastEngine.a` существует), пути к заголовкам и библиотеке в настройках target корректны, в бандл попала папка `shaders` с `sprite.vert` и `sprite.frag`.

## Решение проблем

- **«glm/glm.hpp file not found»** — убедитесь, что в корне репозитория есть `thirdparty/glm/glm/glm.hpp`. Выполните `git submodule update --init --recursive` или добавьте GLM в `thirdparty/glm`.
- **«library not found»** — убедитесь, что выполнен шаг 1 и файл `build/ios/src/libFastEngine.a` существует.
- **Чёрный экран** — проверьте консоль Xcode на ошибки загрузки шейдеров; убедитесь, что в Copy Bundle Resources добавлена папка `shaders` из `../assets`.
- **Ошибка подписи** — выберите Team в Signing & Capabilities или включите автоматическую подпись.
