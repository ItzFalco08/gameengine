#include "KeyManager.hpp"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_DELETE) {
        LOG::Info("Delete Pressed!");
    }
}