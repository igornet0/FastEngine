# Пример приложения
add_executable(ExampleApp
    main.cpp
    Game.cpp
    Game.h
)

# Простой тест
add_executable(SimpleTest
    simple_test.cpp
)

# Минимальный тест
add_executable(MinimalTest
    minimal_test.cpp
)

# Тест Engine
add_executable(EngineTest
    engine_test.cpp
)

# Тест Platform
add_executable(PlatformTest
    platform_test.cpp
)

# Debug тест
add_executable(DebugTest
    debug_test.cpp
)

# Shader тест
add_executable(ShaderTest
    shader_test.cpp
)

# Complete тест
add_executable(CompleteTest
    complete_test.cpp
)

# Component тест
add_executable(ComponentTest
    component_test.cpp
)

# Debug Tools тест
add_executable(DebugToolsTest
    debug_tools_test.cpp
)

# Scene Editor Simple тест
add_executable(SceneEditorSimpleTest
    scene_editor_simple_test.cpp
)

# Blueprint Editor тест
add_executable(BlueprintEditorTest
    blueprint_editor_test.cpp
)

# Advanced Editor тест
add_executable(AdvancedEditorTest
    advanced_editor_test.cpp
)

# Advanced Features тест
add_executable(AdvancedFeaturesTest
    advanced_features_test.cpp
)

target_link_libraries(ExampleApp FastEngine)
target_link_libraries(SimpleTest FastEngine)
target_link_libraries(MinimalTest FastEngine)
target_link_libraries(EngineTest FastEngine)
target_link_libraries(PlatformTest FastEngine)
target_link_libraries(DebugTest FastEngine)
target_link_libraries(ShaderTest FastEngine)
target_link_libraries(CompleteTest FastEngine)
target_link_libraries(ComponentTest FastEngine)
target_link_libraries(DebugToolsTest FastEngine)
target_link_libraries(SceneEditorSimpleTest FastEngine)
target_link_libraries(BlueprintEditorTest FastEngine)
target_link_libraries(AdvancedEditorTest FastEngine)
target_link_libraries(AdvancedFeaturesTest FastEngine)

add_executable(ExportTest export_test.cpp)
target_link_libraries(ExportTest FastEngine)

add_executable(PerformanceTest performance_test.cpp)
target_link_libraries(PerformanceTest FastEngine)

# Устанавливаем свойства
set_target_properties(ExampleApp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(SimpleTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(MinimalTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(EngineTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(PlatformTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(DebugTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(ShaderTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(CompleteTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(ComponentTest PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
