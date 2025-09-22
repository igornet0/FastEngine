#!/bin/bash

# Демонстрационный скрипт для Project Simulator CLI
# Показывает возможности командной строки

set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
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

log_demo() {
    echo -e "${PURPLE}[DEMO]${NC} $1"
}

log_command() {
    echo -e "${CYAN}[COMMAND]${NC} $1"
}

# Проверяем, что мы в правильной директории
if [ ! -f "CMakeLists.txt" ] || [ ! -d "src" ]; then
    log_error "Please run this script from the simulator directory"
    exit 1
fi

echo -e "${PURPLE}🚀 Project Simulator CLI Demo${NC}"
echo "=================================="
echo ""

# Шаг 1: Сборка CLI
log_demo "Step 1: Building Project Simulator CLI"
echo "----------------------------------------"

if [ ! -f "scripts/simulator-cli" ]; then
    log_info "CLI wrapper not found, creating..."
    # CLI wrapper уже создан
fi

if [ ! -f "build/desktop/bin/SimulatorCLI" ]; then
    log_info "Building simulator..."
    make build
    if [ $? -ne 0 ]; then
        log_error "Build failed!"
        exit 1
    fi
else
    log_success "CLI already built"
fi

echo ""

# Шаг 2: Показываем справку
log_demo "Step 2: CLI Help"
echo "-----------------"
log_command "./scripts/simulator-cli --help"
echo ""
./scripts/simulator-cli --help
echo ""

# Шаг 3: Список проектов
log_demo "Step 3: List Available Projects"
echo "--------------------------------"
log_command "./scripts/simulator-cli --list-projects"
echo ""
./scripts/simulator-cli --list-projects
echo ""

# Шаг 4: Список тестов
log_demo "Step 4: List Available Test Types"
echo "-----------------------------------"
log_command "./scripts/simulator-cli --list-tests"
echo ""
./scripts/simulator-cli --list-tests
echo ""

# Шаг 5: Тестирование проекта
log_demo "Step 5: Test a Project"
echo "----------------------"

# Находим первый доступный проект
PROJECT_PATH=""
for project in projects/sample_projects/*/; do
    if [ -f "${project}project.conf" ]; then
        PROJECT_PATH="$project"
        break
    fi
done

if [ -n "$PROJECT_PATH" ]; then
    PROJECT_NAME=$(basename "$PROJECT_PATH")
    log_info "Testing project: $PROJECT_NAME"
    log_command "./scripts/simulator-cli test -p $PROJECT_PATH -t unit -v"
    echo ""
    
    # Запускаем тест
    ./scripts/simulator-cli test -p "$PROJECT_PATH" -t unit -v
    echo ""
    
    log_success "Test completed!"
else
    log_warning "No projects found for testing"
fi

echo ""

# Шаг 6: Валидация проекта
log_demo "Step 6: Validate a Project"
echo "---------------------------"

if [ -n "$PROJECT_PATH" ]; then
    log_info "Validating project: $PROJECT_NAME"
    log_command "./scripts/simulator-cli validate -p $PROJECT_PATH -v"
    echo ""
    
    ./scripts/simulator-cli validate -p "$PROJECT_PATH" -v
    echo ""
    
    log_success "Validation completed!"
else
    log_warning "No projects found for validation"
fi

echo ""

# Шаг 7: Пакетное тестирование
log_demo "Step 7: Batch Testing"
echo "----------------------"
log_info "Running batch tests on all projects..."
log_command "./scripts/simulator-cli batch-test -t unit,integration --platforms desktop"
echo ""

./scripts/simulator-cli batch-test -t unit,integration --platforms desktop
echo ""

log_success "Batch testing completed!"
echo ""

# Шаг 8: Использование конфигураций
log_demo "Step 8: Using Configuration Files"
echo "----------------------------------"

if [ -d "configs" ]; then
    log_info "Available configurations:"
    ls -la configs/*.conf 2>/dev/null || log_warning "No configuration files found"
    echo ""
    
    if [ -f "configs/unit_tests.conf" ]; then
        log_info "Testing with unit_tests.conf configuration..."
        log_command "./scripts/simulator-cli test -c configs/unit_tests.conf"
        echo ""
        
        # Обновляем путь к проекту в конфигурации
        if [ -n "$PROJECT_PATH" ]; then
            sed -i.bak "s|project_path=.*|project_path=$PROJECT_PATH|" configs/unit_tests.conf
            ./scripts/simulator-cli test -c configs/unit_tests.conf
            # Восстанавливаем оригинальный файл
            mv configs/unit_tests.conf.bak configs/unit_tests.conf 2>/dev/null || true
        fi
        echo ""
        
        log_success "Configuration-based testing completed!"
    fi
else
    log_warning "Configurations directory not found"
fi

echo ""

# Шаг 9: Использование Makefile
log_demo "Step 9: Using Makefile"
echo "------------------------"
log_info "Makefile provides convenient shortcuts:"
echo ""
log_command "make help"
echo ""
make help | head -20
echo "..."
echo ""

log_info "Testing with Makefile:"
log_command "make test-unit"
echo ""
make test-unit
echo ""

log_success "Makefile testing completed!"
echo ""

# Шаг 10: CI/CD демонстрация
log_demo "Step 10: CI/CD Integration"
echo "---------------------------"
log_info "CI/CD script provides automated testing:"
log_command "./scripts/ci_test.sh --test-types unit,integration --verbose"
echo ""

./scripts/ci_test.sh --test-types unit,integration --verbose
echo ""

log_success "CI/CD testing completed!"
echo ""

# Шаг 11: Показываем результаты
log_demo "Step 11: Test Results"
echo "----------------------"

if [ -d "test_results" ]; then
    log_info "Test results generated in:"
    find test_results -name "*.html" -o -name "*.json" -o -name "*.txt" | head -10
    echo ""
    
    if [ -f "test_results"/*.html ]; then
        log_success "HTML report generated: $(ls test_results/*.html | head -1)"
    fi
    
    if [ -f "ci_test_results/ci_report.json" ]; then
        log_success "CI report generated: ci_test_results/ci_report.json"
    fi
else
    log_warning "No test results found"
fi

echo ""

# Заключение
log_demo "Demo Complete!"
echo "==============="
echo ""
log_success "Project Simulator CLI is working correctly!"
echo ""
log_info "Key features demonstrated:"
echo "  ✓ Command line interface"
echo "  ✓ Project testing"
echo "  ✓ Project validation"
echo "  ✓ Batch testing"
echo "  ✓ Configuration files"
echo "  ✓ Makefile integration"
echo "  ✓ CI/CD integration"
echo "  ✓ HTML and JSON reports"
echo ""
log_info "Next steps:"
echo "  1. Try different test types: make test-performance"
echo "  2. Test on different platforms: --platforms desktop,ios,android"
echo "  3. Create your own projects: make create PROJECT=./projects/my_game"
echo "  4. Integrate with your CI/CD pipeline"
echo ""
log_info "For more information:"
echo "  - CLI Usage: cat CLI_USAGE.md"
echo "  - Full Documentation: cat README.md"
echo "  - Examples: ls projects/sample_projects/"
echo ""
log_success "Happy testing! 🚀"

