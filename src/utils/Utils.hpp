#pragma once
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "vector"
#include "sstream"
#include "fstream"
#include "glm/glm.hpp"
#include "array"


namespace Utils {
    void GLFWErrorCallback(int errorCode, const char* message);
    void genSceneFramebuffers();
    void updateFBODimensions();

    namespace GUI {
        void initImGui(GLFWwindow* window);
        void ShowTextInputDialoge(const char* title, const char* defaultValue, bool& isActive,  std::function<void(std::string)> onOk);
    }
}