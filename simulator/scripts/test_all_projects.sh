#!/bin/bash

# Скрипт для тестирования всех проектов
# Использование: ./test_all_projects.sh [options]

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

# Параметры по умолчанию
TEST_TYPES="unit,integration"
PLATFORMS="desktop"
ITERATIONS=1
TIMEOUT=30
VERBOSE=""
NO_REPORT=""
OUTPUT_DIR="./test_results"
PROJECTS_DIR="./projects/sample_projects"

# Парсим аргументы
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
        --projects-dir)
            PROJECTS_DIR="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --test-types TYPES    Comma-separated test types (unit,integration,performance,compatibility,stress)"
            echo "  --platforms PLATFORMS Comma-separated platforms (desktop,ios,android)"
            echo "  --iterations N        Number of test iterations (default: 1)"
            echo "  --timeout SECONDS     Test timeout in seconds (default: 30)"
            echo "  --verbose             Verbose output"
            echo "  --no-report           Don't generate HTML report"
            echo "  --output PATH         Output directory for results (default: ./test_results)"
            echo "  --projects-dir PATH   Projects directory (default: ./projects/sample_projects)"
            echo ""
            echo "Examples:"
            echo "  $0"
            echo "  $0 --test-types unit,performance --platforms desktop,ios"
            echo "  $0 --verbose --iterations 3 --output ./my_results"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

log_info "Testing all projects in: $PROJECTS_DIR"
log_info "Test types: $TEST_TYPES"
log_info "Platforms: $PLATFORMS"
log_info "Iterations: $ITERATIONS"
log_info "Timeout: $TIMEOUT seconds"
log_info "Output directory: $OUTPUT_DIR"

# Проверяем, что директория проектов существует
if [ ! -d "$PROJECTS_DIR" ]; then
    log_error "Projects directory not found: $PROJECTS_DIR"
    exit 1
fi

# Создаем директорию для результатов
mkdir -p "$OUTPUT_DIR"

# Определяем путь к CLI
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLI_SCRIPT="$SCRIPT_DIR/simulator-cli"

# Находим все проекты
PROJECTS=()
while IFS= read -r -d '' project; do
    PROJECTS+=("$project")
done < <(find "$PROJECTS_DIR" -name "project.conf" -print0)

if [ ${#PROJECTS[@]} -eq 0 ]; then
    log_warning "No projects found in $PROJECTS_DIR"
    exit 0
fi

log_info "Found ${#PROJECTS[@]} projects to test"

# Счетчики
TOTAL_PROJECTS=${#PROJECTS[@]}
PASSED_PROJECTS=0
FAILED_PROJECTS=0
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Создаем файл для сводного отчета
SUMMARY_FILE="$OUTPUT_DIR/summary.txt"
echo "Project Simulator - Batch Test Summary" > "$SUMMARY_FILE"
echo "======================================" >> "$SUMMARY_FILE"
echo "Date: $(date)" >> "$SUMMARY_FILE"
echo "Test types: $TEST_TYPES" >> "$SUMMARY_FILE"
echo "Platforms: $PLATFORMS" >> "$SUMMARY_FILE"
echo "Iterations: $ITERATIONS" >> "$SUMMARY_FILE"
echo "Timeout: $TIMEOUT seconds" >> "$SUMMARY_FILE"
echo "" >> "$SUMMARY_FILE"

# Тестируем каждый проект
for project_path in "${PROJECTS[@]}"; do
    project_dir=$(dirname "$project_path")
    project_name=$(basename "$project_dir")
    
    log_info "Testing project: $project_name"
    echo "----------------------------------------" >> "$SUMMARY_FILE"
    echo "Project: $project_name" >> "$SUMMARY_FILE"
    echo "Path: $project_dir" >> "$SUMMARY_FILE"
    
    # Запускаем тестирование
    project_output_dir="$OUTPUT_DIR/$project_name"
    mkdir -p "$project_output_dir"
    
    if "$CLI_SCRIPT" test \
        --project "$project_dir" \
        --output "$project_output_dir" \
        --test-types "$TEST_TYPES" \
        --platforms "$PLATFORMS" \
        --iterations "$ITERATIONS" \
        --timeout "$TIMEOUT" \
        $VERBOSE \
        $NO_REPORT > "$project_output_dir/test_output.log" 2>&1; then
        
        log_success "Project $project_name: PASSED"
        echo "Status: PASSED" >> "$SUMMARY_FILE"
        ((PASSED_PROJECTS++))
    else
        log_error "Project $project_name: FAILED"
        echo "Status: FAILED" >> "$SUMMARY_FILE"
        ((FAILED_PROJECTS++))
    fi
    
    # Подсчитываем тесты из лога
    if [ -f "$project_output_dir/test_output.log" ]; then
        project_passed=$(grep -c "✓" "$project_output_dir/test_output.log" || echo "0")
        project_failed=$(grep -c "✗" "$project_output_dir/test_output.log" || echo "0")
        
        echo "Tests: $project_passed passed, $project_failed failed" >> "$SUMMARY_FILE"
        
        ((TOTAL_TESTS += project_passed + project_failed))
        ((PASSED_TESTS += project_passed))
        ((FAILED_TESTS += project_failed))
    fi
    
    echo "" >> "$SUMMARY_FILE"
done

# Выводим общую статистику
log_info "Batch test completed!"
echo ""
log_info "Project Statistics:"
log_info "  Total Projects: $TOTAL_PROJECTS"
log_success "  Passed: $PASSED_PROJECTS"
if [ $FAILED_PROJECTS -gt 0 ]; then
    log_error "  Failed: $FAILED_PROJECTS"
else
    log_info "  Failed: $FAILED_PROJECTS"
fi
log_info "  Success Rate: $((PASSED_PROJECTS * 100 / TOTAL_PROJECTS))%"

echo ""
log_info "Test Statistics:"
log_info "  Total Tests: $TOTAL_TESTS"
log_success "  Passed: $PASSED_TESTS"
if [ $FAILED_TESTS -gt 0 ]; then
    log_error "  Failed: $FAILED_TESTS"
else
    log_info "  Failed: $FAILED_TESTS"
fi
if [ $TOTAL_TESTS -gt 0 ]; then
    log_info "  Success Rate: $((PASSED_TESTS * 100 / TOTAL_TESTS))%"
fi

# Добавляем общую статистику в файл
echo "" >> "$SUMMARY_FILE"
echo "Overall Statistics:" >> "$SUMMARY_FILE"
echo "==================" >> "$SUMMARY_FILE"
echo "Total Projects: $TOTAL_PROJECTS" >> "$SUMMARY_FILE"
echo "Passed Projects: $PASSED_PROJECTS" >> "$SUMMARY_FILE"
echo "Failed Projects: $FAILED_PROJECTS" >> "$SUMMARY_FILE"
echo "Project Success Rate: $((PASSED_PROJECTS * 100 / TOTAL_PROJECTS))%" >> "$SUMMARY_FILE"
echo "" >> "$SUMMARY_FILE"
echo "Total Tests: $TOTAL_TESTS" >> "$SUMMARY_FILE"
echo "Passed Tests: $PASSED_TESTS" >> "$SUMMARY_FILE"
echo "Failed Tests: $FAILED_TESTS" >> "$SUMMARY_FILE"
if [ $TOTAL_TESTS -gt 0 ]; then
    echo "Test Success Rate: $((PASSED_TESTS * 100 / TOTAL_TESTS))%" >> "$SUMMARY_FILE"
fi

log_info "Summary report saved: $SUMMARY_FILE"
log_info "Detailed results in: $OUTPUT_DIR"

# Возвращаем код ошибки если есть неудачные проекты
if [ $FAILED_PROJECTS -gt 0 ]; then
    exit 1
else
    exit 0
fi

