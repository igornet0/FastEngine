cmake_minimum_required(VERSION 3.16)
project(FastEngine VERSION 1.0.0 LANGUAGES CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подавляем предупреждения OpenGL на macOS
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

# Опции сборки
option(BUILD_ANDROID "Build for Android" OFF)
option(BUILD_IOS "Build for iOS" OFF)
option(BUILD_DESKTOP "Build for Desktop" ON)

# Настройки для разных платформ
if(BUILD_ANDROID)
    set(CMAKE_SYSTEM_NAME Android)
    set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK})
    set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
    set(CMAKE_ANDROID_STL_TYPE c++_shared)
elseif(BUILD_IOS)
    set(CMAKE_SYSTEM_NAME iOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
endif()

# Включаем директории
include_directories(include)
include_directories(thirdparty)

# Добавляем путь к GLM
include_directories(/opt/homebrew/Cellar/glm/1.0.1/include)

# Добавляем папку cmake для поиска модулей
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Поиск зависимостей
find_package(GLM REQUIRED)

# Подключаем подпроекты
add_subdirectory(src)
add_subdirectory(examples)

# Опция для сборки симулятора
option(BUILD_SIMULATOR "Build Project Simulator" ON)

# Подключаем симулятор если включен
if(BUILD_SIMULATOR)
    add_subdirectory(simulator)
endif()

# Устанавливаем цели
install(TARGETS FastEngine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Устанавливаем заголовочные файлы
install(DIRECTORY include/ DESTINATION include)
