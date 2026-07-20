#include "HeriarchyPanel.hpp"
#include "../utils/Utils.hpp"
#include "../utils/InputManager.hpp"
#include "../core/SceneManager.hpp"
#include "../core/GameObject.hpp"
extern SceneManager sceneManager;
extern GameObject* selectedGameObject;

void HeriarchyPanel::RenameSelected(const char* newName) {
    if(!selectedGameObject) return;

    selectedGameObject->name = newName;
    sceneManager.activeScene->MakeDirty();
}

void HeriarchyPanel::RenderHeriarchy(std::vector<GameObject*>& roots) {
    if(roots.empty()) return;
    

    for(const auto& gameObject : roots) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (gameObject->childs.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

        bool isSelected = selectedGameObject == gameObject;
        if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 1)); 
        bool opened = ImGui::TreeNodeEx((gameObject->name + "##" + std::to_string(uiIdCounter++)).c_str(), flags);
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
                sceneManager.activeScene->RemoveGameObject(selectedGameObject);
                selectedGameObject = nullptr;
            }

            ImGui::EndPopup();
        }

        // 
        if(opened) {
            RenderHeriarchy(gameObject->childs);

            ImGui::TreePop();
        };
    };

    uiIdCounter = 0; // reset after rendering heriarchy

    if (showRenamePanel) {
        Utils::GUI::ShowTextInputDialoge("Rename GameObject", "NewName", showRenamePanel, [this](std::string input) {
            RenameSelected(input.c_str());
        });
    }
}

void HeriarchyPanel::handleKeyEvents() {
    if(InputManager::isKeyPressed(GLFW_KEY_DELETE) && selectedGameObject) {
        sceneManager.activeScene->RemoveGameObject(selectedGameObject);
        selectedGameObject = nullptr;
    }
}

void HeriarchyPanel::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Heriarchy");
    ImGui::PopStyleVar();

    // K E Y _ E V E N T S
    handleKeyEvents();

    // T O P _ P A N E L
    float width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginChild("TOP", ImVec2(width, 30));

    ImGui::SetCursorPos(ImVec2(8, 7));
    ImGui::Text(sceneManager.activeScene->GetNameString());

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
    if(InputManager::isSpecialDown(GLFW_MOD_CONTROL) && InputManager::isKeyPressed(GLFW_KEY_S)) {
        sceneManager.activeScene->SaveScene();
    };

    // left click (content);
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
        selectedGameObject = nullptr;
    }

    if(!sceneManager.activeScene->gameObjects.empty()) {
        RenderHeriarchy(sceneManager.activeScene->roots);
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
            GameObject* go = sceneManager.activeScene->AddGameObject(goName, selectedGameObject);
        } else {
            GameObject* go = sceneManager.activeScene->AddGameObject(goName, nullptr);
        }
    });
}