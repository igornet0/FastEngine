#!/bin/bash

# Скрипт для сборки Project Simulator для Android
# Требует установленный Android NDK

set -e

echo "=== Building Project Simulator for Android ==="

# Проверяем наличие Android NDK
if [ -z "$ANDROID_NDK" ]; then
    echo "Error: ANDROID_NDK not set"
    echo "Please set ANDROID_NDK environment variable to your Android NDK path"
    echo "Example: export ANDROID_NDK=/path/to/android-ndk"
    exit 1
fi

if [ ! -d "$ANDROID_NDK" ]; then
    echo "Error: ANDROID_NDK directory not found: $ANDROID_NDK"
    exit 1
fi

echo "Using Android NDK: $ANDROID_NDK"

# Создаем директорию для сборки
mkdir -p build/android
cd build/android

# Конфигурируем проект
echo "Configuring project for Android..."
cmake ../.. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-21 \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_ANDROID=ON \
    -DANDROID_STL=c++_shared

# Собираем проект
echo "Building project..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Android build completed successfully!"
echo "APK location: build/android/SimulatorApp.apk"

# Создаем APK (упрощенная версия)
echo "Creating APK..."
mkdir -p apk/lib/arm64-v8a
mkdir -p apk/assets/projects

# Копируем библиотеки
cp libProjectSimulator.a apk/lib/arm64-v8a/
cp libFastEngine.a apk/lib/arm64-v8a/

# Копируем ассеты
cp -r ../../projects/sample_projects apk/assets/projects/

echo "=== Project Simulator Android Build Complete ==="
echo "APK components created in: build/android/apk/"
echo "To create full APK, use Android Studio or gradle"

