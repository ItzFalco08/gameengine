#pragma once
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "vector"
#include "sstream"
#include "fstream"
#include "glm/glm.hpp"
#include "array"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

namespace Utils {
    void GLFWErrorCallback(int errorCode, const char* message);
    void genSceneFramebuffers();
    void updateFBODimensions();
    void loadObj(std::vector<Vertex>& vertices, const char* objPath);

    namespace GUI {
        void initImGui(GLFWwindow* window);
        void ShowTextInputDialoge(const char* title, const char* defaultValue, bool& isActive,  std::function<void(std::string)> onOk);
    }
}