#!/bin/bash

# Объединённый скрипт для запуска FastEngine
# Поддерживает запуск редакторов, тестов и сборку проекта

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

# Функция для показа справки
show_help() {
    echo "FastEngine - Объединённый скрипт запуска"
    echo ""
    echo "ИСПОЛЬЗОВАНИЕ: $0 [КОМАНДА] [ОПЦИИ]"
    echo ""
    echo "КОМАНДЫ:"
    echo "  editors              Запустить редакторы"
    echo "  tests                Запустить тесты"
    echo "  build                Собрать проект"
    echo "  clean                Очистить build файлы"
    echo "  help                 Показать эту справку"
    echo ""
    echo "ОПЦИИ ДЛЯ РЕДАКТОРОВ:"
    echo "  --qt                 Запустить Qt редактор"
    echo "  --sdl                Запустить SDL редактор"
    echo "  --advanced           Запустить Advanced редактор"
    echo "  --all                Запустить все редакторы"
    echo ""
    echo "ОПЦИИ ДЛЯ ТЕСТОВ:"
    echo "  --unit               Запустить unit тесты"
    echo "  --integration        Запустить интеграционные тесты"
    echo "  --performance        Запустить тесты производительности"
    echo "  --coverage           Генерировать отчет о покрытии"
    echo "  --all                Запустить все тесты"
    echo ""
    echo "ПРИМЕРЫ:"
    echo "  $0 editors --qt              # Запустить Qt редактор"
    echo "  $0 tests --unit              # Запустить unit тесты"
    echo "  $0 build                     # Собрать проект"
    echo "  $0 clean                     # Очистить build файлы"
}

# Функция для запуска редакторов
run_editors() {
    local editor_type=""
    
    # Обрабатываем опции
    while [[ $# -gt 0 ]]; do
        case $1 in
            --qt)
                editor_type="qt"
                shift
                ;;
            --sdl)
                editor_type="sdl"
                shift
                ;;
            --advanced)
                editor_type="advanced"
                shift
                ;;
            --all)
                editor_type="all"
                shift
                ;;
            *)
                print_error "Неизвестная опция: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    if [ -z "$editor_type" ]; then
        print_message "Выберите редактор для запуска:"
        echo "1) Qt Editor"
        echo "2) SDL Editor"
        echo "3) Advanced Editor"
        echo "4) Все редакторы"
        echo "5) Выход"
        echo ""
        
        read -p "Введите номер (1-5): " choice
        
        case $choice in
            1) editor_type="qt" ;;
            2) editor_type="sdl" ;;
            3) editor_type="advanced" ;;
            4) editor_type="all" ;;
            5) exit 0 ;;
            *) print_error "Неверный выбор"; exit 1 ;;
        esac
    fi
    
    case $editor_type in
        qt)
            print_message "Запуск Qt редактора..."
            cd editors/qt
            if [ -f "qt_editor" ]; then
                ./qt_editor
            else
                print_error "Qt редактор не собран. Запустите: $0 build"
                exit 1
            fi
            ;;
        sdl)
            print_message "Запуск SDL редактора..."
            cd editors/sdl
            if [ -f "SimpleGameEditorSDL" ]; then
                ./SimpleGameEditorSDL
            else
                print_error "SDL редактор не собран. Запустите: $0 build"
                exit 1
            fi
            ;;
        advanced)
            print_message "Запуск Advanced редактора..."
            cd editors/advanced
            if [ -f "AdvancedGameEditor" ]; then
                ./AdvancedGameEditor
            else
                print_error "Advanced редактор не собран. Запустите: $0 build"
                exit 1
            fi
            ;;
        all)
            print_message "Запуск всех редакторов..."
            print_warning "Редакторы будут запущены в фоновом режиме"
            $0 editors --qt &
            $0 editors --sdl &
            $0 editors --advanced &
            wait
            ;;
    esac
}

# Функция для запуска тестов
run_tests() {
    local test_type=""
    
    # Обрабатываем опции
    while [[ $# -gt 0 ]]; do
        case $1 in
            --unit)
                test_type="unit"
                shift
                ;;
            --integration)
                test_type="integration"
                shift
                ;;
            --performance)
                test_type="performance"
                shift
                ;;
            --coverage)
                test_type="coverage"
                shift
                ;;
            --all)
                test_type="all"
                shift
                ;;
            *)
                print_error "Неизвестная опция: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    if [ -z "$test_type" ]; then
        print_message "Выберите тип тестов:"
        echo "1) Unit тесты"
        echo "2) Интеграционные тесты"
        echo "3) Тесты производительности"
        echo "4) Все тесты"
        echo "5) Выход"
        echo ""
        
        read -p "Введите номер (1-5): " choice
        
        case $choice in
            1) test_type="unit" ;;
            2) test_type="integration" ;;
            3) test_type="performance" ;;
            4) test_type="all" ;;
            5) exit 0 ;;
            *) print_error "Неверный выбор"; exit 1 ;;
        esac
    fi
    
    # Создаем директории для тестов
    mkdir -p build_test
    mkdir -p test_results
    
    cd build_test
    
    # Конфигурируем CMake
    print_message "Конфигурация CMake для тестов..."
    cmake ../tests
    
    # Собираем тесты
    print_message "Сборка тестов..."
    make -j4
    
    # Запускаем тесты в зависимости от типа
    case $test_type in
        unit)
            print_message "Запуск unit тестов..."
            if [ -f "./UnitTests" ]; then
                ./UnitTests --gtest_output="xml:../test_results/unit_test_report.xml"
                print_success "Unit тесты завершены"
            else
                print_error "Unit тесты не собраны"
                exit 1
            fi
            ;;
        integration)
            print_message "Запуск интеграционных тестов..."
            if [ -f "./IntegrationTests" ]; then
                ./IntegrationTests --gtest_output="xml:../test_results/integration_test_report.xml"
                print_success "Интеграционные тесты завершены"
            else
                print_error "Интеграционные тесты не собраны"
                exit 1
            fi
            ;;
        performance)
            print_message "Запуск тестов производительности..."
            if [ -f "./PerformanceTests" ]; then
                ./PerformanceTests --gtest_output="xml:../test_results/performance_test_report.xml"
                print_success "Тесты производительности завершены"
            else
                print_error "Тесты производительности не собраны"
                exit 1
            fi
            ;;
        all)
            print_message "Запуск всех тестов..."
            if [ -f "./UnitTests" ]; then
                ./UnitTests --gtest_output="xml:../test_results/unit_test_report.xml"
            fi
            if [ -f "./IntegrationTests" ]; then
                ./IntegrationTests --gtest_output="xml:../test_results/integration_test_report.xml"
            fi
            if [ -f "./PerformanceTests" ]; then
                ./PerformanceTests --gtest_output="xml:../test_results/performance_test_report.xml"
            fi
            print_success "Все тесты завершены"
            ;;
    esac
    
    cd ..
    print_success "Отчеты сохранены в test_results/"
}

# Функция для сборки проекта
build_project() {
    print_message "Сборка FastEngine..."
    
    # Создаем build директорию
    mkdir -p build
    cd build
    
    # Конфигурируем CMake
    print_message "Конфигурация CMake..."
    cmake ..
    
    # Собираем проект
    print_message "Сборка проекта..."
    make -j4
    
    cd ..
    print_success "Проект собран успешно!"
}

# Функция для очистки
clean_project() {
    print_message "Очистка build файлов..."
    
    rm -rf build
    rm -rf build_test
    rm -rf test_results
    rm -rf coverage_reports
    
    print_success "Очистка завершена!"
}

# Основная функция
main() {
    if [ $# -eq 0 ]; then
        show_help
        exit 1
    fi
    
    case $1 in
        editors)
            shift
            run_editors "$@"
            ;;
        tests)
            shift
            run_tests "$@"
            ;;
        build)
            build_project
            ;;
        clean)
            clean_project
            ;;
        help)
            show_help
            ;;
        *)
            print_error "Неизвестная команда: $1"
            show_help
            exit 1
            ;;
    esac
}

# Запускаем основную функцию
main "$@"







