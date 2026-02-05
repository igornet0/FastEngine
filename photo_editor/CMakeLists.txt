cmake_minimum_required(VERSION 3.16)
project(PhotoEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
)

# Enable Qt6 features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(include)

# Modern photo editor with tool system and logging
set(SOURCES
    src/main.cpp
    src/ModernPhotoEditor.cpp
    src/ToolManager.cpp
    src/CanvasWidget.cpp
    src/Logger.cpp
    src/LogPanel.cpp
    src/HotkeyManager.cpp
    src/TooltipManager.cpp
    src/MoveTool.cpp
    src/EyedropperTool.cpp
    src/GradientTool.cpp
    src/PaintBucketTool.cpp
)

# Create executable
add_executable(PhotoEditor ${SOURCES})

# Link libraries
target_link_libraries(PhotoEditor
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# Compile definitions
target_compile_definitions(PhotoEditor PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)