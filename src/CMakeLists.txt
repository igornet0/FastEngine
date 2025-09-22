# Создаем библиотеку FastEngine
add_library(FastEngine STATIC
    # Core
    core/Engine.cpp
    core/World.cpp
    core/Entity.cpp
    
    # Platform - базовые файлы
    platform/Window.cpp
    platform/FileSystem.cpp
    platform/Timer.cpp
    
    # Render
    render/Renderer.cpp
    render/Texture.cpp
    render/Shader.cpp
    render/Camera.cpp
    render/ImageLoader.cpp
    render/Mesh.cpp
    render/Lighting.cpp
    
    # Audio
    audio/AudioManager.cpp
    audio/Sound.cpp
    
    # Input
    input/InputManager.cpp
    input/TouchInput.cpp
    input/KeyboardInput.cpp
    input/GamepadInput.cpp
    
    # Physics
    physics/Collision.cpp
    
    # Components
    components/Transform.cpp
    components/Sprite.cpp
    components/Animator.cpp
    components/RigidBody.cpp
    components/Collider.cpp
    components/AudioSource.cpp
    components/Text.cpp
    
    # Systems
    Systems/RenderSystem.cpp
    Systems/AnimationSystem.cpp
    Systems/PhysicsSystem.cpp
    
    # Resources
    resources/ResourceManager.cpp
    
    # Debug Tools
    debug/Console.cpp
    debug/Profiler.cpp
    debug/Wireframe.cpp
    
    # Editor Tools
    editor/BlueprintNode.cpp
    editor/BlueprintEditor.cpp
    editor/UIElement.cpp
    editor/UIBuilder.cpp
    editor/AnimationEditor.cpp
    editor/MaterialEditor.cpp
    editor/ParticleEditor.cpp
    
    # AI Systems
    ai/NavMesh.cpp
    ai/Pathfinding.cpp
    ai/BehaviorTree.cpp
    
    # Cinematic
    cinematic/CinematicEditor.cpp
    
    # Network
    network/NetworkManager.cpp
    
    # Plugins
    plugins/PluginManager.cpp
    
    # Export
    export/ProjectExporter.cpp
    
    # Profiling
    profiling/PerformanceProfiler.cpp
)

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
    target_sources(FastEngine PRIVATE platform/Platform_iOS.cpp)
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
