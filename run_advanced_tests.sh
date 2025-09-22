#!/bin/bash

# Скрипт для запуска расширенных тестов FastEngine
# Поддерживает интеграцию с FastEngine, санитайзеры и тесты производительности

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

# Функция для проверки зависимостей
check_dependencies() {
    print_message "Проверка зависимостей..."
    
    # Проверяем CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake не найден. Установите CMake для продолжения."
        exit 1
    fi
    
    # Проверяем Google Test
    if ! pkg-config --exists gtest; then
        print_error "Google Test не найден. Установите libgtest-dev для продолжения."
        exit 1
    fi
    
    # Проверяем SDL2
    if ! pkg-config --exists sdl2; then
        print_warning "SDL2 не найден. Некоторые тесты могут не работать."
    fi
    
    # Проверяем GLM
    if ! pkg-config --exists glm; then
        print_warning "GLM не найден. Некоторые тесты могут не работать."
    fi
    
    print_success "Зависимости проверены"
}

# Функция для создания директорий
create_directories() {
    print_message "Создание директорий для тестов..."
    
    mkdir -p build_advanced_tests
    mkdir -p test_results
    mkdir -p coverage_reports
    mkdir -p performance_reports
    mkdir -p sanitizer_reports
    
    print_success "Директории созданы"
}

# Функция для сборки тестов
build_tests() {
    local test_type=$1
    local sanitizer=$2
    
    print_message "Сборка тестов: $test_type"
    
    cd build_advanced_tests
    
    # Настройка CMake
    local cmake_args="-DBUILD_TESTS=ON"
    
    if [ "$test_type" = "integration" ]; then
        cmake_args="$cmake_args -DINTEGRATE_FASTENGINE=ON"
    fi
    
    if [ "$sanitizer" = "asan" ]; then
        cmake_args="$cmake_args -DENABLE_ASAN=ON"
    elif [ "$sanitizer" = "tsan" ]; then
        cmake_args="$cmake_args -DENABLE_TSAN=ON"
    elif [ "$sanitizer" = "ubsan" ]; then
        cmake_args="$cmake_args -DENABLE_UBSAN=ON"
    elif [ "$sanitizer" = "msan" ]; then
        cmake_args="$cmake_args -DENABLE_MSAN=ON"
    fi
    
    if [ "$test_type" = "coverage" ]; then
        cmake_args="$cmake_args -DENABLE_COVERAGE=ON"
    fi
    
    # Конфигурация
    cmake -S ../tests -B . $cmake_args
    
    # Сборка
    make -j$(nproc)
    
    cd ..
    print_success "Тесты собраны: $test_type"
}

# Функция для запуска unit тестов
run_unit_tests() {
    print_message "Запуск unit тестов..."
    
    cd build_advanced_tests
    ./UnitTests --gtest_output="xml:../test_results/unit_test_report.xml"
    cd ..
    
    print_success "Unit тесты завершены"
}

# Функция для запуска интеграционных тестов
run_integration_tests() {
    print_message "Запуск интеграционных тестов..."
    
    cd build_advanced_tests
    
    if [ -f "./IntegrationTests" ]; then
        ./IntegrationTests --gtest_output="xml:../test_results/integration_test_report.xml"
        print_success "Интеграционные тесты завершены"
    else
        print_warning "Интеграционные тесты не собраны (требуется INTEGRATE_FASTENGINE=ON)"
    fi
    
    cd ..
}

# Функция для запуска тестов производительности
run_performance_tests() {
    print_message "Запуск тестов производительности..."
    
    cd build_advanced_tests
    
    if [ -f "./PerformanceTests" ]; then
        ./PerformanceTests --gtest_output="xml:../test_results/performance_test_report.xml"
        print_success "Тесты производительности завершены"
    else
        print_warning "Тесты производительности не собраны (требуется INTEGRATE_FASTENGINE=ON)"
    fi
    
    cd ..
}

# Функция для запуска тестов безопасности
run_security_tests() {
    print_message "Запуск тестов безопасности..."
    
    cd build_advanced_tests
    
    if [ -f "./SecurityTests" ]; then
        ./SecurityTests --gtest_output="xml:../test_results/security_test_report.xml"
        print_success "Тесты безопасности завершены"
    else
        print_warning "Тесты безопасности не собраны (требуется INTEGRATE_FASTENGINE=ON)"
    fi
    
    cd ..
}

# Функция для запуска тестов с санитайзерами
run_sanitizer_tests() {
    local sanitizer=$1
    
    print_message "Запуск тестов с $sanitizer..."
    
    cd build_advanced_tests
    
    # Запускаем тесты с санитайзером
    if [ -f "./UnitTests" ]; then
        ./UnitTests --gtest_output="xml:../sanitizer_reports/${sanitizer}_test_report.xml" 2>&1 | tee ../sanitizer_reports/${sanitizer}_output.log
    fi
    
    if [ -f "./IntegrationTests" ]; then
        ./IntegrationTests --gtest_output="xml:../sanitizer_reports/${sanitizer}_integration_report.xml" 2>&1 | tee -a ../sanitizer_reports/${sanitizer}_output.log
    fi
    
    cd ..
    print_success "Тесты с $sanitizer завершены"
}

# Функция для генерации отчета о покрытии
generate_coverage_report() {
    print_message "Генерация отчета о покрытии кода..."
    
    if command -v gcov &> /dev/null && command -v lcov &> /dev/null; then
        cd build_advanced_tests
        
        # Генерируем отчет о покрытии
        lcov --capture --directory . --output-file coverage.info
        lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
        genhtml coverage_filtered.info --output-directory ../coverage_reports
        
        cd ..
        print_success "Отчет о покрытии сгенерирован в coverage_reports/"
    else
        print_warning "gcov или lcov не найдены. Отчет о покрытии не сгенерирован."
    fi
}

# Функция для очистки
cleanup() {
    print_message "Очистка временных файлов..."
    
    rm -rf build_advanced_tests
    rm -rf test_results
    rm -rf coverage_reports
    rm -rf performance_reports
    rm -rf sanitizer_reports
    
    print_success "Очистка завершена"
}

# Функция для показа справки
show_help() {
    echo "Использование: $0 [ОПЦИИ]"
    echo ""
    echo "ОПЦИИ:"
    echo "  --unit              Запустить только unit тесты"
    echo "  --integration       Запустить интеграционные тесты (требует FastEngine)"
    echo "  --performance       Запустить тесты производительности"
    echo "  --security          Запустить тесты безопасности"
    echo "  --asan              Запустить тесты с AddressSanitizer"
    echo "  --tsan              Запустить тесты с ThreadSanitizer"
    echo "  --ubsan             Запустить тесты с UndefinedBehaviorSanitizer"
    echo "  --msan              Запустить тесты с MemorySanitizer"
    echo "  --coverage          Генерировать отчет о покрытии кода"
    echo "  --all               Запустить все тесты"
    echo "  --clean             Очистить временные файлы"
    echo "  --help              Показать эту справку"
    echo ""
    echo "ПРИМЕРЫ:"
    echo "  $0 --unit                    # Только unit тесты"
    echo "  $0 --integration --asan      # Интеграционные тесты с AddressSanitizer"
    echo "  $0 --all --coverage          # Все тесты с отчетом о покрытии"
}

# Основная функция
main() {
    # Проверяем аргументы
    if [ $# -eq 0 ]; then
        show_help
        exit 1
    fi
    
    # Проверяем зависимости
    check_dependencies
    
    # Создаем директории
    create_directories
    
    # Обрабатываем аргументы
    while [[ $# -gt 0 ]]; do
        case $1 in
            --unit)
                build_tests "unit"
                run_unit_tests
                shift
                ;;
            --integration)
                build_tests "integration"
                run_integration_tests
                shift
                ;;
            --performance)
                build_tests "integration"
                run_performance_tests
                shift
                ;;
            --security)
                build_tests "integration"
                run_security_tests
                shift
                ;;
            --asan)
                build_tests "unit" "asan"
                run_sanitizer_tests "asan"
                shift
                ;;
            --tsan)
                build_tests "unit" "tsan"
                run_sanitizer_tests "tsan"
                shift
                ;;
            --ubsan)
                build_tests "unit" "ubsan"
                run_sanitizer_tests "ubsan"
                shift
                ;;
            --msan)
                build_tests "unit" "msan"
                run_sanitizer_tests "msan"
                shift
                ;;
            --coverage)
                build_tests "coverage"
                run_unit_tests
                generate_coverage_report
                shift
                ;;
            --all)
                build_tests "integration"
                run_unit_tests
                run_integration_tests
                run_performance_tests
                run_security_tests
                shift
                ;;
            --clean)
                cleanup
                exit 0
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                print_error "Неизвестная опция: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    print_success "Все тесты завершены!"
}

# Запускаем основную функцию
main "$@"
