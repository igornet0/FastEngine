#!/bin/bash

# Скрипт для сборки Project Simulator для Desktop платформ
# Поддерживает macOS, Linux, Windows

set -e

echo "=== Building Project Simulator for Desktop ==="

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
mkdir -p build/desktop
cd build/desktop

# Конфигурируем проект
echo "Configuring project..."
cmake ../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_DESKTOP=ON \
    -DCMAKE_INSTALL_PREFIX=./install

# Собираем проект
echo "Building project..."
if [[ "$PLATFORM" == "Windows" ]]; then
    cmake --build . --config Release
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

echo "Build completed successfully!"
echo "Executable location: build/desktop/bin/SimulatorApp"

# Копируем ассеты
echo "Copying assets..."
mkdir -p bin/projects
cp -r ../../projects/sample_projects bin/projects/

echo "=== Project Simulator Desktop Build Complete ==="
echo "To run: cd build/desktop && ./bin/SimulatorApp"

