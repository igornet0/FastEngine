#!/bin/bash

# Скрипт для сборки под Desktop (Windows/Linux/macOS)

set -e

# Определяем платформу
if [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="windows"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
else
    PLATFORM="linux"
fi

echo "Сборка для платформы: $PLATFORM"

# Создаем директорию для сборки
mkdir -p build/$PLATFORM
cd build/$PLATFORM

# Конфигурируем проект (симулятор отключён — в нём есть ошибки сборки)
cmake ../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_DESKTOP=ON \
    -DBUILD_SIMULATOR=OFF

# Собираем проект
if [[ "$PLATFORM" == "windows" ]]; then
    cmake --build . --config Release
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
fi

echo "Сборка завершена! Исполняемый файл находится в build/$PLATFORM/"
