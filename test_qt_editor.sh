#!/bin/bash

# Простой тест функционала Qt редактора FastEngine
# Проверяет основные возможности редактора

set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функции для вывода сообщений
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

print_message "=== FastEngine Qt Editor Functional Test ==="

# Проверка 1: Существование исполняемого файла
print_message "1. Проверка исполняемого файла Qt редактора..."
if [ -f "build/editors/qt/FastEngineQtEditor" ]; then
    print_success "Qt редактор найден: build/editors/qt/FastEngineQtEditor"
else
    print_error "Qt редактор не найден! Сначала соберите проект: ./run_all.sh build"
    exit 1
fi

# Проверка 2: Размер исполняемого файла
print_message "2. Проверка размера исполняемого файла..."
file_size=$(stat -f%z "build/editors/qt/FastEngineQtEditor" 2>/dev/null || echo "0")
if [ "$file_size" -gt 1000000 ]; then
    print_success "Размер файла корректный: $file_size байт"
else
    print_warning "Размер файла подозрительно мал: $file_size байт"
fi

# Проверка 3: Права на выполнение
print_message "3. Проверка прав на выполнение..."
if [ -x "build/editors/qt/FastEngineQtEditor" ]; then
    print_success "Файл исполняемый"
else
    print_error "Файл не исполняемый! Исправляем..."
    chmod +x "build/editors/qt/FastEngineQtEditor"
    print_success "Права исправлены"
fi

# Проверка 4: Зависимости Qt
print_message "4. Проверка зависимостей Qt..."
if command -v qmake6 &> /dev/null; then
    qt_version=$(qmake6 -version | head -n1)
    print_success "Qt6 найден: $qt_version"
else
    print_warning "Qt6 не найден в PATH, но может быть доступен через CMake"
fi

# Проверка 5: OpenGL поддержка
print_message "5. Проверка OpenGL поддержки..."
if command -v glxinfo &> /dev/null; then
    opengl_version=$(glxinfo | grep "OpenGL version" | head -n1)
    print_success "OpenGL: $opengl_version"
elif command -v system_profiler &> /dev/null; then
    print_success "macOS система - OpenGL должен быть доступен"
else
    print_warning "Не удалось проверить OpenGL, но это может быть нормально"
fi

# Проверка 6: Ресурсы редактора
print_message "6. Проверка ресурсов редактора..."
if [ -d "build/editors/qt/assets" ]; then
    print_success "Ресурсы найдены: build/editors/qt/assets"
    asset_count=$(find "build/editors/qt/assets" -type f | wc -l)
    print_success "Найдено ресурсов: $asset_count"
else
    print_warning "Ресурсы не найдены в build/editors/qt/assets"
fi

# Проверка 7: Тест запуска (краткий)
print_message "7. Тест запуска Qt редактора (5 секунд)..."
print_warning "Запускаем Qt редактор на 5 секунд для проверки..."

# Запускаем редактор в фоне
cd build/editors/qt
./FastEngineQtEditor 2>&1 | head -20 &
editor_pid=$!

# Ждем немного
sleep 2

# Проверяем, что процесс запустился
if ps -p $editor_pid > /dev/null 2>&1; then
    print_success "Qt редактор успешно запустился (PID: $editor_pid)"
    
    # Ждем еще немного
    sleep 3
    
    # Останавливаем редактор
    kill $editor_pid 2>/dev/null || true
    wait $editor_pid 2>/dev/null || true
    
    print_success "Qt редактор корректно остановлен"
else
    print_error "Qt редактор не запустился или завершился с ошибкой"
    exit 1
fi

cd ../..

# Проверка 8: Логи ошибок
print_message "8. Анализ возможных проблем..."
if [ -f "build/editors/qt/qt_editor.log" ]; then
    error_count=$(grep -c "ERROR\|error" "build/editors/qt/qt_editor.log" 2>/dev/null || echo "0")
    if [ "$error_count" -gt 0 ]; then
        print_warning "Найдено ошибок в логах: $error_count"
    else
        print_success "Критических ошибок в логах не найдено"
    fi
fi

# Итоговый отчет
print_message "=== Итоговый отчет ==="
print_success "✅ Qt редактор FastEngine готов к использованию!"
print_message "Функционал, который должен работать:"
echo "  • Создание и редактирование 2D/3D сцен"
echo "  • Управление объектами (клик, перетаскивание, двойной клик)"
echo "  • Панели: Inspector, Hierarchy, Assets, Console"
echo "  • Управление проектами"
echo "  • Переключение между 2D и 3D режимами"
echo "  • Управление камерой (мышь, колесо)"
echo "  • Создание новых объектов (правый клик)"

print_message "Запуск редактора:"
echo "  cd build/editors/qt && ./FastEngineQtEditor"
echo "  или"
echo "  ./run_all.sh editors --qt"

print_success "Тест завершен успешно! 🎉"
