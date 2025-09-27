#!/bin/bash

# Скрипт для сборки Qt редактора FastEngine
# Использование: ./scripts/build_qt_editor.sh [debug|release]

set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функция для вывода сообщений
print_message() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Проверка аргументов
BUILD_TYPE=${1:-release}
if [[ "$BUILD_TYPE" != "debug" && "$BUILD_TYPE" != "release" ]]; then
    print_error "Неверный тип сборки. Используйте 'debug' или 'release'"
    exit 1
fi

print_message "Начинаем сборку Qt редактора FastEngine..."
print_message "Тип сборки: $BUILD_TYPE"

# Переходим в корневую директорию проекта
cd "$(dirname "$0")/.."

# Проверяем наличие Qt6
print_message "Проверяем наличие Qt6..."
if ! command -v qmake6 &> /dev/null; then
    if ! command -v qmake &> /dev/null; then
        print_error "Qt6 не найден. Установите Qt6 и добавьте его в PATH"
        exit 1
    else
        QMAKE_CMD="qmake"
    fi
else
    QMAKE_CMD="qmake6"
fi

print_success "Qt6 найден: $QMAKE_CMD"

# Проверяем наличие CMake
print_message "Проверяем наличие CMake..."
if ! command -v cmake &> /dev/null; then
    print_error "CMake не найден. Установите CMake"
    exit 1
fi

print_success "CMake найден: $(cmake --version | head -n1)"

# Создаем директорию сборки
BUILD_DIR="build_qt_editor"
print_message "Создаем директорию сборки: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Настраиваем CMake
print_message "Настраиваем CMake..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_QT_EDITOR=ON"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_SIMULATOR=OFF"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_ANDROID=OFF"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_IOS=OFF"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_DESKTOP=ON"

# Добавляем пути к Qt6
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    QT6_DIR=$(brew --prefix qt6)
    if [[ -d "$QT6_DIR" ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DQt6_DIR=$QT6_DIR/lib/cmake/Qt6"
        print_message "Найден Qt6 в Homebrew: $QT6_DIR"
    fi
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    if [[ -d "/usr/lib/x86_64-linux-gnu/cmake/Qt6" ]]; then
        CMAKE_ARGS="$CMAKE_ARGS -DQt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6"
        print_message "Найден Qt6 в системе"
    fi
fi

print_message "Запускаем CMake с параметрами: $CMAKE_ARGS"
cmake .. $CMAKE_ARGS

# Собираем проект
print_message "Собираем проект..."
if command -v ninja &> /dev/null; then
    print_message "Используем Ninja для сборки"
    ninja -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    print_message "Используем Make для сборки"
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

# Проверяем результат сборки
if [[ -f "qt_editor/FastEngineQtEditor" ]]; then
    print_success "Сборка завершена успешно!"
    print_message "Исполняемый файл: $BUILD_DIR/qt_editor/FastEngineQtEditor"
    
    # Копируем ресурсы
    print_message "Копируем ресурсы..."
    if [[ -d "../assets" ]]; then
        cp -r ../assets qt_editor/
        print_success "Ресурсы скопированы"
    fi
    
    # Создаем скрипт запуска
    print_message "Создаем скрипт запуска..."
    cat > run_qt_editor.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./FastEngineQtEditor "$@"
EOF
    chmod +x run_qt_editor.sh
    print_success "Скрипт запуска создан: run_qt_editor.sh"
    
    # Показываем информацию о сборке
    print_message "Информация о сборке:"
    echo "  Тип: $BUILD_TYPE"
    echo "  Директория: $(pwd)/qt_editor"
    echo "  Исполняемый файл: FastEngineQtEditor"
    echo "  Скрипт запуска: run_qt_editor.sh"
    
    # Предлагаем запустить редактор
    read -p "Запустить Qt редактор? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_message "Запускаем Qt редактор..."
        cd qt_editor
        ./FastEngineQtEditor
    fi
    
else
    print_error "Сборка не удалась. Проверьте ошибки выше."
    exit 1
fi

print_success "Готово!"

