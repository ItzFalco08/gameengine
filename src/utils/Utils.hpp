#pragma once
#include "glad/gl.h"
#include "GLFW/glfw3.h"

namespace Utils {
    void initImGui(GLFWwindow* window);
    void GLFWErrorCallback(int errorCode, const char* message);
    void genSceneFramebuffers();
    void updateFBODimensions();
}