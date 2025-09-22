#pragma once

#include <string>
#include <glm/glm.hpp>

namespace FastEngine {
    class Shader {
    public:
        Shader();
        ~Shader();
        
        // Загрузка и создание шейдера
        bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
        bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
        void Destroy();
        
        // Использование шейдера
        void Use() const;
        void Unuse() const;
        
        // Установка uniform переменных
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetVec2(const std::string& name, const glm::vec2& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetVec4(const std::string& name, const glm::vec4& value) const;
        void SetMat2(const std::string& name, const glm::mat2& value) const;
        void SetMat3(const std::string& name, const glm::mat3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        
        // Получение ID шейдера
        unsigned int GetID() const { return m_shaderID; }
        
    private:
        unsigned int m_shaderID;
        bool m_loaded;
        
        // Вспомогательные методы
        unsigned int CompileShader(const std::string& source, unsigned int type);
        bool LinkProgram(unsigned int vertex, unsigned int fragment);
        std::string ReadFile(const std::string& filePath) const;
    };
}
