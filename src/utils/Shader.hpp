#pragma once
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

class ShaderBase {
public:
    unsigned int shaderId = 0;
    std::unordered_map<std::string, int> uLocCache;

    void use();
    void setFloat(const char* name, float val);
    void setInt(const char* name, int value);
    void setUInt(const char* name, unsigned int value);
    void setMat4(const char* name, const glm::mat4& value);
    void setMat3(const char* name, const glm::mat3& value);
    void setVec2(const char* name, const glm::vec2& value);
    void setVec4(const char* name, const glm::vec4& value);
    void setVec3(const char* name, const glm::vec3& value);
	void setVec3u(const char* name, const glm::uvec3& value);

    virtual ~ShaderBase() = default;
private:
	int getULoc(const char* name);
};

class Shader : public ShaderBase {
public:

    Shader() = default;
    Shader(const char* fragPath, const char* vertPath);
    Shader(const std::string& fragPath, const std::string& vertPath);
    
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
    void initShader(const char* fragPath, const char* vertPath);
};

class ComputeShader : public ShaderBase {
public:
    ComputeShader() = default;
	ComputeShader(const char* compPath);

    // non-copyable
    ComputeShader(const ComputeShader&) = delete;
	ComputeShader& operator=(const ComputeShader&) = delete;
    
    // movable
    ComputeShader(ComputeShader&& other) noexcept;
    ComputeShader& operator=(ComputeShader&& other) noexcept;

    ~ComputeShader();

};