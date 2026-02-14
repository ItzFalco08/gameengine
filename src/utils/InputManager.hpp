#pragma once
#include "glfw/glfw3.h"
#include "Logger.hpp"
#include <iostream> 
#include <unordered_map>

// basically a namespace since everything is static
class InputManager {
public:
    static std::unordered_map<int, bool[3]> keyMap;
    static enum KeyState {
        PRESSED = 0,
        HOLD = 1,
        RELEASED = 2
    };
    
    static void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static bool isKeyPressed(int keyId);
    static bool isKeyHold(int keyId);
    static bool isKeyReleased(int keyId);
};