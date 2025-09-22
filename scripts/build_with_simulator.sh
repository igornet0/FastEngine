#!/bin/bash

# Скрипт для сборки FastEngine с Project Simulator
# Собирает движок и симулятор для всех поддерживаемых платформ

set -e

echo "=== Building FastEngine with Project Simulator ==="

# Определяем платформу
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="Linux"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    PLATFORM="Windows"
else
    echo "Unsupported platform: $OSTYPE"
    exit 1
fi

echo "Platform: $PLATFORM"

# Создаем директорию для сборки
mkdir -p build/with_simulator
cd build/with_simulator

# Конфигурируем проект с симулятором
echo "Configuring project with simulator..."
cmake ../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SIMULATOR=ON \
    -DBUILD_DESKTOP=ON

# Собираем проект
echo "Building project..."
if [[ "$PLATFORM" == "Windows" ]]; then
    cmake --build . --config Release
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

echo "Build completed successfully!"

# Копируем ассеты симулятора
echo "Copying simulator assets..."
mkdir -p bin/projects
cp -r ../../simulator/projects/sample_projects bin/projects/

# Создаем символические ссылки на исполняемые файлы
echo "Creating executables..."
ln -sf bin/SimulatorApp simulator
ln -sf bin/ExampleApp example

echo "=== FastEngine with Project Simulator Build Complete ==="
echo ""
echo "Available executables:"
echo "  - ./simulator    - Project Simulator"
echo "  - ./example      - Example FastEngine app"
echo ""
echo "Sample projects available in: bin/projects/"
echo ""
echo "To run simulator:"
echo "  cd build/with_simulator"
echo "  ./simulator"
echo ""
echo "To run example:"
echo "  cd build/with_simulator"
echo "  ./example"

