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

# Функция для показа справки
show_help() {
    echo "Использование: $0 [ОПЦИИ]"
    echo ""
    echo "ОПЦИИ:"
    echo "  -h, --help              Показать эту справку"
    echo "  -t, --test TYPE         Запустить определенный тип тестов"
    echo "  -a, --all               Запустить все тесты (по умолчанию)"
    echo "  -m, --memory            Запустить тесты памяти"
    echo "  -s, --security          Запустить тесты безопасности"
    echo "  -p, --performance       Запустить тесты производительности"
    echo "  -c, --concurrency       Запустить тесты многозадачности"
    echo "  -r, --resilience        Запустить тесты отказоустойчивости"
    echo "  -b, --boundary          Запустить тесты граничных случаев"
    echo "  -u, --unit              Запустить unit тесты"
    echo "  -i, --integration       Запустить интеграционные тесты"
    echo "  -e, --external          Запустить тесты внешних библиотек"
    echo "  --asan                  Включить AddressSanitizer"
    echo "  --msan                  Включить MemorySanitizer"
    echo "  --tsan                  Включить ThreadSanitizer"
    echo "  --ubsan                 Включить UndefinedBehaviorSanitizer"
    echo "  --valgrind              Включить Valgrind"
    echo "  --coverage              Включить покрытие кода"
    echo "  --verbose               Подробный вывод"
    echo "  --build-dir DIR         Директория сборки (по умолчанию: build_test)"
    echo "  --output-dir DIR        Директория для результатов (по умолчанию: test_results)"
    echo ""
    echo "ПРИМЕРЫ:"
    echo "  $0 --all                    # Запустить все тесты"
    echo "  $0 --memory --asan          # Запустить тесты памяти с AddressSanitizer"
    echo "  $0 --performance --verbose  # Запустить тесты производительности с подробным выводом"
    echo "  $0 --unit --coverage        # Запустить unit тесты с покрытием кода"
}

# Параметры по умолчанию
TEST_TYPE="all"
BUILD_DIR="build_test"
OUTPUT_DIR="test_results"
VERBOSE=false
ENABLE_ASAN=false
ENABLE_MSAN=false
ENABLE_TSAN=false
ENABLE_UBSAN=false
ENABLE_VALGRIND=false
ENABLE_COVERAGE=false

# Парсинг аргументов командной строки
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -t|--test)
            TEST_TYPE="$2"
            shift 2
            ;;
        -a|--all)
            TEST_TYPE="all"
            shift
            ;;
        -m|--memory)
            TEST_TYPE="memory"
            shift
            ;;
        -s|--security)
            TEST_TYPE="security"
            shift
            ;;
        -p|--performance)
            TEST_TYPE="performance"
            shift
            ;;
        -c|--concurrency)
            TEST_TYPE="concurrency"
            shift
            ;;
        -r|--resilience)
            TEST_TYPE="resilience"
            shift
            ;;
        -b|--boundary)
            TEST_TYPE="boundary"
            shift
            ;;
        -u|--unit)
            TEST_TYPE="unit"
            shift
            ;;
        -i|--integration)
            TEST_TYPE="integration"
            shift
            ;;
        -e|--external)
            TEST_TYPE="external"
            shift
            ;;
        --asan)
            ENABLE_ASAN=true
            shift
            ;;
        --msan)
            ENABLE_MSAN=true
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
        --valgrind)
            ENABLE_VALGRIND=true
            shift
            ;;
        --coverage)
            ENABLE_COVERAGE=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        *)
            print_error "Неизвестная опция: $1"
            show_help
            exit 1
            ;;
    esac
done

# Создаем директории
mkdir -p "$BUILD_DIR"
mkdir -p "$OUTPUT_DIR"

print_message "Запуск тестов FastEngine"
print_message "Тип тестов: $TEST_TYPE"
print_message "Директория сборки: $BUILD_DIR"
print_message "Директория результатов: $OUTPUT_DIR"

# Переходим в директорию сборки
cd "$BUILD_DIR"

# Настройка CMake
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Debug"
CMAKE_ARGS="$CMAKE_ARGS -DBUILD_TESTS=ON"

if [ "$ENABLE_ASAN" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_ASAN=ON"
    print_message "Включен AddressSanitizer"
fi

if [ "$ENABLE_MSAN" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_MSAN=ON"
    print_message "Включен MemorySanitizer"
fi

if [ "$ENABLE_TSAN" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_TSAN=ON"
    print_message "Включен ThreadSanitizer"
fi

if [ "$ENABLE_UBSAN" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_UBSAN=ON"
    print_message "Включен UndefinedBehaviorSanitizer"
fi

if [ "$ENABLE_VALGRIND" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_VALGRIND=ON"
    print_message "Включен Valgrind"
fi

if [ "$ENABLE_COVERAGE" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_COVERAGE=ON"
    print_message "Включено покрытие кода"
fi

# Конфигурация CMake
print_message "Конфигурация CMake..."
cmake .. $CMAKE_ARGS

# Сборка
print_message "Сборка проекта..."
make -j$(nproc)

# Функция для запуска тестов
run_tests() {
    local test_name="$1"
    local test_executable="$2"
    local output_file="$3"
    
    if [ -f "$test_executable" ]; then
        print_message "Запуск $test_name..."
        
        if [ "$VERBOSE" = true ]; then
            if [ "$ENABLE_VALGRIND" = true ]; then
                valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="$output_file" ./"$test_executable"
            else
                ./"$test_executable" --gtest_output=xml:"$output_file"
            fi
        else
            if [ "$ENABLE_VALGRIND" = true ]; then
                valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="$output_file" ./"$test_executable" > /dev/null 2>&1
            else
                ./"$test_executable" --gtest_output=xml:"$output_file" > /dev/null 2>&1
            fi
        fi
        
        if [ $? -eq 0 ]; then
            print_success "$test_name завершен успешно"
        else
            print_error "$test_name завершен с ошибками"
            return 1
        fi
    else
        print_warning "$test_executable не найден, пропускаем $test_name"
    fi
}

# Запуск тестов в зависимости от типа
case $TEST_TYPE in
    "all")
        print_message "Запуск всех тестов..."
        
        # Unit тесты
        run_tests "Unit Tests" "UnitTests" "../$OUTPUT_DIR/unit_tests.xml"
        
        # Интеграционные тесты
        run_tests "Integration Tests" "IntegrationTests" "../$OUTPUT_DIR/integration_tests.xml"
        
        # Тесты памяти
        run_tests "Memory Tests" "MemoryTests" "../$OUTPUT_DIR/memory_tests.xml"
        
        # Тесты безопасности
        run_tests "Security Tests" "SecurityTests" "../$OUTPUT_DIR/security_tests.xml"
        
        # Тесты производительности
        run_tests "Performance Tests" "PerformanceTests" "../$OUTPUT_DIR/performance_tests.xml"
        
        # Тесты многозадачности
        run_tests "Concurrency Tests" "ConcurrencyTests" "../$OUTPUT_DIR/concurrency_tests.xml"
        
        # Тесты отказоустойчивости
        run_tests "Resilience Tests" "ResilienceTests" "../$OUTPUT_DIR/resilience_tests.xml"
        
        # Тесты граничных случаев
        run_tests "Boundary Tests" "BoundaryTests" "../$OUTPUT_DIR/boundary_tests.xml"
        
        # Тесты внешних библиотек
        run_tests "External Library Tests" "ExternalLibraryTests" "../$OUTPUT_DIR/external_tests.xml"
        ;;
        
    "memory")
        run_tests "Memory Tests" "MemoryTests" "../$OUTPUT_DIR/memory_tests.xml"
        ;;
        
    "security")
        run_tests "Security Tests" "SecurityTests" "../$OUTPUT_DIR/security_tests.xml"
        ;;
        
    "performance")
        run_tests "Performance Tests" "PerformanceTests" "../$OUTPUT_DIR/performance_tests.xml"
        ;;
        
    "concurrency")
        run_tests "Concurrency Tests" "ConcurrencyTests" "../$OUTPUT_DIR/concurrency_tests.xml"
        ;;
        
    "resilience")
        run_tests "Resilience Tests" "ResilienceTests" "../$OUTPUT_DIR/resilience_tests.xml"
        ;;
        
    "boundary")
        run_tests "Boundary Tests" "BoundaryTests" "../$OUTPUT_DIR/boundary_tests.xml"
        ;;
        
    "unit")
        run_tests "Unit Tests" "UnitTests" "../$OUTPUT_DIR/unit_tests.xml"
        ;;
        
    "integration")
        run_tests "Integration Tests" "IntegrationTests" "../$OUTPUT_DIR/integration_tests.xml"
        ;;
        
    "external")
        run_tests "External Library Tests" "ExternalLibraryTests" "../$OUTPUT_DIR/external_tests.xml"
        ;;
        
    *)
        print_error "Неизвестный тип тестов: $TEST_TYPE"
        show_help
        exit 1
        ;;
esac

# Генерация отчета о покрытии кода
if [ "$ENABLE_COVERAGE" = true ]; then
    print_message "Генерация отчета о покрытии кода..."
    make coverage
fi

# Генерация сводного отчета
print_message "Генерация сводного отчета..."
cd "../$OUTPUT_DIR"

# Создаем HTML отчет
cat > test_report.html << EOF
<!DOCTYPE html>
<html>
<head>
    <title>FastEngine Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .test-section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .success { color: green; }
        .error { color: red; }
        .warning { color: orange; }
        .info { color: blue; }
    </style>
</head>
<body>
    <div class="header">
        <h1>FastEngine Test Report</h1>
        <p>Дата: $(date)</p>
        <p>Тип тестов: $TEST_TYPE</p>
        <p>Директория сборки: $BUILD_DIR</p>
    </div>
    
    <div class="test-section">
        <h2>Результаты тестов</h2>
        <p>Все тесты завершены. Подробные результаты доступны в XML файлах.</p>
    </div>
    
    <div class="test-section">
        <h2>Настройки</h2>
        <ul>
            <li>AddressSanitizer: $ENABLE_ASAN</li>
            <li>MemorySanitizer: $ENABLE_MSAN</li>
            <li>ThreadSanitizer: $ENABLE_TSAN</li>
            <li>UndefinedBehaviorSanitizer: $ENABLE_UBSAN</li>
            <li>Valgrind: $ENABLE_VALGRIND</li>
            <li>Покрытие кода: $ENABLE_COVERAGE</li>
        </ul>
    </div>
</body>
</html>
EOF

print_success "Тестирование завершено!"
print_message "Результаты сохранены в директории: $OUTPUT_DIR"
print_message "HTML отчет: $OUTPUT_DIR/test_report.html"

# Возвращаемся в корневую директорию
cd ..

print_message "Все тесты завершены успешно!"


