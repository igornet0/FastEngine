# Создаем библиотеку FastEngine
add_library(FastEngine STATIC
    # Core
    core/Application.cpp
    core/Engine.cpp
    core/World.cpp
    core/Entity.cpp
    core/Component.cpp
    core/System.cpp
    
    # Platform
    platform/Platform.cpp
    platform/Window.cpp
    platform/FileSystem.cpp
    platform/Timer.cpp
    
    # Render
    render/Renderer.cpp
    render/Texture.cpp
    render/Sprite.cpp
    render/Camera.cpp
    
    # Audio
    audio/AudioManager.cpp
    audio/Sound.cpp
    
    # Input
    input/InputManager.cpp
    input/TouchInput.cpp
    input/KeyboardInput.cpp
)

# Подключаем библиотеки в зависимости от платформы
if(BUILD_ANDROID)
    target_link_libraries(FastEngine
        android
        log
        EGL
        GLESv2
    )
elseif(BUILD_IOS)
    target_link_libraries(FastEngine
        "-framework OpenGLES"
        "-framework AudioToolbox"
        "-framework AVFoundation"
    )
elseif(BUILD_DESKTOP)
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
