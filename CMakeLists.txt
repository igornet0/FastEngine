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

# Добавляем папку cmake для поиска модулей
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Поиск зависимостей (GLM из thirdparty/glm или системы)
set(GLM_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
find_package(GLM)
if(GLM_INCLUDE_DIRS)
    include_directories(${GLM_INCLUDE_DIRS})
    message(STATUS "Found GLM: ${GLM_INCLUDE_DIRS}")
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm/glm/glm.hpp)
    set(GLM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm)
    include_directories(${GLM_INCLUDE_DIRS})
    message(STATUS "Using GLM from thirdparty: ${GLM_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "GLM not found. Add thirdparty/glm (git submodule add https://github.com/g-truc/glm.git thirdparty/glm) or install system GLM.")
endif()

# Подключаем подпроекты
add_subdirectory(src)
# Примеры для iOS собираются через ios_app/ в Xcode
if(NOT BUILD_IOS)
    add_subdirectory(examples)
endif()

# Desktop приложения в apps/
if(BUILD_DESKTOP AND NOT BUILD_IOS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/apps/macos_app/CMakeLists.txt)
        add_subdirectory(apps/macos_app)
    endif()
    add_subdirectory(apps/snake_cros_app)
endif()

# Опция для сборки симулятора
option(BUILD_SIMULATOR "Build Project Simulator" ON)

# Подключаем симулятор если включен (не для iOS)
if(BUILD_SIMULATOR AND NOT BUILD_IOS)
    add_subdirectory(simulator)
endif()

# Опции для сборки редакторов
option(BUILD_QT_EDITOR "Build Qt Editor" ON)
option(BUILD_SDL_EDITOR "Build SDL Editor" ON)
option(BUILD_ADVANCED_EDITOR "Build Advanced Editor" ON)
option(BUILD_QT_EDITOR_TESTS "Build Qt Editor Tests" OFF)

# Подключаем редакторы если включены (не для iOS)
if(BUILD_QT_EDITOR AND NOT BUILD_IOS)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL OpenGLWidgets 3DCore 3DRender 3DInput 3DExtras Gui)
    add_subdirectory(editors/qt)
endif()

if(BUILD_SDL_EDITOR AND NOT BUILD_IOS)
    add_subdirectory(editors/sdl)
endif()

if(BUILD_ADVANCED_EDITOR AND NOT BUILD_IOS)
    add_subdirectory(editors/advanced)
endif()

if(BUILD_QT_EDITOR_TESTS AND NOT BUILD_IOS)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test OpenGL OpenGLWidgets)
    add_subdirectory(tests/editors/qt)
endif()

# Устанавливаем цели
install(TARGETS FastEngine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Устанавливаем заголовочные файлы
install(DIRECTORY include/ DESTINATION include)
