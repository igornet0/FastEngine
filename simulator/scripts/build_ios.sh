#!/bin/bash

# Скрипт для сборки Project Simulator для iOS
# Требует установленный Xcode

set -e

echo "=== Building Project Simulator for iOS ==="

# Проверяем наличие Xcode
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode not found. Please install Xcode."
    exit 1
fi

# Создаем директорию для сборки
mkdir -p build/ios
cd build/ios

# Конфигурируем проект
echo "Configuring project for iOS..."
cmake ../.. \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_IOS=ON \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer"

# Собираем проект
echo "Building project..."
cmake --build . --config Release

echo "iOS build completed successfully!"
echo "App bundle location: build/ios/SimulatorApp.app"

# Создаем директорию для ассетов в бандле
mkdir -p SimulatorApp.app/projects
cp -r ../../projects/sample_projects SimulatorApp.app/projects/

echo "=== Project Simulator iOS Build Complete ==="
echo "To install on device: Use Xcode or xcrun simctl install"

