#!/bin/bash

# Скрипт для сборки под Android
# Требует установленный Android NDK

set -e

# Проверяем наличие Android NDK
if [ -z "$ANDROID_NDK" ]; then
    echo "Ошибка: ANDROID_NDK не установлен"
    echo "Установите Android NDK и установите переменную окружения ANDROID_NDK"
    exit 1
fi

# Создаем директорию для сборки
mkdir -p build/android
cd build/android

# Конфигурируем проект
cmake ../.. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-21 \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_ANDROID=ON

# Собираем проект
make -j$(nproc)

echo "Сборка завершена! APK находится в build/android/"
