cmake_minimum_required(VERSION 3.16)
project(FastEngineQtEditorTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test OpenGL OpenGLWidgets)

# Enable Qt6 MOC
set(CMAKE_AUTOMOC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../editors/qt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Individual test executables (safer approach)

# Test 1: MainWindow only
add_executable(TestMainWindow 
    test_mainwindow.cpp
    ../../editors/qt/MainWindow.cpp
    ../../editors/qt/SceneView2D.cpp
    ../../editors/qt/SceneView3D.cpp
    ../../editors/qt/HierarchyPanel.cpp
    ../../editors/qt/InspectorPanel.cpp
    ../../editors/qt/AssetsPanel.cpp
    ../../editors/qt/ConsolePanel.cpp
    ../../editors/qt/ProjectManager.cpp
    ../../editors/qt/FastEngineIntegration.cpp
)

target_link_libraries(TestMainWindow 
    Qt6::Core Qt6::Widgets Qt6::Test Qt6::OpenGL Qt6::OpenGLWidgets
)

# Test 2: Individual component tests
add_executable(TestComponents
    test_component_main.cpp
    test_hierarchypanel.cpp
    test_inspectorpanel.cpp
    test_assetspanel.cpp
    test_consolepanel.cpp
    ../../editors/qt/HierarchyPanel.cpp
    ../../editors/qt/InspectorPanel.cpp
    ../../editors/qt/AssetsPanel.cpp
    ../../editors/qt/ConsolePanel.cpp
)

target_link_libraries(TestComponents 
    Qt6::Core Qt6::Widgets Qt6::Test
)

# Test 3: Scene views (potentially problematic)
add_executable(TestSceneViews
    test_sceneviews_main.cpp
    test_sceneview2d.cpp
    test_sceneview3d.cpp
    ../../editors/qt/SceneView2D.cpp
    ../../editors/qt/SceneView3D.cpp
    ../../editors/qt/FastEngineIntegration.cpp
)

target_link_libraries(TestSceneViews 
    Qt6::Core Qt6::Widgets Qt6::Test Qt6::OpenGL Qt6::OpenGLWidgets
)

# Test 4: FastEngine integration (most likely problematic)
add_executable(TestFastEngineIntegration
    test_integration_main.cpp
    test_fastengineintegration.cpp
    test_projectmanager.cpp
    ../../editors/qt/FastEngineIntegration.cpp
    ../../editors/qt/ProjectManager.cpp
)

target_link_libraries(TestFastEngineIntegration 
    Qt6::Core Qt6::Widgets Qt6::Test
)

# Enable testing
enable_testing()
add_test(NAME TestMainWindow COMMAND TestMainWindow)
add_test(NAME TestComponents COMMAND TestComponents)
add_test(NAME TestSceneViews COMMAND TestSceneViews)
add_test(NAME TestFastEngineIntegration COMMAND TestFastEngineIntegration)
