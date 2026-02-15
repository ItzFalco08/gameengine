#include "InputManager.hpp"
#include "Logger.hpp"
// Define the static member variable
std::unordered_map<int, bool[3]> InputManager::keyMap;
int InputManager::mods;

void InputManager::inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager::mods = mods;

    switch(action) {
        case GLFW_PRESS:
            keyMap[key][PRESSED] = true;
            keyMap[key][HOLD] = true;
            keyMap[key][RELEASED] = false;
            break;

        case GLFW_REPEAT:
            // Key is still held - HOLD remains true
            keyMap[key][PRESSED] = false;
            keyMap[key][HOLD] = true;
            keyMap[key][RELEASED] = false;
            break;

        case GLFW_RELEASE:
            keyMap[key][PRESSED] = false;
            keyMap[key][HOLD] = false;
            keyMap[key][RELEASED] = true;
            break;
    }
}

bool InputManager::isKeyPressed(int keyId) {
    return keyMap[keyId] ? keyMap[keyId][PRESSED] : false;
}

bool InputManager::isKeyHold(int keyId) {
    return keyMap[keyId] ? keyMap[keyId][HOLD] : false;
}

bool InputManager::isKeyReleased(int keyId) {
    return keyMap[keyId] ? keyMap[keyId][RELEASED] : false;
}

void InputManager::clearFrameStates() {
    // Clear PRESSED and RELEASED states for all keys (should only last one frame)
    for (auto& pair : keyMap) {
        pair.second[PRESSED] = false;
        pair.second[RELEASED] = false;
    }
}

bool InputManager::isSpecialPressed(int specialKeyId) {
    return (mods & specialKeyId);
}
