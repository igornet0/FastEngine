# Список исходников (Export не нужен на iOS — std::filesystem недоступен до iOS 13)
set(FastEngine_SOURCES
    core/Engine.cpp
    core/World.cpp
    core/Entity.cpp
    platform/Window.cpp
    platform/FileSystem.cpp
    platform/Timer.cpp
    render/Renderer.cpp
    render/Texture.cpp
    render/Shader.cpp
    render/Camera.cpp
    render/ImageLoader.cpp
    render/Mesh.cpp
    render/Lighting.cpp
    audio/AudioManager.cpp
    audio/Sound.cpp
    input/InputManager.cpp
    input/TouchInput.cpp
    input/KeyboardInput.cpp
    input/GamepadInput.cpp
    physics/Collision.cpp
    components/Transform.cpp
    components/Sprite.cpp
    components/Animator.cpp
    components/RigidBody.cpp
    components/Collider.cpp
    components/AudioSource.cpp
    components/Text.cpp
    Systems/RenderSystem.cpp
    Systems/AnimationSystem.cpp
    Systems/PhysicsSystem.cpp
    resources/ResourceManager.cpp
    ui/ButtonManager.cpp
    debug/Console.cpp
    debug/Profiler.cpp
    debug/Wireframe.cpp
    editor/BlueprintNode.cpp
    editor/BlueprintEditor.cpp
    editor/UIElement.cpp
    editor/UIBuilder.cpp
    editor/AnimationEditor.cpp
    editor/MaterialEditor.cpp
    editor/ParticleEditor.cpp
    ai/NavMesh.cpp
    ai/Pathfinding.cpp
    ai/BehaviorTree.cpp
    cinematic/CinematicEditor.cpp
    network/NetworkManager.cpp
    plugins/PluginManager.cpp
    profiling/PerformanceProfiler.cpp
)
if(NOT BUILD_IOS)
    list(APPEND FastEngine_SOURCES export/ProjectExporter.cpp)
endif()

add_library(FastEngine STATIC ${FastEngine_SOURCES})

# Добавляем платформо-специфичные файлы
if(BUILD_ANDROID)
    target_sources(FastEngine PRIVATE platform/Platform_Android.cpp)
    target_link_libraries(FastEngine
        android
        log
        EGL
        GLESv3
    )
elseif(BUILD_IOS)
    enable_language(OBJCXX)
    set_source_files_properties(platform/Platform_iOS.mm PROPERTIES LANGUAGE OBJCXX)
    target_sources(FastEngine PRIVATE platform/Platform_iOS.mm)
    target_link_libraries(FastEngine
        "-framework OpenGLES"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework UIKit"
        "-framework Foundation"
        "-framework QuartzCore"
    )
elseif(BUILD_DESKTOP)
    target_sources(FastEngine PRIVATE platform/Platform_Desktop.cpp)
    find_package(OpenGL REQUIRED)
    find_package(SDL2 REQUIRED)
    
    target_link_libraries(FastEngine
        OpenGL::GL
        SDL2::SDL2
        SDL2::SDL2main
    )
    
    target_include_directories(FastEngine PRIVATE ${SDL2_INCLUDE_DIRS})
endif()

# Устанавливаем свойства библиотеки
set_target_properties(FastEngine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
