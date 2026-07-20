#include "imgui.h"
#include <iostream>
#include <string>
#include "../core/Scene.hpp"

class HeriarchyPanel {
public:
    void Render();
    
private:
    // P A N E L S
    void CreateGameObjectPanel();
    bool showCreateGameObjPanel = false;
    bool showRenamePanel = false;
    int uiIdCounter = 0;

    void RenderHeriarchy(std::vector<GameObject*>& roots);
    void handleKeyEvents();
    void RenameSelected(const char* newName);
};