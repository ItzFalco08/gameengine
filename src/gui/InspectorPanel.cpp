#include "InspectorPanel.hpp"
#include "../core/GameObject.hpp"
#include "../core/Scene.hpp"
#include "glm/glm.hpp"
extern GameObject* selectedGameObject;
extern Scene* editorScene;

enum ComponentsRegistry {
    Mesh,
    Material,
    Light,
    Script,
};

void InspectorPanel::Render() {
    ImGui::Begin("Inspector");

    if (selectedGameObject) {
        ImGui::InputText("Name", selectedGameObject->name.data(), selectedGameObject->name.size() + 1, ImGuiInputTextFlags_ReadOnly);

        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        ImGui::Separator();

        ImGui::Dummy(ImVec2(0.0f, 4.0f)); 

        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(ImGui::DragFloat3("position", &selectedGameObject->transform->position.x)) {
                selectedGameObject->transform->MakeDirty();
                editorScene->MakeDirty();
            }

            if(ImGui::DragFloat3("scale", &selectedGameObject->transform->scale.x)) { 
                selectedGameObject->transform->MakeDirty();
                editorScene->MakeDirty();
            }

            glm::vec3 gEulars = glm::degrees(glm::eulerAngles(selectedGameObject->transform->rotation));
            
            if(ImGui::DragFloat3("Rotation", &gEulars.x)) {
                selectedGameObject->transform->SetRotation(gEulars);
                selectedGameObject->transform->MakeDirty();
                editorScene->MakeDirty();
            }
            
            ImGui::Dummy(ImVec2(0.0f, 6.0f));
        }

        if(ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x,24))) 
            ImGui::OpenPopup("AddComponentPopup");

        if(ImGui::BeginPopup("AddComponentPopup")) {
            char buf[50]; // temp value, internally managed by imgui
            ImGui::InputTextWithHint("", "Search Component.", &buf[0], 50);

            

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}