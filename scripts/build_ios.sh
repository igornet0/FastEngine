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

# Собираем проект
make -j$(sysctl -n hw.ncpu)

echo "Сборка завершена! Библиотека находится в build/ios/"
