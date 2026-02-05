# Makefile для системы тестирования FastEngine
# Автор: FastEngine Team
# Версия: 1.0

# Переменные
BUILD_DIR = build_test
OUTPUT_DIR = test_results
TEST_CONFIG = test_config.json
SCRIPT_DIR = ../scripts

# Цвета для вывода
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# Цели по умолчанию
.PHONY: all help clean setup build test test-memory test-security test-performance test-concurrency test-resilience test-boundary test-unit test-integration test-external test-coverage test-valgrind test-asan test-msan test-tsan test-ubsan

# Показать справку
help:
	@echo "FastEngine Test System"
	@echo "====================="
	@echo ""
	@echo "Доступные команды:"
	@echo "  all              - Запустить все тесты"
	@echo "  setup            - Настроить окружение для тестирования"
	@echo "  build            - Собрать тесты"
	@echo "  test             - Запустить все тесты"
	@echo "  test-memory      - Запустить тесты памяти"
	@echo "  test-security    - Запустить тесты безопасности"
	@echo "  test-performance - Запустить тесты производительности"
	@echo "  test-concurrency - Запустить тесты многозадачности"
	@echo "  test-resilience  - Запустить тесты отказоустойчивости"
	@echo "  test-boundary    - Запустить тесты граничных случаев"
	@echo "  test-unit        - Запустить unit тесты"
	@echo "  test-integration - Запустить интеграционные тесты"
	@echo "  test-external    - Запустить тесты внешних библиотек"
	@echo "  test-coverage    - Запустить тесты с покрытием кода"
	@echo "  test-valgrind    - Запустить тесты с Valgrind"
	@echo "  test-asan        - Запустить тесты с AddressSanitizer"
	@echo "  test-msan        - Запустить тесты с MemorySanitizer"
	@echo "  test-tsan        - Запустить тесты с ThreadSanitizer"
	@echo "  test-ubsan       - Запустить тесты с UndefinedBehaviorSanitizer"
	@echo "  clean            - Очистить файлы сборки"
	@echo "  clean-results    - Очистить результаты тестов"
	@echo "  clean-all        - Очистить все"
	@echo ""

# Настройка окружения
setup:
	@echo "$(BLUE)[INFO]$(NC) Настройка окружения для тестирования..."
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Окружение настроено"

# Сборка тестов
build: setup
	@echo "$(BLUE)[INFO]$(NC) Сборка тестов..."
	@cd $(BUILD_DIR) && cmake .. -DBUILD_TESTS=ON
	@cd $(BUILD_DIR) && make -j$$(nproc)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты собраны"

# Запуск всех тестов
test: build
	@echo "$(BLUE)[INFO]$(NC) Запуск всех тестов..."
	@$(SCRIPT_DIR)/run_tests.sh --all --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Все тесты завершены"

# Тесты памяти
test-memory: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов памяти..."
	@$(SCRIPT_DIR)/run_tests.sh --memory --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты памяти завершены"

# Тесты безопасности
test-security: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов безопасности..."
	@$(SCRIPT_DIR)/run_tests.sh --security --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты безопасности завершены"

# Тесты производительности
test-performance: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов производительности..."
	@$(SCRIPT_DIR)/run_tests.sh --performance --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты производительности завершены"

# Тесты многозадачности
test-concurrency: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов многозадачности..."
	@$(SCRIPT_DIR)/run_tests.sh --concurrency --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты многозадачности завершены"

# Тесты отказоустойчивости
test-resilience: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов отказоустойчивости..."
	@$(SCRIPT_DIR)/run_tests.sh --resilience --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты отказоустойчивости завершены"

# Тесты граничных случаев
test-boundary: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов граничных случаев..."
	@$(SCRIPT_DIR)/run_tests.sh --boundary --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты граничных случаев завершены"

# Unit тесты
test-unit: build
	@echo "$(BLUE)[INFO]$(NC) Запуск unit тестов..."
	@$(SCRIPT_DIR)/run_tests.sh --unit --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Unit тесты завершены"

# Интеграционные тесты
test-integration: build
	@echo "$(BLUE)[INFO]$(NC) Запуск интеграционных тестов..."
	@$(SCRIPT_DIR)/run_tests.sh --integration --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Интеграционные тесты завершены"

# Тесты внешних библиотек
test-external: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов внешних библиотек..."
	@$(SCRIPT_DIR)/run_tests.sh --external --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты внешних библиотек завершены"

# Тесты с покрытием кода
test-coverage: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов с покрытием кода..."
	@$(SCRIPT_DIR)/run_tests.sh --unit --coverage --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты с покрытием кода завершены"

# Тесты с Valgrind
test-valgrind: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов с Valgrind..."
	@$(SCRIPT_DIR)/run_tests.sh --memory --valgrind --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты с Valgrind завершены"

# Тесты с AddressSanitizer
test-asan: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов с AddressSanitizer..."
	@$(SCRIPT_DIR)/run_tests.sh --memory --asan --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты с AddressSanitizer завершены"

# Тесты с MemorySanitizer
test-msan: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов с MemorySanitizer..."
	@$(SCRIPT_DIR)/run_tests.sh --memory --msan --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты с MemorySanitizer завершены"

# Тесты с ThreadSanitizer
test-tsan: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов с ThreadSanitizer..."
	@$(SCRIPT_DIR)/run_tests.sh --concurrency --tsan --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты с ThreadSanitizer завершены"

# Тесты с UndefinedBehaviorSanitizer
test-ubsan: build
	@echo "$(BLUE)[INFO]$(NC) Запуск тестов с UndefinedBehaviorSanitizer..."
	@$(SCRIPT_DIR)/run_tests.sh --all --ubsan --build-dir $(BUILD_DIR) --output-dir $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Тесты с UndefinedBehaviorSanitizer завершены"

# Очистка файлов сборки
clean:
	@echo "$(BLUE)[INFO]$(NC) Очистка файлов сборки..."
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Файлы сборки очищены"

# Очистка результатов тестов
clean-results:
	@echo "$(BLUE)[INFO]$(NC) Очистка результатов тестов..."
	@rm -rf $(OUTPUT_DIR)
	@echo "$(GREEN)[SUCCESS]$(NC) Результаты тестов очищены"

# Очистка всего
clean-all: clean clean-results
	@echo "$(GREEN)[SUCCESS]$(NC) Все очищено"

# Показать статус
status:
	@echo "$(BLUE)[INFO]$(NC) Статус системы тестирования:"
	@echo "  Директория сборки: $(BUILD_DIR)"
	@echo "  Директория результатов: $(OUTPUT_DIR)"
	@echo "  Конфигурация: $(TEST_CONFIG)"
	@if [ -d "$(BUILD_DIR)" ]; then echo "  Сборка: $(GREEN)готова$(NC)"; else echo "  Сборка: $(RED)не готова$(NC)"; fi
	@if [ -d "$(OUTPUT_DIR)" ]; then echo "  Результаты: $(GREEN)доступны$(NC)"; else echo "  Результаты: $(RED)не доступны$(NC)"; fi

# Показать конфигурацию
config:
	@echo "$(BLUE)[INFO]$(NC) Конфигурация тестирования:"
	@cat $(TEST_CONFIG) | jq '.' 2>/dev/null || cat $(TEST_CONFIG)

# Показать результаты
results:
	@echo "$(BLUE)[INFO]$(NC) Результаты тестирования:"
	@if [ -d "$(OUTPUT_DIR)" ]; then \
		echo "  HTML отчет: $(OUTPUT_DIR)/test_report.html"; \
		echo "  XML отчеты: $(OUTPUT_DIR)/*.xml"; \
		echo "  Логи: $(OUTPUT_DIR)/*.log"; \
	else \
		echo "  $(RED)Результаты не найдены$(NC)"; \
	fi

# Показать статистику
stats:
	@echo "$(BLUE)[INFO]$(NC) Статистика тестирования:"
	@if [ -d "$(OUTPUT_DIR)" ]; then \
		echo "  Количество XML файлов: $$(find $(OUTPUT_DIR) -name "*.xml" | wc -l)"; \
		echo "  Количество логов: $$(find $(OUTPUT_DIR) -name "*.log" | wc -l)"; \
		echo "  Размер результатов: $$(du -sh $(OUTPUT_DIR) | cut -f1)"; \
	else \
		echo "  $(RED)Результаты не найдены$(NC)"; \
	fi

# Показать версию
version:
	@echo "FastEngine Test System v1.0"
	@echo "CMake: $$(cmake --version | head -n1)"
	@echo "Make: $$(make --version | head -n1)"
	@echo "GCC: $$(gcc --version | head -n1 2>/dev/null || echo 'Не установлен')"
	@echo "Clang: $$(clang --version | head -n1 2>/dev/null || echo 'Не установлен')"
	@echo "Valgrind: $$(valgrind --version 2>/dev/null || echo 'Не установлен')"

# Показать помощь
all: help










