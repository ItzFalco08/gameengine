#include "../../core/Scene.hpp"

class HierarchyPanel {
public:
    void Render();
    
private:
    // P A N E L S
    char createInputBuffer[256] = {};
    char renameInputBuffer[256] = {};
    int uiIdCounter = 0;

    void RenderHeriarchy(std::vector<GameObject*>& roots);
    void handleKeyEvents();
    void RenameSelected(const char* newName);
    void renderPanels();
};