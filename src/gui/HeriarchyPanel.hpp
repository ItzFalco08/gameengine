#include "imgui/imgui.h"
#include <iostream>
#include <string>
#include "../core/Scene.hpp"
extern Scene* editorScene;

class HeriarchyPanel {
public:
    void Render();
    
private:
    // P A N E L S
    void CreateGameObjectPanel();
    bool showCreateGameObjPanel = false;
    bool showRenamePanel = false;

    void HandleAddGameObject(std::string gameObjectName);
    void RenderHeriarchy(std::vector<GameObject*>& roots);
    void RenameSelected(const char* newName);
    void DeleteSelected(std::vector<GameObject*>& roots);
};