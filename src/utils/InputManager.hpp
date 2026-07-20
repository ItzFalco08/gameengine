#pragma once
#include "glfw/glfw3.h"
#include "Logger.hpp"
#include <iostream> 
#include <unordered_map>

// basically a namespace since everything is static
class InputManager {
public:
    static std::unordered_map<int, bool[3]> keyMap;
    static int mods;
    enum KeyState {
        PRESSED = 0,
        DOWN = 1,
        RELEASED = 2
    };
    
    static void inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void clearFrameStates(); // Call this at the end of each frame

    static bool isKeyPressed(int keyId);
    static bool isKeyDown(int keyId);
    static bool isKeyReleased(int keyId);
    static bool isSpecialDown(int specialKeyId);
};