# Зависимости FastEngine

## Обязательные зависимости

### CMake
- **Версия**: 3.16+
- **Установка**:
  - macOS: `brew install cmake`
  - Ubuntu/Debian: `sudo apt install cmake`
  - Windows: [Скачать с официального сайта](https://cmake.org/download/)

### C++17 компилятор
- **macOS**: Xcode Command Line Tools (`xcode-select --install`)
- **Linux**: GCC 7+ или Clang 5+
- **Windows**: Visual Studio 2019+ или MinGW-w64

## Платформо-специфичные зависимости

### Android
- **Android NDK**: r21+ (рекомендуется r25)
- **Android SDK**: API Level 21+
- **Установка**:
  ```bash
  # Через Android Studio SDK Manager
  # Или скачать с https://developer.android.com/ndk/downloads
  export ANDROID_NDK=/path/to/android-ndk
  ```

### iOS
- **Xcode**: 12+ (рекомендуется 14+)
- **iOS Deployment Target**: 12.0+
- **Установка**: Через App Store

### Desktop (Windows/Linux/macOS)

#### OpenGL
- **Версия**: 3.3+ (Desktop), OpenGL ES 2.0+ (Mobile)
- **Установка**: Обычно входит в драйверы видеокарты

#### SDL2
- **Версия**: 2.0.10+
- **Установка**:
  - macOS: `brew install sdl2`
  - Ubuntu/Debian: `sudo apt install libsdl2-dev`
  - Windows: [Скачать с libsdl.org](https://www.libsdl.org/download-2.0.php)

## Опциональные зависимости

### Аудио
- **OpenAL Soft**: Для кроссплатформенного аудио
- **SoLoud**: Альтернативная аудио библиотека
- **Установка**:
  - macOS: `brew install openal-soft`
  - Ubuntu/Debian: `sudo apt install libopenal-dev`

### Физика
- **Box2D**: Для 2D физики
- **Установка**: Включена в thirdparty/

### Шрифты
- **FreeType**: Для рендеринга текста
- **Установка**:
  - macOS: `brew install freetype`
  - Ubuntu/Debian: `sudo apt install libfreetype6-dev`

## Установка зависимостей

### macOS
```bash
# Установка через Homebrew
brew install cmake sdl2 openal-soft freetype

# Установка Xcode Command Line Tools
xcode-select --install
```

### Ubuntu/Debian
```bash
# Обновление пакетов
sudo apt update

# Установка зависимостей
sudo apt install build-essential cmake libsdl2-dev libopenal-dev libfreetype6-dev

# Для Android разработки
sudo apt install openjdk-11-jdk
```

### Windows
1. Установите Visual Studio 2019+ с C++ поддержкой
2. Установите CMake
3. Скачайте SDL2 development libraries
4. Распакуйте SDL2 в `thirdparty/sdl2/`

## Проверка установки

### Проверка CMake
```bash
cmake --version
```

### Проверка компилятора
```bash
# macOS/Linux
g++ --version
clang++ --version

# Windows
cl
```

### Проверка SDL2
```bash
# macOS
pkg-config --modversion sdl2

# Linux
pkg-config --modversion SDL2
```

## Решение проблем

### Ошибка "CMake not found"
- Убедитесь, что CMake установлен и добавлен в PATH
- Перезапустите терминал после установки

### Ошибка "SDL2 not found"
- Убедитесь, что SDL2 development libraries установлены
- Проверьте, что заголовочные файлы находятся в правильных директориях

### Ошибка компиляции на Android
- Проверьте, что ANDROID_NDK установлен и переменная окружения настроена
- Убедитесь, что версия NDK совместима с CMake

### Ошибка компиляции на iOS
- Убедитесь, что Xcode установлен и обновлен
- Проверьте, что iOS Deployment Target установлен правильно
