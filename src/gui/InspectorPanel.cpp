#include "InspectorPanel.hpp"
#include "../core/GameObject.hpp"
#include "../core/Scene.hpp"
#include "glm/glm.hpp"
#include "../core/components/Mesh.hpp"
extern GameObject* selectedGameObject;
extern Scene* editorScene;

enum class ComponentsRegistry {
    Mesh,
    Material,
    Light,
    Script,
};



extern std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>> componentRegistry;

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

        for (auto& [key, component] : selectedGameObject->components) {
            if(ImGui::CollapsingHeader(key.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                if (key == "Mesh") {
                    if (auto* mesh = dynamic_cast<Mesh*>(component.get())) {
                        const char* items[] = { "FRONT", "BACK", "BOTH" };
                        if(ImGui::Combo("Face Culling", &curIdx, items, IM_ARRAYSIZE(items))) {
                            mesh->cullDir = static_cast<CullDir>(curIdx);
                            editorScene->MakeDirty();
                        }

                        std::string objPathStr = mesh->objFilePath.has_value() ? mesh->objFilePath.value() : "Empty (Drop .obj here)";
                        
                        float labelWidth = ImGui::CalcTextSize("Mesh Object").x;

                        ImGui::Button(objPathStr.c_str());
                        ImGui::SameLine(labelWidth);
                        
                        // Make the button a drop target
                        if (ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                                const wchar_t* path = (const wchar_t*)payload->Data;
                                std::filesystem::path p(path);
                                
                                if (p.extension() == ".obj") {
                                    if(mesh->Initialize(p.string().c_str())) editorScene->MakeDirty();
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }
                        
                        ImGui::SameLine();
                        ImGui::Text("Mesh Object");
                        
                        // Label next to it or above it? 
                        // Since Button takes full width above, let's keep it simple or add a label before if you prefer:
                        // ImGui::Text("Mesh Object"); ImGui::SameLine(); ...
                    }

                } else if (key == "Material") {
                    ImGui::Text("Mesh");
                    
                } else if (key == "Light") {
                    ImGui::Text("Mesh");

                } else if (key == "Script") {
                    ImGui::Text("Mesh");

                } else if (key == "Camera") {
                    ImGui::Text("Mesh");

                }

                // gap
                ImGui::Dummy(ImVec2(0, 4));
            }
        }

        if(ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x,24))) 
            ImGui::OpenPopup("AddComponentPopup");

        if(ImGui::BeginPopup("AddComponentPopup")) {
            char buf[50] = {}; // temp value, internally managed by imgui
            ImGui::InputTextWithHint("Search", "Search Component.", &buf[0], 50);

            for(auto& [key,val] : componentRegistry) {
                if (selectedGameObject->components.find(key) == selectedGameObject->components.end() && strstr(key.c_str(), buf)) {
                    if(ImGui::Selectable(key.c_str())) {
                        selectedGameObject->components[key] = val();
                        editorScene->MakeDirty();
                    }
                }
            };

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}