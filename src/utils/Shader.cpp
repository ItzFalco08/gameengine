#include "Shader.hpp"
#include "Logger.hpp"
#include "glad/gl.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <string>


void ShaderBase::use() {
    glUseProgram(shaderId);
}

void ShaderBase::setMat4(const char* name, const glm::mat4& value) {
    glUniformMatrix4fv(getULoc(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderBase::setMat3(const char* name, const glm::mat3& value) {
    glUniformMatrix3fv(getULoc(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderBase::setVec2(const char* name, const glm::vec2& value) {
    glUniform2fv(getULoc(name), 1, glm::value_ptr(value));
}

void ShaderBase::setVec4(const char* name, const glm::vec4& value) {
    glUniform4fv(getULoc(name), 1, glm::value_ptr(value));
}

void ShaderBase::setVec3(const char* name, const glm::vec3& value) {
    glUniform3fv(getULoc(name), 1, glm::value_ptr(value));
}

void ShaderBase::setVec3u(const char* name, const glm::uvec3& value)
{
    glUniform3uiv(getULoc(name), 1, glm::value_ptr(value));
}

void ShaderBase::setFloat(const char* name, float value) {
    glUniform1f(getULoc(name), value);
}

void ShaderBase::setInt(const char* name, int value) {
    glUniform1i(getULoc(name), value);
}

void ShaderBase::setUInt(const char* name, unsigned int value) {
    glUniform1ui(getULoc(name), value);
}

// cached uniform lookup
int ShaderBase::getULoc(const char* name) {
    int uLoc = -1;
    if (uLocCache.contains(name)) {
        uLoc = uLocCache.at(name);
    }
    else {
        uLoc = glGetUniformLocation(shaderId, name);
        if (uLoc == -1) return -1;
        uLocCache.emplace(name, uLoc);
    }

    return uLoc;
}

// -- SHADER --
Shader::Shader(const char* fragPath, const char* vertPath) {
    initShader(fragPath, vertPath);
}

Shader::Shader(const std::string& fragPath, const std::string& vertPath) {
    initShader(fragPath.c_str(), vertPath.c_str());
};

void Shader::initShader(const char* fragPath, const char* vertPath) {
    std::string fragTxt;
    std::string vertTxt;

    getShaderTexts(fragTxt, vertTxt, fragPath, vertPath);
    const char* fragTxtPtr = fragTxt.c_str();
    const char* vertTxtPtr = vertTxt.c_str();

    unsigned int fragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(fragShaderObj, 1, &fragTxtPtr, NULL);
    glCompileShader(fragShaderObj);
    checkCompileErrors(fragShaderObj, "[FRAGMENT SHADER]");

    glShaderSource(vertexShaderObj, 1, &vertTxtPtr, NULL);
    glCompileShader(vertexShaderObj);
    checkCompileErrors(vertexShaderObj, "[VERTEX SHADER]");

    shaderId = glCreateProgram();
    glAttachShader(shaderId, fragShaderObj);
    glAttachShader(shaderId, vertexShaderObj);
    glLinkProgram(shaderId);
    checkLinkErrors(shaderId);
    glDeleteShader(fragShaderObj);
    glDeleteShader(vertexShaderObj);
}

// Movable
Shader::Shader(Shader&& other) noexcept {
	shaderId = other.shaderId;
    other.shaderId = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (shaderId != 0) {
            glDeleteProgram(shaderId);
        }
        shaderId = other.shaderId;
        other.shaderId = 0;
    }
    return *this;
}

Shader::~Shader() {
    if (shaderId != 0) {
        glDeleteProgram(shaderId);
    }
}

void Shader::checkCompileErrors(unsigned int shaderId, const char* shaderType) {
    int success;
    char infoLog[1024];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
        LOG::Error(shaderType, ": \n", infoLog);
    }
}

void Shader::checkLinkErrors(unsigned int programId) {
    int success;
    char infoLog[1024];
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(programId, 1024, NULL, infoLog);
        LOG::Error("[SHADER LINK ERROR]", ": \n", infoLog);
    }
}

void Shader::getShaderTexts(std::string& fragTxt, std::string& vertTxt, const char* fragPath, const char* vertPath) {
    std::ifstream fragFile(fragPath);
    std::ifstream vertFile(vertPath);

    if (!fragFile.is_open()) {
        LOG::Error("Failed to open fragment shader: ", fragPath);
        return;
    }
    if (!vertFile.is_open()) {
        LOG::Error("Failed to open vertex shader: ", vertPath);
        return;
    }

    std::string line;

    while (std::getline(fragFile, line)) {
        fragTxt += line + "\n";
    }

    while (std::getline(vertFile, line)) {
        vertTxt += line + "\n";
    }
}

// -- COMPUTE SHADER --
ComputeShader::ComputeShader(const char* path)
{
    std::ifstream f(path);
    if (!f) { std::cerr << "Cannot open " << path << "\n"; return; }
    std::stringstream ss; ss << f.rdbuf();
    std::string src = ss.str();
    const char* csrc = src.c_str();

    GLuint shc = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shc, 1, &csrc, nullptr);
    glCompileShader(shc);
    
    GLint ok; glGetShaderiv(shc, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetShaderInfoLog(shc, sizeof(log), nullptr, log);
        std::cerr << path << " compile error:\n" << log << "\n";
        glDeleteShader(shc); return;
    }

    shaderId = glCreateProgram();
    glAttachShader(shaderId, shc);
    glLinkProgram(shaderId);
    glDeleteShader(shc);

    glGetProgramiv(shaderId, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetProgramInfoLog(shaderId, sizeof(log), nullptr, log);
        std::cerr << path << " link error:\n" << log << "\n";
        glDeleteProgram(shaderId); return;
    }
}

ComputeShader::ComputeShader(ComputeShader&& other) noexcept
{
    shaderId = other.shaderId;
    other.shaderId = 0;
}

ComputeShader::~ComputeShader()
{
    if (shaderId != 0) {
        glDeleteProgram(shaderId);
    }
}
