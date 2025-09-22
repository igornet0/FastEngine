#!/bin/bash

# Скрипт для тестирования конкретного проекта
# Использование: ./test_project.sh <project_path> [options]

set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функция для вывода сообщений
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Проверяем аргументы
if [ $# -eq 0 ]; then
    log_error "Project path not specified"
    echo "Usage: $0 <project_path> [options]"
    echo ""
    echo "Options:"
    echo "  --test-types TYPES    Comma-separated test types (unit,integration,performance,compatibility,stress)"
    echo "  --platforms PLATFORMS Comma-separated platforms (desktop,ios,android)"
    echo "  --iterations N        Number of test iterations (default: 1)"
    echo "  --timeout SECONDS     Test timeout in seconds (default: 30)"
    echo "  --verbose             Verbose output"
    echo "  --no-report           Don't generate HTML report"
    echo "  --output PATH         Output directory for results"
    echo ""
    echo "Examples:"
    echo "  $0 ./projects/basic_game"
    echo "  $0 ./projects/basic_game --test-types unit,performance --platforms desktop,ios"
    echo "  $0 ./projects/sprite_demo --verbose --iterations 5"
    exit 1
fi

PROJECT_PATH="$1"
shift

# Параметры по умолчанию
TEST_TYPES="unit,integration"
PLATFORMS="desktop"
ITERATIONS=1
TIMEOUT=30
VERBOSE=""
NO_REPORT=""
OUTPUT_DIR="./test_results"

# Парсим дополнительные аргументы
while [[ $# -gt 0 ]]; do
    case $1 in
        --test-types)
            TEST_TYPES="$2"
            shift 2
            ;;
        --platforms)
            PLATFORMS="$2"
            shift 2
            ;;
        --iterations)
            ITERATIONS="$2"
            shift 2
            ;;
        --timeout)
            TIMEOUT="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE="--verbose"
            shift
            ;;
        --no-report)
            NO_REPORT="--no-report"
            shift
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        *)
            log_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Проверяем, что проект существует
if [ ! -f "$PROJECT_PATH/project.conf" ]; then
    log_error "Project not found: $PROJECT_PATH"
    log_error "Make sure the project directory contains a project.conf file"
    exit 1
fi

log_info "Testing project: $PROJECT_PATH"
log_info "Test types: $TEST_TYPES"
log_info "Platforms: $PLATFORMS"
log_info "Iterations: $ITERATIONS"
log_info "Timeout: $TIMEOUT seconds"
log_info "Output directory: $OUTPUT_DIR"

# Создаем директорию для результатов
mkdir -p "$OUTPUT_DIR"

# Определяем путь к CLI
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLI_SCRIPT="$SCRIPT_DIR/simulator-cli"

# Запускаем тестирование
log_info "Starting tests..."

"$CLI_SCRIPT" test \
    --project "$PROJECT_PATH" \
    --output "$OUTPUT_DIR" \
    --test-types "$TEST_TYPES" \
    --platforms "$PLATFORMS" \
    --iterations "$ITERATIONS" \
    --timeout "$TIMEOUT" \
    $VERBOSE \
    $NO_REPORT

EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
    log_success "All tests passed!"
else
    log_error "Some tests failed (exit code: $EXIT_CODE)"
fi

# Показываем результаты
if [ -d "$OUTPUT_DIR" ]; then
    log_info "Test results saved in: $OUTPUT_DIR"
    if [ -f "$OUTPUT_DIR"/*.html ]; then
        log_info "HTML report generated: $(ls "$OUTPUT_DIR"/*.html | head -1)"
    fi
fi

exit $EXIT_CODE

