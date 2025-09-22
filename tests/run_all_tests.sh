#!/bin/bash

# Скрипт для запуска всех тестов FastEngine
# Автор: FastEngine Team
# Версия: 1.0

set -e  # Выход при ошибке

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

# Функция для проверки зависимостей
check_dependencies() {
    print_message "Проверка зависимостей..."
    
    # Проверяем CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake не найден. Установите CMake 3.16+"
        exit 1
    fi
    
    # Проверяем компилятор C++
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "Компилятор C++ не найден. Установите GCC или Clang"
        exit 1
    fi
    
    # Проверяем Google Test
    if ! pkg-config --exists gtest; then
        print_warning "Google Test не найден. Установите libgtest-dev"
    fi
    
    print_success "Зависимости проверены"
}

# Функция для создания директории сборки
create_build_dir() {
    print_message "Создание директории сборки..."
    
    if [ -d "build_test" ]; then
        print_message "Удаление существующей директории сборки..."
        rm -rf build_test
    fi
    
    mkdir build_test
    cd build_test
    
    print_success "Директория сборки создана"
}

# Функция для конфигурации CMake
configure_cmake() {
    print_message "Конфигурация CMake..."
    
    local cmake_args="-DBUILD_TESTS=ON"
    
    # Добавляем флаги для тестирования
    if [ "$ENABLE_ASAN" = "true" ]; then
        cmake_args="$cmake_args -DENABLE_ASAN=ON"
        print_message "AddressSanitizer включен"
    fi
    
    if [ "$ENABLE_TSAN" = "true" ]; then
        cmake_args="$cmake_args -DENABLE_TSAN=ON"
        print_message "ThreadSanitizer включен"
    fi
    
    if [ "$ENABLE_UBSAN" = "true" ]; then
        cmake_args="$cmake_args -DENABLE_UBSAN=ON"
        print_message "UndefinedBehaviorSanitizer включен"
    fi
    
    if [ "$ENABLE_COVERAGE" = "true" ]; then
        cmake_args="$cmake_args -DENABLE_COVERAGE=ON"
        print_message "Покрытие кода включено"
    fi
    
    cmake .. $cmake_args
    
    print_success "CMake сконфигурирован"
}

# Функция для сборки тестов
build_tests() {
    print_message "Сборка тестов..."
    
    local jobs=$(sysctl -n hw.ncpu)
    if [ $jobs -gt 8 ]; then
        jobs=8
    fi
    
    make -j$jobs
    
    print_success "Тесты собраны"
}

# Функция для запуска unit тестов
run_unit_tests() {
    print_message "Запуск unit тестов..."
    
    if [ -f "./UnitTests" ]; then
        ./UnitTests --gtest_output=xml:unit_test_results.xml
        print_success "Unit тесты завершены"
    else
        print_error "UnitTests не найден"
        return 1
    fi
}

# Функция для запуска интеграционных тестов
run_integration_tests() {
    print_message "Запуск интеграционных тестов..."
    
    if [ -f "./IntegrationTests" ]; then
        ./IntegrationTests --gtest_output=xml:integration_test_results.xml
        print_success "Интеграционные тесты завершены"
    else
        print_warning "IntegrationTests не найден"
    fi
}

# Функция для запуска тестов производительности
run_performance_tests() {
    print_message "Запуск тестов производительности..."
    
    if [ -f "./PerformanceTests" ]; then
        ./PerformanceTests --gtest_output=xml:performance_test_results.xml
        print_success "Тесты производительности завершены"
    else
        print_warning "PerformanceTests не найден"
    fi
}

# Функция для запуска тестов безопасности
run_security_tests() {
    print_message "Запуск тестов безопасности..."
    
    if [ -f "./SecurityTests" ]; then
        ./SecurityTests --gtest_output=xml:security_test_results.xml
        print_success "Тесты безопасности завершены"
    else
        print_warning "SecurityTests не найден"
    fi
}

# Функция для генерации отчета о покрытии
generate_coverage_report() {
    if [ "$ENABLE_COVERAGE" = "true" ]; then
        print_message "Генерация отчета о покрытии..."
        
        if command -v gcovr &> /dev/null; then
            gcovr -r .. --html --html-details -o coverage_report.html
            print_success "Отчет о покрытии сгенерирован: coverage_report.html"
        else
            print_warning "gcovr не найден. Установите gcovr для генерации отчета о покрытии"
        fi
    fi
}

# Функция для генерации сводного отчета
generate_summary_report() {
    print_message "Генерация сводного отчета..."
    
    local total_tests=0
    local passed_tests=0
    local failed_tests=0
    
    # Подсчитываем результаты тестов
    for xml_file in *_test_results.xml; do
        if [ -f "$xml_file" ]; then
            local tests=$(grep -o 'tests="[0-9]*"' "$xml_file" | grep -o '[0-9]*' | head -1)
            local failures=$(grep -o 'failures="[0-9]*"' "$xml_file" | grep -o '[0-9]*' | head -1)
            
            if [ ! -z "$tests" ]; then
                total_tests=$((total_tests + tests))
                passed_tests=$((passed_tests + tests - failures))
                failed_tests=$((failed_tests + failures))
            fi
        fi
    done
    
    # Создаем HTML отчет
    cat > test_summary.html << EOF
<!DOCTYPE html>
<html>
<head>
    <title>FastEngine Test Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .summary { margin: 20px 0; }
        .success { color: green; }
        .error { color: red; }
        .warning { color: orange; }
    </style>
</head>
<body>
    <div class="header">
        <h1>FastEngine Test Results</h1>
        <p>Дата: $(date)</p>
        <p>Платформа: $(uname -s) $(uname -m)</p>
    </div>
    
    <div class="summary">
        <h2>Сводка тестов</h2>
        <p>Всего тестов: <strong>$total_tests</strong></p>
        <p>Пройдено: <strong class="success">$passed_tests</strong></p>
        <p>Провалено: <strong class="error">$failed_tests</strong></p>
        
        <h3>Покрытие тестами</h3>
        <ul>
            <li>Компоненты: Transform, Sprite, Animator, RigidBody, Collider, AudioSource, Text</li>
            <li>Системы: PhysicsSystem, AnimationSystem, InputSystem, RenderSystem</li>
            <li>Модули: AudioManager, ResourceManager</li>
        </ul>
    </div>
    
    <div class="summary">
        <h2>Детальные результаты</h2>
        <ul>
            <li><a href="unit_test_results.xml">Unit тесты</a></li>
            <li><a href="integration_test_results.xml">Интеграционные тесты</a></li>
            <li><a href="performance_test_results.xml">Тесты производительности</a></li>
            <li><a href="security_test_results.xml">Тесты безопасности</a></li>
        </ul>
    </div>
</body>
</html>
EOF
    
    print_success "Сводный отчет сгенерирован: test_summary.html"
}

# Функция для очистки
cleanup() {
    print_message "Очистка временных файлов..."
    
    # Удаляем временные файлы
    rm -f *.gcov *.gcda *.gcno
    
    print_success "Очистка завершена"
}

# Основная функция
main() {
    print_message "Запуск тестирования FastEngine"
    print_message "Версия: 1.0"
    print_message "Дата: $(date)"
    
    # Проверяем аргументы командной строки
    while [[ $# -gt 0 ]]; do
        case $1 in
            --asan)
                ENABLE_ASAN=true
                shift
                ;;
            --tsan)
                ENABLE_TSAN=true
                shift
                ;;
            --ubsan)
                ENABLE_UBSAN=true
                shift
                ;;
            --coverage)
                ENABLE_COVERAGE=true
                shift
                ;;
            --help)
                echo "Использование: $0 [опции]"
                echo "Опции:"
                echo "  --asan      Включить AddressSanitizer"
                echo "  --tsan      Включить ThreadSanitizer"
                echo "  --ubsan     Включить UndefinedBehaviorSanitizer"
                echo "  --coverage  Включить покрытие кода"
                echo "  --help      Показать эту справку"
                exit 0
                ;;
            *)
                print_error "Неизвестная опция: $1"
                exit 1
                ;;
        esac
    done
    
    # Выполняем тестирование
    check_dependencies
    create_build_dir
    configure_cmake
    build_tests
    
    # Запускаем тесты
    run_unit_tests
    run_integration_tests
    run_performance_tests
    run_security_tests
    
    # Генерируем отчеты
    generate_coverage_report
    generate_summary_report
    cleanup
    
    print_success "Тестирование завершено успешно!"
    print_message "Результаты сохранены в build_test/"
}

# Запускаем основную функцию
main "$@"
