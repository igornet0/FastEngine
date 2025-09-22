#!/bin/bash

# Скрипт для CI/CD интеграции
# Автоматически тестирует проекты и генерирует отчеты

set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Функция для вывода сообщений
log_info() {
    echo -e "${BLUE}[CI-INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[CI-SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[CI-WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[CI-ERROR]${NC} $1"
}

# Параметры по умолчанию
TEST_TYPES="unit,integration,performance"
PLATFORMS="desktop"
ITERATIONS=1
TIMEOUT=60
VERBOSE=""
OUTPUT_DIR="./ci_test_results"
PROJECTS_DIR="./projects/sample_projects"
GENERATE_REPORT=true
UPLOAD_RESULTS=false
WEBHOOK_URL=""

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
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --projects-dir)
            PROJECTS_DIR="$2"
            shift 2
            ;;
        --no-report)
            GENERATE_REPORT=false
            shift
            ;;
        --upload-results)
            UPLOAD_RESULTS=true
            shift
            ;;
        --webhook-url)
            WEBHOOK_URL="$2"
            shift 2
            ;;
        --help|-h)
            echo "CI/CD Test Script for Project Simulator"
            echo "======================================"
            echo ""
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  --test-types TYPES    Comma-separated test types (default: unit,integration,performance)"
            echo "  --platforms PLATFORMS Comma-separated platforms (default: desktop)"
            echo "  --iterations N        Number of test iterations (default: 1)"
            echo "  --timeout SECONDS     Test timeout in seconds (default: 60)"
            echo "  --verbose             Verbose output"
            echo "  --output PATH         Output directory for results (default: ./ci_test_results)"
            echo "  --projects-dir PATH   Projects directory (default: ./projects/sample_projects)"
            echo "  --no-report           Don't generate HTML report"
            echo "  --upload-results      Upload results to external service"
            echo "  --webhook-url URL     Send results to webhook"
            echo ""
            echo "Environment Variables:"
            echo "  CI=true               Enable CI mode (more verbose output)"
            echo "  BUILD_NUMBER          Build number for reports"
            echo "  GIT_COMMIT            Git commit hash for reports"
            echo "  GIT_BRANCH            Git branch for reports"
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# CI режим
if [ "$CI" = "true" ]; then
    log_info "Running in CI mode"
    VERBOSE="--verbose"
fi

log_info "Starting CI test run"
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

# Проверяем, что CLI доступен
if [ ! -f "$CLI_SCRIPT" ]; then
    log_error "CLI script not found: $CLI_SCRIPT"
    log_info "Building simulator..."
    cd "$(dirname "$SCRIPT_DIR")"
    ./scripts/build_desktop.sh
    if [ $? -ne 0 ]; then
        log_error "Failed to build simulator"
        exit 1
    fi
fi

# Создаем файл для CI отчета
CI_REPORT="$OUTPUT_DIR/ci_report.json"
echo "{" > "$CI_REPORT"
echo "  \"timestamp\": \"$(date -u +%Y-%m-%dT%H:%M:%SZ)\"," >> "$CI_REPORT"
echo "  \"build_number\": \"${BUILD_NUMBER:-unknown}\"," >> "$CI_REPORT"
echo "  \"git_commit\": \"${GIT_COMMIT:-unknown}\"," >> "$CI_REPORT"
echo "  \"git_branch\": \"${GIT_BRANCH:-unknown}\"," >> "$CI_REPORT"
echo "  \"test_types\": \"$TEST_TYPES\"," >> "$CI_REPORT"
echo "  \"platforms\": \"$PLATFORMS\"," >> "$CI_REPORT"
echo "  \"iterations\": $ITERATIONS," >> "$CI_REPORT"
echo "  \"timeout\": $TIMEOUT," >> "$CI_REPORT"
echo "  \"projects\": [" >> "$CI_REPORT"

# Находим все проекты
PROJECTS=()
while IFS= read -r -d '' project; do
    PROJECTS+=("$project")
done < <(find "$PROJECTS_DIR" -name "project.conf" -print0)

if [ ${#PROJECTS[@]} -eq 0 ]; then
    log_warning "No projects found in $PROJECTS_DIR"
    echo "  ]" >> "$CI_REPORT"
    echo "}" >> "$CI_REPORT"
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

# Тестируем каждый проект
for i in "${!PROJECTS[@]}"; do
    project_path="${PROJECTS[$i]}"
    project_dir=$(dirname "$project_path")
    project_name=$(basename "$project_dir")
    
    log_info "Testing project $((i+1))/${#PROJECTS[@]}: $project_name"
    
    # Запускаем тестирование
    project_output_dir="$OUTPUT_DIR/$project_name"
    mkdir -p "$project_output_dir"
    
    # Добавляем информацию о проекте в JSON
    if [ $i -gt 0 ]; then
        echo "," >> "$CI_REPORT"
    fi
    echo "    {" >> "$CI_REPORT"
    echo "      \"name\": \"$project_name\"," >> "$CI_REPORT"
    echo "      \"path\": \"$project_dir\"," >> "$CI_REPORT"
    
    if "$CLI_SCRIPT" test \
        --project "$project_dir" \
        --output "$project_output_dir" \
        --test-types "$TEST_TYPES" \
        --platforms "$PLATFORMS" \
        --iterations "$ITERATIONS" \
        --timeout "$TIMEOUT" \
        $VERBOSE > "$project_output_dir/test_output.log" 2>&1; then
        
        log_success "Project $project_name: PASSED"
        echo "      \"status\": \"passed\"," >> "$CI_REPORT"
        ((PASSED_PROJECTS++))
    else
        log_error "Project $project_name: FAILED"
        echo "      \"status\": \"failed\"," >> "$CI_REPORT"
        ((FAILED_PROJECTS++))
    fi
    
    # Подсчитываем тесты из лога
    project_passed=0
    project_failed=0
    if [ -f "$project_output_dir/test_output.log" ]; then
        project_passed=$(grep -c "✓" "$project_output_dir/test_output.log" || echo "0")
        project_failed=$(grep -c "✗" "$project_output_dir/test_output.log" || echo "0")
    fi
    
    echo "      \"tests_passed\": $project_passed," >> "$CI_REPORT"
    echo "      \"tests_failed\": $project_failed" >> "$CI_REPORT"
    echo "    }" >> "$CI_REPORT"
    
    ((TOTAL_TESTS += project_passed + project_failed))
    ((PASSED_TESTS += project_passed))
    ((FAILED_TESTS += project_failed))
done

# Завершаем JSON
echo "  ]," >> "$CI_REPORT"
echo "  \"summary\": {" >> "$CI_REPORT"
echo "    \"total_projects\": $TOTAL_PROJECTS," >> "$CI_REPORT"
echo "    \"passed_projects\": $PASSED_PROJECTS," >> "$CI_REPORT"
echo "    \"failed_projects\": $FAILED_PROJECTS," >> "$CI_REPORT"
echo "    \"project_success_rate\": $((PASSED_PROJECTS * 100 / TOTAL_PROJECTS))," >> "$CI_REPORT"
echo "    \"total_tests\": $TOTAL_TESTS," >> "$CI_REPORT"
echo "    \"passed_tests\": $PASSED_TESTS," >> "$CI_REPORT"
echo "    \"failed_tests\": $FAILED_TESTS," >> "$CI_REPORT"
if [ $TOTAL_TESTS -gt 0 ]; then
    echo "    \"test_success_rate\": $((PASSED_TESTS * 100 / TOTAL_TESTS))" >> "$CI_REPORT"
else
    echo "    \"test_success_rate\": 0" >> "$CI_REPORT"
fi
echo "  }" >> "$CI_REPORT"
echo "}" >> "$CI_REPORT"

# Выводим общую статистику
log_info "CI test run completed!"
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

# Генерируем HTML отчет если нужно
if [ "$GENERATE_REPORT" = true ]; then
    log_info "Generating HTML report..."
    if [ -f "$OUTPUT_DIR"/*.html ]; then
        log_success "HTML report generated: $(ls "$OUTPUT_DIR"/*.html | head -1)"
    fi
fi

# Отправляем результаты в webhook если указан
if [ -n "$WEBHOOK_URL" ]; then
    log_info "Sending results to webhook..."
    if command -v curl >/dev/null 2>&1; then
        curl -X POST -H "Content-Type: application/json" \
             -d @"$CI_REPORT" \
             "$WEBHOOK_URL" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            log_success "Results sent to webhook"
        else
            log_warning "Failed to send results to webhook"
        fi
    else
        log_warning "curl not available, cannot send webhook"
    fi
fi

log_info "CI report saved: $CI_REPORT"
log_info "Detailed results in: $OUTPUT_DIR"

# Возвращаем код ошибки если есть неудачные проекты
if [ $FAILED_PROJECTS -gt 0 ]; then
    log_error "CI test run failed: $FAILED_PROJECTS projects failed"
    exit 1
else
    log_success "CI test run passed: all projects successful"
    exit 0
fi

