cmake_minimum_required(VERSION 3.16)
project(PhotoEditorTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
    Test
)

# Enable Qt6 features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
include_directories(../include)
include_directories(../src)

# Test sources
set(TEST_SOURCES
    main.cpp
    HotkeyManagerTest.cpp
    TooltipManagerTest.cpp
    MoveToolTest.cpp
    EyedropperToolTest.cpp
    IntegrationTest.cpp
)

# Source files needed for tests
set(SOURCE_FILES
    ../src/HotkeyManager.cpp
    ../src/TooltipManager.cpp
    ../src/MoveTool.cpp
    ../src/EyedropperTool.cpp
    ../src/Tool.cpp
    ../src/Logger.cpp
)

# Header files needed for MOC generation
set(HEADER_FILES
    ../include/HotkeyManager.h
    ../include/TooltipManager.h
    ../include/MoveTool.h
    ../include/EyedropperTool.h
    ../include/Tool.h
    ../include/Logger.h
)

# Create test executable
add_executable(PhotoEditorTests ${TEST_SOURCES} ${SOURCE_FILES} ${HEADER_FILES})

# Link libraries
target_link_libraries(PhotoEditorTests
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Test
)

# Compile definitions
target_compile_definitions(PhotoEditorTests PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

# Enable testing
enable_testing()
add_test(NAME PhotoEditorTests COMMAND PhotoEditorTests)
