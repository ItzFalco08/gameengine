#include "InputManager.hpp"
#include "../utils/Logger.hpp"
#include "backends/imgui_impl_sdl3.h"
// Define the static member variable
std::unordered_map<int, bool[3]> InputManager::keyMap;
int InputManager::mods;

void InputManager::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);

    if (event.type == SDL_EVENT_KEY_DOWN) {
        InputManager::mods = event.key.mod;
        auto& state = keyMap[event.key.scancode];
        state[PRESSED] = !event.key.repeat;
        state[DOWN] = true;
        state[RELEASED] = false;
    } else if (event.type == SDL_EVENT_KEY_UP) {
        InputManager::mods = event.key.mod;
        auto& state = keyMap[event.key.scancode];
        state[PRESSED] = false;
        state[DOWN] = false;
        state[RELEASED] = true;
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
