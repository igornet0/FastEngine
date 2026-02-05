# Snake (snake_cros_app)

Кроссплатформенная игра «Змейка» на FastEngine. Собирается для Desktop (macOS/Windows/Linux); игровая логика не зависит от платформы и может быть переиспользована для iOS/Android при подключении того же кода в соответствующий проект.

## Требования

- CMake 3.16+, C++17
- SDL2, OpenGL (подтягиваются при сборке движка)
- Сборка из корня репозитория с `BUILD_DESKTOP=ON`

## Сборка

Из корня FastEngine:

```bash
./scripts/build_desktop.sh
```

Или вручную:

```bash
mkdir -p build/snake
cd build/snake
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DBUILD_DESKTOP=ON
make snake_cros_app -j$(nproc)
```

Исполняемый файл: `build/snake/apps/snake_cros_app/snake_cros_app`.

## Запуск

Запускайте из корня репозитория (чтобы находились `assets/`):

```bash
./build/snake/apps/snake_cros_app/snake_cros_app
```

## Управление

- **Стрелки** или **WASD** — направление движения змейки (нельзя развернуться на 180°)
- **R** или **Пробел** — перезапуск после Game Over
- **ESC** — выход

## Правила

- Сетка 20×15 клеток. Змейка растёт, поедая красную еду.
- Столкновение со стеной или с собственным хвостом — Game Over.
- Счёт выводится в консоль при Game Over; для рестарта нажмите R или Пробел.

## Кроссплатформенность

- **Desktop:** полная поддержка (тот же CMake, что и у других приложений).
- **iOS/Android:** код `Game` и `GameUpdateSystem` не использует платформо-специфичные API; для мобильных платформ достаточно подключить эти файлы в целевой проект и вызвать `Game::Initialize/Update/Render/Shutdown` из платформенного цикла (как в `ios_app` через GameBridge).

## Структура

- `main.cpp` — точка входа, инициализация движка и игры, цикл
- `Game.h` / `Game.cpp` — логика змейки (сетка, движение, еда, столкновения, рестарт), рендер через ECS (сущности со Sprite/Transform)
- `GameUpdateSystem.h` / `GameUpdateSystem.cpp` — система, вызывающая `Game::Update()` каждый кадр
