#include "InputManager.hpp"

void InputManager::inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    keyMap[key][PRESSED] = false;
    keyMap[key][HOLD] = false;
    keyMap[key][RELEASED] = false;

    switch(action) {
        case GLFW_PRESS:
            keyMap[key][PRESSED] = true;
            break;
        case GLFW_REPEAT:
            keyMap[key][HOLD] = true;
            break;
        case GLFW_RELEASE:
            keyMap[key][RELEASED] = true;
            break;
    }
}    

bool InputManager::isKeyPressed(int keyId) {
    return keyMap[keyId][PRESSED];
}

bool InputManager::isKeyHold(int keyId) {
    return keyMap[keyId][HOLD];
}

bool InputManager::isKeyReleased(int keyId) {
    return keyMap[keyId][RELEASED];
}