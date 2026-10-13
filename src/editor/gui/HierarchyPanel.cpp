#include "../../core/GameObject.hpp"
#include "../../core/InputManager.hpp"
#include "../../core/SceneManager.hpp"
#include "../../utils/globals.hpp"
#include "HierarchyPanel.hpp"
#include "../../utils/utils.hpp"
#include "imgui.h"
#include <cstring>

void HierarchyPanel::renderPanels() {
    if (showCreateGameobjectPopup) { ImGui::OpenPopup("CreateGameObject"); showCreateGameobjectPopup = false; }
    if (showRenameGameobjectPopup) { ImGui::OpenPopup("RenameGameObject"); showRenameGameobjectPopup = false; }

    centerWindow();
    if (ImGui::BeginPopupModal("CreateGameObject", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("GameObject Name", createInputBuffer, sizeof(createInputBuffer));

        if (ImGui::Button("Create")) {
            if (editor.selectedGameObject) {
                engine.sceneManager.activeScene->AddGameObject(createInputBuffer, editor.selectedGameObject);
            } else {
                engine.sceneManager.activeScene->AddGameObject(createInputBuffer, nullptr);
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    centerWindow();
    if (ImGui::BeginPopupModal("RenameGameObject", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("New Name", renameInputBuffer, sizeof(renameInputBuffer));

        if (ImGui::Button("Rename")) {
            RenameSelected(renameInputBuffer);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void HierarchyPanel::RenameSelected(const char* newName) {
    if(!editor.selectedGameObject) return;

    editor.selectedGameObject->name = newName;
    engine.sceneManager.activeScene->MakeDirty();
}

void HierarchyPanel::RenderHeriarchy(std::vector<GameObject*>& roots) {
    if(roots.empty()) return;
    

    for(const auto& gameObject : roots) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (gameObject->childs.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

        bool isSelected = editor.selectedGameObject == gameObject;
        if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 1)); 
        bool opened = ImGui::TreeNodeEx((gameObject->name + "##" + std::to_string(uiIdCounter++)).c_str(), flags);
        ImGui::PopStyleVar();

        // left click
        if(!isSelected && ImGui::IsItemClicked(ImGuiMouseButton_Left))            
            editor.selectedGameObject = gameObject;

        // right click
        if (ImGui::BeginPopupContextItem()) {
            editor.selectedGameObject = gameObject;

            if(ImGui::MenuItem("Rename")) {
                std::strncpy(renameInputBuffer, editor.selectedGameObject->name.c_str(), sizeof(renameInputBuffer) - 1);
                renameInputBuffer[sizeof(renameInputBuffer) - 1] = '\0';
				showRenameGameobjectPopup = true;
            }
            if(ImGui::MenuItem("Delete")) {
                engine.sceneManager.activeScene->RemoveGameObject(editor.selectedGameObject);
                editor.selectedGameObject = nullptr;
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
}

void HierarchyPanel::handleKeyEvents() {
    if(InputManager::isKeyPressed(SDL_SCANCODE_DELETE) && editor.selectedGameObject) {
        engine.sceneManager.activeScene->RemoveGameObject(editor.selectedGameObject);
        editor.selectedGameObject = nullptr;
    }
}

void HierarchyPanel::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Hierarchy");
    ImGui::PopStyleVar();

    // K E Y _ E V E N T S
    handleKeyEvents();

    // T O P _ P A N E L
    float width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginChild("TOP", ImVec2(width, 30));

    ImGui::SetCursorPos(ImVec2(8, 7));
    ImGui::Text(engine.sceneManager.activeScene->GetNameString());

    ImGui::SameLine(width - 30);
    ImGui::SetCursorPosY(5);
    if(ImGui::Button("+", ImVec2(20, 20))) {
        std::strncpy(createInputBuffer, "GameObject", sizeof(createInputBuffer) - 1);
        createInputBuffer[sizeof(createInputBuffer) - 1] = '\0';
		showCreateGameobjectPopup = true;
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
    if(InputManager::isSpecialDown(SDL_KMOD_CTRL) && InputManager::isKeyPressed(SDL_SCANCODE_S)) {
        engine.sceneManager.activeScene->SaveScene();
    };

    // left click (content);
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered()) {
        editor.selectedGameObject = nullptr;
    }

    if(!engine.sceneManager.activeScene->gameObjects.empty()) {
        RenderHeriarchy(engine.sceneManager.activeScene->roots);
    }


    ImGui::EndChild();

    ImGui::End();

    renderPanels();
}