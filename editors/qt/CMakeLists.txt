cmake_minimum_required(VERSION 3.16)
project(FastEngineQtEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Qt6
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    OpenGL 
    OpenGLWidgets
    3DCore
    3DRender
    3DInput
    3DExtras
    Gui
)

# Поиск OpenGL
find_package(OpenGL REQUIRED)

# Включение Qt6
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Исходные файлы
set(SOURCES
    main.cpp
    MainWindow.cpp
    SceneView2D.cpp
    SceneView3D.cpp
    InspectorPanel.cpp
    HierarchyPanel.cpp
    AssetsPanel.cpp
    ConsolePanel.cpp
    ProjectManager.cpp
    FastEngineIntegration.cpp
)

# Заголовочные файлы
set(HEADERS
    MainWindow.h
    SceneView2D.h
    SceneView3D.h
    InspectorPanel.h
    HierarchyPanel.h
    AssetsPanel.h
    ConsolePanel.h
    ProjectManager.h
    FastEngineIntegration.h
)

# Создание исполняемого файла
add_executable(FastEngineQtEditor ${SOURCES} ${HEADERS})

# Подключение библиотек
target_link_libraries(FastEngineQtEditor
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::3DCore
    Qt6::3DRender
    Qt6::3DInput
    Qt6::3DExtras
    Qt6::Gui
    OpenGL::GL
    FastEngine
)

# Включение директорий
target_include_directories(FastEngineQtEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Настройки компиляции
target_compile_definitions(FastEngineQtEditor PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

# Копирование ресурсов
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/editors/qt)
