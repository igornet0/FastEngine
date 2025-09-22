#include "FastEngine/Render/Shader.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"
#include <iostream>

int main() {
    std::cout << "Shader Test Starting..." << std::endl;
    
    // Инициализируем платформу
    auto& platform = FastEngine::Platform::GetInstance();
    if (!platform.Initialize()) {
        std::cout << "Failed to initialize platform!" << std::endl;
        return 1;
    }
    
    // Создаем шейдер
    FastEngine::Shader shader;
    
    // Пробуем загрузить из файлов
    std::string vertexPath = platform.GetFileSystem()->GetResourcePath("shaders/sprite.vert");
    std::string fragmentPath = platform.GetFileSystem()->GetResourcePath("shaders/sprite.frag");
    
    std::cout << "Vertex shader path: " << vertexPath << std::endl;
    std::cout << "Fragment shader path: " << fragmentPath << std::endl;
    
    if (shader.LoadFromFiles(vertexPath, fragmentPath)) {
        std::cout << "Shader loaded successfully from files!" << std::endl;
    } else {
        std::cout << "Failed to load shader from files, trying built-in..." << std::endl;
        
        // Пробуем встроенные шейдеры
        std::string vertexSource = R"(
#version 120
attribute vec3 aPos;
attribute vec2 aTexCoord;
uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
varying vec2 TexCoord;
void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
        )";
        
        std::string fragmentSource = R"(
#version 120
varying vec2 TexCoord;
uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;
void main() {
    if (uUseTexture) {
        gl_FragColor = texture2D(uTexture, TexCoord) * uColor;
    } else {
        gl_FragColor = uColor;
    }
}
        )";
        
        if (shader.LoadFromSource(vertexSource, fragmentSource)) {
            std::cout << "Built-in shader loaded successfully!" << std::endl;
        } else {
            std::cout << "Failed to load built-in shader!" << std::endl;
        }
    }
    
    platform.Shutdown();
    std::cout << "Shader Test Finished." << std::endl;
    return 0;
}
