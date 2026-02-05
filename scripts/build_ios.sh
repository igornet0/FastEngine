#!/bin/bash

# Скрипт для сборки под iOS
# Требует установленный Xcode

set -e

# Создаем директорию для сборки
mkdir -p build/ios
cd build/ios

# Конфигурируем проект
cmake ../.. \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_IOS=ON

# Собираем проект (только библиотека FastEngine; примеры для iOS не собираются)
make -j$(sysctl -n hw.ncpu) FastEngine

echo "Сборка завершена! Библиотека: build/ios/src/libFastEngine.a"
echo "Для запуска на симуляторе или устройстве откройте ios_app/FastEngineApp.xcodeproj в Xcode."
