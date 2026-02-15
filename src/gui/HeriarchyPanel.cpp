#include "HeriarchyPanel.hpp"
#include "../utils/Utils.hpp"
#include "../utils/InputManager.hpp"
extern Scene* editorScene;
extern GameObject* selectedGameObject;


void HeriarchyPanel::DeleteSelected(std::vector<GameObject*>& roots) {
    if(!selectedGameObject) return;

    // delete child from tree node
    for (auto itr = roots.begin(); itr != roots.end(); itr++) {
        if (selectedGameObject == *itr) {
            (*itr) = roots.back();
            roots.pop_back();
            break;
        }
    }


    // delete from memory
    std::cout << " GAY" << std::endl;
    for(auto itr = editorScene->gameObjects.begin(); itr != editorScene->gameObjects.end(); itr++) {
        if((*itr).get() == selectedGameObject) {
            *itr = std::move(editorScene->gameObjects.back());
            editorScene->gameObjects.pop_back();
            editorScene->MakeDirty();
            selectedGameObject = nullptr;
            break;
        }
    }
}

void PrintGameObjects() {
    std::cout << "[ ";
    for (auto itr = editorScene->gameObjects.begin(); itr != editorScene->gameObjects.end(); itr++) {
        std::cout << (*itr)->name << ", ";
    }
    std::cout << " ]" << std::endl;
}

void HeriarchyPanel::RenameSelected(const char* newName) {
    if(!selectedGameObject) return;

    selectedGameObject->name = newName;
    editorScene->MakeDirty();
}

void HeriarchyPanel::RenderHeriarchy(std::vector<GameObject*>& roots) {
    if(roots.empty()) return;
    
    for(const auto& gameObject : roots) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (gameObject->childs.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

        bool isSelected = selectedGameObject == gameObject;
        if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 1)); 
        bool opened = ImGui::TreeNodeEx(gameObject->name.c_str(), flags);
        ImGui::PopStyleVar();

        // left click
        if(!isSelected && ImGui::IsItemClicked(ImGuiMouseButton_Left))            
            selectedGameObject = gameObject;

        // right click
        if (ImGui::BeginPopupContextItem()) {
            selectedGameObject = gameObject;

            if(ImGui::MenuItem("Rename")) {
                showRenamePanel = true;
            }
            if(ImGui::MenuItem("Delete")) {
                PrintGameObjects();
                DeleteSelected(roots);
                PrintGameObjects();
            } 

            ImGui::EndPopup();
        }

        // 
        if(opened) {
            RenderHeriarchy(gameObject->childs);

            ImGui::TreePop();
        };
    };

    if (showRenamePanel) {
        Utils::GUI::ShowTextInputDialoge("Rename GameObject", "NewName", showRenamePanel, [this](std::string input) {
            RenameSelected(input.c_str());
        });
    }
}

void HeriarchyPanel::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Heriarchy");
    ImGui::PopStyleVar();

    // T O P _ P A N E L
    float width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginChild("TOP", ImVec2(width, 30));

    ImGui::SetCursorPos(ImVec2(8, 7));
    ImGui::Text(editorScene->GetNameString());

    ImGui::SameLine(width - 30);
    ImGui::SetCursorPosY(5);
    if(ImGui::Button("+", ImVec2(20, 20))) {
        showCreateGameObjPanel = true;
    }

    ImGui::EndChild();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y);

    // H E R I A R C H Y 

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0,0,0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::BeginChild("Content", ImVec2(width, 0), ImGuiChildFlags_AlwaysUseWindowPadding);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    // Save Scene
    if(InputManager::isSpecialPressed(GLFW_MOD_CONTROL) && InputManager::isKeyPressed(GLFW_KEY_S)) {
        editorScene->SaveScene();
    };

    // left click (content);
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
        selectedGameObject = nullptr;
    }

    if(!editorScene->gameObjects.empty()) {
        RenderHeriarchy(editorScene->roots);
    }


    ImGui::EndChild();

    ImGui::End();

    // E X T E R N A L _ P A N E L S
    if (showCreateGameObjPanel) CreateGameObjectPanel();
}

void HeriarchyPanel::CreateGameObjectPanel() {
    Utils::GUI::ShowTextInputDialoge("Create GameObject", "GameObject", showCreateGameObjPanel, [this](std::string goName) {
        // ADD GAMEOBJECT
        std::cout << "Game Object Created!: " << goName << std::endl;
        
        if(selectedGameObject) {
            GameObject* go = editorScene->AddGameObject(goName);
            selectedGameObject->childs.push_back(go);
        } else {
            GameObject* go = editorScene->AddGameObject(goName);
            editorScene->roots.push_back(go);
        }
    });
}