#include "InputManager.hpp"
#include "Logger.hpp"
#include "backends/imgui_impl_glfw.h" // Add this include for ImGui_ImplGlfw_KeyCallback
// Define the static member variable
std::unordered_map<int, bool[3]> InputManager::keyMap;
int InputManager::mods;

void InputManager::inputCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // imguiCallback
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    InputManager::mods = mods;

    switch(action) {
        case GLFW_PRESS:
            keyMap[key][PRESSED] = true;
            keyMap[key][DOWN] = false;
            keyMap[key][RELEASED] = false;
            break;

        case GLFW_REPEAT:
            // Key is still held - DOWN remains true
            keyMap[key][PRESSED] = false;
            keyMap[key][DOWN] = true;
            keyMap[key][RELEASED] = false;
            break;

        case GLFW_RELEASE:
            keyMap[key][PRESSED] = false;
            keyMap[key][DOWN] = false;
            keyMap[key][RELEASED] = true;
            break;

    }
}

bool InputManager::isKeyPressed(int keyId) {
    return keyMap[keyId] ? keyMap[keyId][PRESSED] : false;
}

bool InputManager::isKeyDown(int keyId) {
    return keyMap[keyId] ? keyMap[keyId][DOWN] : false;
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

bool InputManager::isSpecialDown(int specialKeyId) {
    return (mods & specialKeyId);
}
