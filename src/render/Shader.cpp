#include "FastEngine/Render/Shader.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl.h>
#endif
#else
#include <GL/gl.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>

namespace FastEngine {
    Shader::Shader() 
        : m_shaderID(0)
        , m_loaded(false) {
    }
    
    Shader::~Shader() {
        Destroy();
    }
    
    bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
        // Читаем файлы шейдеров
        std::string vertexSource = ReadFile(vertexPath);
        std::string fragmentSource = ReadFile(fragmentPath);
        
        if (vertexSource.empty() || fragmentSource.empty()) {
            std::cerr << "Failed to read shader files" << std::endl;
            return false;
        }
        
        return LoadFromSource(vertexSource, fragmentSource);
    }
    
    bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
        // Компилируем шейдеры
        unsigned int vertex = CompileShader(vertexSource, GL_VERTEX_SHADER);
        unsigned int fragment = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
        
        if (vertex == 0 || fragment == 0) {
            return false;
        }
        
        // Создаем программу шейдеров
        m_shaderID = glCreateProgram();
        glAttachShader(m_shaderID, vertex);
        glAttachShader(m_shaderID, fragment);
        
        // Линкуем программу
        if (!LinkProgram(vertex, fragment)) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }
        
        // Удаляем промежуточные шейдеры
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        m_loaded = true;
        return true;
    }
    
    void Shader::Destroy() {
        if (m_shaderID != 0) {
            glDeleteProgram(m_shaderID);
            m_shaderID = 0;
        }
        m_loaded = false;
    }
    
    void Shader::Use() const {
        if (m_loaded) {
            glUseProgram(m_shaderID);
        }
    }
    
    void Shader::Unuse() const {
        glUseProgram(0);
    }
    
    void Shader::SetBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_shaderID, name.c_str()), (int)value);
    }
    
    void Shader::SetInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_shaderID, name.c_str()), value);
    }
    
    void Shader::SetFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_shaderID, name.c_str()), value);
    }
    
    void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, &value[0]);
    }
    
    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, &value[0]);
    }
    
    void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, &value[0]);
    }
    
    void Shader::SetMat2(const std::string& name, const glm::mat2& value) const {
        glUniformMatrix2fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    
    void Shader::SetMat3(const std::string& name, const glm::mat3& value) const {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    
    void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    
    unsigned int Shader::CompileShader(const std::string& source, unsigned int type) {
        unsigned int shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        
        // Проверяем ошибки компиляции
        int success;
        char infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        
        return shader;
    }
    
    bool Shader::LinkProgram(unsigned int vertex, unsigned int fragment) {
        glLinkProgram(m_shaderID);
        
        // Проверяем ошибки линковки
        int success;
        char infoLog[1024];
        glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
        
        if (!success) {
            glGetProgramInfoLog(m_shaderID, 1024, nullptr, infoLog);
            std::cerr << "Shader program linking failed: " << infoLog << std::endl;
            return false;
        }
        
        return true;
    }
    
    std::string Shader::ReadFile(const std::string& filePath) const {
        // Сначала пробуем прочитать через FileSystem
        auto* fileSystem = Platform::GetInstance().GetFileSystem();
        if (fileSystem) {
            std::string content = fileSystem->ReadFile(filePath);
            if (!content.empty()) {
                return content;
            }
        }
        
        // Если не удалось, пробуем стандартный способ
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}
