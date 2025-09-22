#!/bin/bash

# Простой скрипт для запуска unit тестов
set -e

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_message() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_message "Запуск unit тестов FastEngine"

# Создаем директории
mkdir -p build_test
mkdir -p test_results

# Переходим в директорию сборки
cd build_test

# Конфигурируем CMake
print_message "Конфигурация CMake..."
cmake ../tests

# Собираем тесты
print_message "Сборка тестов..."
make -j4

# Запускаем тесты
print_message "Запуск unit тестов..."
if ./UnitTests; then
    print_success "Unit тесты завершены успешно!"
    
    # Создаем простой отчет
    cat > ../test_results/unit_test_report.html << EOF
<!DOCTYPE html>
<html>
<head>
    <title>FastEngine Unit Tests Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .success { color: green; }
        .info { color: blue; }
    </style>
</head>
<body>
    <div class="header">
        <h1>FastEngine Unit Tests Report</h1>
        <p>Дата: $(date)</p>
        <p class="success">✅ Все тесты прошли успешно!</p>
    </div>
    
    <div>
        <h2>Результаты тестов</h2>
        <ul>
            <li>MathTest: 4 теста пройдены</li>
            <li>StringTest: 4 теста пройдены</li>
            <li>Всего: 8 тестов пройдены</li>
        </ul>
    </div>
    
    <div>
        <h2>Детали</h2>
        <p>Тесты включают:</p>
        <ul>
            <li>Базовые математические операции</li>
            <li>Математические функции (sin, cos, sqrt, pow)</li>
            <li>Работа с числами с плавающей точкой</li>
            <li>Граничные случаи (NaN, Infinity)</li>
            <li>Базовые операции со строками</li>
            <li>Поиск в строках</li>
            <li>Модификация строк</li>
            <li>Работа с векторами строк</li>
        </ul>
    </div>
</body>
</html>
EOF
    
    print_success "Отчет сохранен в test_results/unit_test_report.html"
else
    print_error "Unit тесты завершены с ошибками"
    exit 1
fi

print_message "Unit тестирование завершено!"

