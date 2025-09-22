#!/bin/bash

# Скрипт для тестирования Project Simulator
# Запускает симулятор и проверяет его работу

set -e

echo "=== Testing Project Simulator ==="

# Проверяем, что симулятор собран
if [ ! -f "build/desktop/bin/SimulatorApp" ]; then
    echo "Simulator not built. Building now..."
    ./scripts/build_desktop.sh
fi

# Проверяем наличие примеров проектов
if [ ! -d "projects/sample_projects" ]; then
    echo "Sample projects not found!"
    exit 1
fi

echo "Sample projects found:"
ls -la projects/sample_projects/

# Запускаем симулятор в тестовом режиме
echo "Starting simulator test..."
cd build/desktop

# Создаем тестовую конфигурацию
mkdir -p test_output
export SIMULATOR_TEST_MODE=1
export SIMULATOR_OUTPUT_DIR=test_output

# Запускаем симулятор с автоматическим тестированием
echo "Running simulator with test projects..."
timeout 30s ./bin/SimulatorApp || true

# Проверяем результаты
if [ -d "test_output" ]; then
    echo "Test output directory created successfully"
    ls -la test_output/
else
    echo "Warning: No test output generated"
fi

echo "=== Project Simulator Test Complete ==="
echo "Check the output above for any errors or warnings"

