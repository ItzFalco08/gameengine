#pragma once
#include <string>

class Shader {
public:
    unsigned int shaderId = 0;

    Shader() = default;
    Shader(const char* fragPath, const char* vertPath);

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
