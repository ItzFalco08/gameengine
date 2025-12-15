#include "Shader.hpp"
#include "Logger.hpp"
#include "glad/gl.h"

#include <fstream>
#include <sstream>
#include <string>

Shader::Shader(const char* fragPath, const char* vertPath) {
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
Shader::Shader(Shader&& other) noexcept : shaderId(other.shaderId) {
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

    LOG::Info("Vertex Shader: \n", vertTxt, "\nFragment Shader: \n", fragTxt);
}
