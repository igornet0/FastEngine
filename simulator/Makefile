# Makefile для Project Simulator
# Упрощает использование команд

.PHONY: help build test test-unit test-integration test-performance test-cross-platform test-stress test-all clean install

# Переменные
SIMULATOR_DIR = $(shell pwd)
BUILD_DIR = $(SIMULATOR_DIR)/build/desktop
CLI_SCRIPT = $(SIMULATOR_DIR)/scripts/simulator-cli
TEST_SCRIPT = $(SIMULATOR_DIR)/scripts/test_project.sh
TEST_ALL_SCRIPT = $(SIMULATOR_DIR)/scripts/test_all_projects.sh
CI_SCRIPT = $(SIMULATOR_DIR)/scripts/ci_test.sh

# Цвета для вывода
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

help: ## Показать справку
	@echo "Project Simulator - Available Commands"
	@echo "======================================"
	@echo ""
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "Examples:"
	@echo "  make build                    # Собрать симулятор"
	@echo "  make test-all                 # Протестировать все проекты"
	@echo "  make test-unit                # Запустить unit тесты"
	@echo "  make test PROJECT=./projects/basic_game  # Тестировать конкретный проект"

build: ## Собрать симулятор
	@echo "$(BLUE)[BUILD]$(NC) Building Project Simulator..."
	@cd $(SIMULATOR_DIR) && ./scripts/build_desktop.sh
	@echo "$(GREEN)[SUCCESS]$(NC) Build completed!"

test: ## Тестировать конкретный проект (используйте PROJECT=path)
	@if [ -z "$(PROJECT)" ]; then \
		echo "$(RED)[ERROR]$(NC) PROJECT not specified. Use: make test PROJECT=./projects/basic_game"; \
		exit 1; \
	fi
	@echo "$(BLUE)[TEST]$(NC) Testing project: $(PROJECT)"
	@$(TEST_SCRIPT) $(PROJECT) $(ARGS)

test-unit: ## Запустить unit тесты
	@echo "$(BLUE)[TEST]$(NC) Running unit tests..."
	@$(TEST_ALL_SCRIPT) --test-types unit --verbose

test-integration: ## Запустить интеграционные тесты
	@echo "$(BLUE)[TEST]$(NC) Running integration tests..."
	@$(TEST_ALL_SCRIPT) --test-types unit,integration --verbose

test-performance: ## Запустить тесты производительности
	@echo "$(BLUE)[TEST]$(NC) Running performance tests..."
	@$(TEST_ALL_SCRIPT) --test-types performance --iterations 3 --verbose

test-cross-platform: ## Запустить кроссплатформенные тесты
	@echo "$(BLUE)[TEST]$(NC) Running cross-platform tests..."
	@$(TEST_ALL_SCRIPT) --test-types unit,integration,compatibility --platforms desktop,ios,android --verbose

test-stress: ## Запустить стресс-тесты
	@echo "$(BLUE)[TEST]$(NC) Running stress tests..."
	@$(TEST_ALL_SCRIPT) --test-types stress --iterations 5 --timeout 120 --verbose

test-all: ## Протестировать все проекты
	@echo "$(BLUE)[TEST]$(NC) Running all tests..."
	@$(TEST_ALL_SCRIPT) --test-types unit,integration,performance --verbose

test-ci: ## Запустить CI тесты
	@echo "$(BLUE)[CI]$(NC) Running CI tests..."
	@$(CI_SCRIPT) --test-types unit,integration,performance --verbose

test-config: ## Запустить тесты с конфигурацией (используйте CONFIG=config_file)
	@if [ -z "$(CONFIG)" ]; then \
		echo "$(RED)[ERROR]$(NC) CONFIG not specified. Use: make test-config CONFIG=unit_tests"; \
		exit 1; \
	fi
	@echo "$(BLUE)[TEST]$(NC) Running tests with config: $(CONFIG)"
	@$(CLI_SCRIPT) test --config $(SIMULATOR_DIR)/configs/$(CONFIG).conf $(ARGS)

simulate: ## Запустить симуляцию проекта (используйте PROJECT=path)
	@if [ -z "$(PROJECT)" ]; then \
		echo "$(RED)[ERROR]$(NC) PROJECT not specified. Use: make simulate PROJECT=./projects/basic_game"; \
		exit 1; \
	fi
	@echo "$(BLUE)[SIMULATE]$(NC) Simulating project: $(PROJECT)"
	@$(CLI_SCRIPT) simulate --project $(PROJECT) $(ARGS)

validate: ## Валидировать проект (используйте PROJECT=path)
	@if [ -z "$(PROJECT)" ]; then \
		echo "$(RED)[ERROR]$(NC) PROJECT not specified. Use: make validate PROJECT=./projects/basic_game"; \
		exit 1; \
	fi
	@echo "$(BLUE)[VALIDATE]$(NC) Validating project: $(PROJECT)"
	@$(CLI_SCRIPT) validate --project $(PROJECT) $(ARGS)

create: ## Создать новый проект (используйте PROJECT=path)
	@if [ -z "$(PROJECT)" ]; then \
		echo "$(RED)[ERROR]$(NC) PROJECT not specified. Use: make create PROJECT=./projects/my_game"; \
		exit 1; \
	fi
	@echo "$(BLUE)[CREATE]$(NC) Creating project: $(PROJECT)"
	@$(CLI_SCRIPT) create --project $(PROJECT) $(ARGS)

list-projects: ## Показать список доступных проектов
	@echo "$(BLUE)[LIST]$(NC) Available projects:"
	@$(CLI_SCRIPT) --list-projects

list-tests: ## Показать список доступных тестов
	@echo "$(BLUE)[LIST]$(NC) Available test types:"
	@$(CLI_SCRIPT) --list-tests

run-simulator: ## Запустить GUI симулятор
	@echo "$(BLUE)[RUN]$(NC) Starting Project Simulator GUI..."
	@cd $(BUILD_DIR) && ./bin/SimulatorApp

clean: ## Очистить файлы сборки
	@echo "$(BLUE)[CLEAN]$(NC) Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(SIMULATOR_DIR)/test_results
	@echo "$(GREEN)[SUCCESS]$(NC) Clean completed!"

install: ## Установить симулятор в систему
	@echo "$(BLUE)[INSTALL]$(NC) Installing Project Simulator..."
	@sudo cp $(CLI_SCRIPT) /usr/local/bin/simulator-cli
	@sudo chmod +x /usr/local/bin/simulator-cli
	@echo "$(GREEN)[SUCCESS]$(NC) Installation completed!"
	@echo "You can now use 'simulator-cli' from anywhere in the system"

uninstall: ## Удалить симулятор из системы
	@echo "$(BLUE)[UNINSTALL]$(NC) Uninstalling Project Simulator..."
	@sudo rm -f /usr/local/bin/simulator-cli
	@echo "$(GREEN)[SUCCESS]$(NC) Uninstallation completed!"

# Специальные цели для CI/CD
ci-unit: ## CI: Unit тесты
	@$(CI_SCRIPT) --test-types unit --no-report

ci-integration: ## CI: Интеграционные тесты
	@$(CI_SCRIPT) --test-types unit,integration --no-report

ci-performance: ## CI: Тесты производительности
	@$(CI_SCRIPT) --test-types performance --iterations 3 --no-report

ci-full: ## CI: Полный набор тестов
	@$(CI_SCRIPT) --test-types unit,integration,performance --verbose

# Цели для разработки
dev-setup: ## Настроить окружение для разработки
	@echo "$(BLUE)[DEV]$(NC) Setting up development environment..."
	@mkdir -p $(SIMULATOR_DIR)/test_results
	@mkdir -p $(SIMULATOR_DIR)/projects/user_projects
	@echo "$(GREEN)[SUCCESS]$(NC) Development environment ready!"

dev-test: build test-all ## Разработка: собрать и протестировать

dev-clean: clean ## Разработка: очистить все

# Показать информацию о проекте
info: ## Показать информацию о симуляторе
	@echo "Project Simulator Information"
	@echo "============================"
	@echo "Version: 1.0.0"
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "CLI Script: $(CLI_SCRIPT)"
	@echo "Test Script: $(TEST_SCRIPT)"
	@echo "Projects Directory: $(SIMULATOR_DIR)/projects"
	@echo ""
	@echo "Available Commands:"
	@echo "  make help              - Show this help"
	@echo "  make build             - Build simulator"
	@echo "  make test-all          - Test all projects"
	@echo "  make run-simulator     - Run GUI simulator"
	@echo "  make list-projects     - List available projects"

