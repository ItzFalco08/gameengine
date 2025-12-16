#pragma once
#include <string>
#include "glm/glm.hpp"

class Shader {
public:
    unsigned int shaderId = 0;

    Shader() = default;
    Shader(const char* fragPath, const char* vertPath);
        
    void use();
    void setFloat(const char* name, const float& val);
    void setMat4(const char* name, const glm::mat4& value);
    void setMat3(const char* name, const glm::mat3& value);
    void setVec4(const char* name, const glm::vec4& value);
    void setVec3(const char* name, const glm::vec3& value);
    void setInt(const char* name, const int& value);

    // Non-copyable (owns GL resource)
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Movable
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

private:
    void checkCompileErrors(unsigned int shaderId, const char* shaderType);
    void checkLinkErrors(unsigned int programId);
    void getShaderTexts(std::string& fragTxt, std::string& vertTxt, const char* fragPath, const char* vertPath);
};
