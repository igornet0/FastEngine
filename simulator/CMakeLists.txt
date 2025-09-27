cmake_minimum_required(VERSION 3.16)
project(ProjectSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройки для разных платформ
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    if(TARGET_OS_IPHONE)
        set(CMAKE_SYSTEM_NAME iOS)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    endif()
elseif(ANDROID)
    set(CMAKE_ANDROID_STL_TYPE c++_shared)
    set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)
endif()

# Включаем директории
include_directories(include)
include_directories(../include)

# Находим зависимости
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Подключаем уже собранную библиотеку FastEngine
add_library(FastEngine STATIC IMPORTED)
set_target_properties(FastEngine PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../build/libFastEngine.a
)

# Включаем заголовочные файлы FastEngine
target_include_directories(FastEngine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Исходные файлы
set(SIMULATOR_SOURCES
    src/core/ProjectSimulator.cpp
    src/core/ProjectManager.cpp
    src/core/SimulationEngine.cpp
    src/core/TestRunner.cpp
    src/core/CLI.cpp
    src/core/SceneEditorSimulator.cpp
    src/ui/UIManager.cpp
    src/ui/SceneEditor.cpp
    src/ui/Inspector.cpp
    src/ui/Hierarchy.cpp
    src/ui/AssetBrowser.cpp
    src/projects/Project.cpp
    src/projects/ProjectLoader.cpp
    src/projects/ProjectValidator.cpp
)

# Заголовочные файлы
set(SIMULATOR_HEADERS
    include/ProjectSimulator/ProjectSimulator.h
    include/ProjectSimulator/ProjectManager.h
    include/ProjectSimulator/SimulationEngine.h
    include/ProjectSimulator/UIManager.h
    include/ProjectSimulator/TestRunner.h
    include/ProjectSimulator/Project.h
    include/ProjectSimulator/CLI.h
)

# Создаем библиотеку симулятора
add_library(ProjectSimulator STATIC ${SIMULATOR_SOURCES} ${SIMULATOR_HEADERS})

# Линкуем с FastEngine и зависимостями
target_link_libraries(ProjectSimulator FastEngine OpenGL::GL SDL2::SDL2 SDL2::SDL2main)

# Настройки для iOS
if(IOS)
    target_link_libraries(ProjectSimulator
        "-framework Foundation"
        "-framework UIKit"
        "-framework OpenGLES"
        "-framework QuartzCore"
    )
endif()

# Настройки для Android
if(ANDROID)
    target_link_libraries(ProjectSimulator
        android
        log
        EGL
        GLESv3
    )
endif()

# Настройки для macOS
if(APPLE AND NOT IOS)
    target_link_libraries(ProjectSimulator
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
endif()

# Создаем CLI исполняемый файл
add_executable(SimulatorCLI
    examples/cli_main.cpp
)

target_link_libraries(SimulatorCLI ProjectSimulator)

# Тест редактора сцен
add_executable(SceneEditorTest
    examples/scene_editor_test.cpp
)

target_link_libraries(SceneEditorTest ProjectSimulator)

# Устанавливаем директории для ассетов
if(IOS)
    set_target_properties(SimulatorApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Project Simulator"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
    
    # Копируем ассеты в бандл
    set_source_files_properties(
        projects/sample_projects
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

# Копируем ассеты в выходную директорию
file(COPY projects/sample_projects DESTINATION ${CMAKE_BINARY_DIR}/projects)

# Устанавливаем переменные для конфигурации
target_compile_definitions(ProjectSimulator PRIVATE
    $<$<CONFIG:Debug>:DEBUG=1>
    $<$<CONFIG:Release>:NDEBUG=1>
)

# Включаем предупреждения
if(MSVC)
    target_compile_options(ProjectSimulator PRIVATE /W4)
else()
    target_compile_options(ProjectSimulator PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Устанавливаем выходные директории
set_target_properties(ProjectSimulator PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Создаем примеры проектов
add_subdirectory(examples)