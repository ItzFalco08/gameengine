#include "InspectorPanel.hpp"
#include "AssetDragPayload.hpp"
#include "imgui.h"
#include "../../utils/UUID.hpp"
#include "../../utils/globals.hpp"
#include "../../core/components/Mesh.hpp"
#include "../../core/components/Light.hpp"

void InspectorPanel::renderPanels() {
    if (showDeleteComponentPopup) {
        ImGui::OpenPopup("MeshComponentContextMenu");
        showDeleteComponentPopup = false;
    }

    if (showAddComponentPopup) {
        ImGui::OpenPopup("AddComponentPopup");
        showAddComponentPopup = false;
    }

    if(ImGui::BeginPopup("MeshComponentContextMenu", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        if(ImGui::MenuItem("Remove Component")) {
            // Handle delete action
            editor.selectedGameObject->components.erase(typeToDelete);
            engine.sceneManager.activeScene->MakeDirty();
        }
        ImGui::EndPopup();
    }

    if(ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        char buf[50] = {}; // temp value, internally managed by imgui
        ImGui::InputTextWithHint("Search", "Search Component.", &buf[0], 50);

        struct ComponentEntry {
            std::function<std::unique_ptr<Component>()> createFunc;
            std::string name;
        };

        static std::map<std::type_index, ComponentEntry> componentRegistry = {
            { std::type_index(typeid(MeshComponent)), {[]() { return std::make_unique<MeshComponent>(); }, "Mesh" } },
            { std::type_index(typeid(LightComponent)), {[]() { return std::make_unique<LightComponent>(); }, "Light" } },
        };

        for(auto& [key,val] : componentRegistry) {
            if (!editor.selectedGameObject->HasComponent(key) && (val.name.find(buf) != std::string::npos || buf[0] == '\0')) {
                if(ImGui::Selectable(val.name.c_str())) {
                    editor.selectedGameObject->components[key] = val.createFunc();
                    editor.selectedGameObject->components[key]->parent = editor.selectedGameObject;
                    engine.sceneManager.activeScene->MakeDirty();
                }
            }
        };

        ImGui::EndPopup();
    }
};

void InspectorPanel::Render() {
    renderPanels();

    ImGui::Begin("Inspector");
    
    if (editor.selectedGameObject) {
        ImGui::InputText("Name", editor.selectedGameObject->name.data(), editor.selectedGameObject->name.size() + 1, ImGuiInputTextFlags_ReadOnly);

        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        ImGui::Separator();

        ImGui::Dummy(ImVec2(0.0f, 4.0f)); 

        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::DragFloat3("position", &editor.selectedGameObject->transform->position.x, 0.1f)) {
                editor.selectedGameObject->transform->modelDirty = true;
                engine.sceneManager.activeScene->MakeDirty();
            }

            if (ImGui::DragFloat3("scale", &editor.selectedGameObject->transform->scale.x, 0.1f)) {
                editor.selectedGameObject->transform->modelDirty = true;
                editor.selectedGameObject->transform->normalDirty= true;
                engine.sceneManager.activeScene->MakeDirty();
            }

            glm::vec3 gEulars = editor.selectedGameObject->transform->gEulars;
            if (ImGui::DragFloat3("Rotation", &gEulars.x)) {
                editor.selectedGameObject->transform->SetRotationGlobal(gEulars);
                editor.selectedGameObject->transform->modelDirty = true;
                editor.selectedGameObject->transform->normalDirty= true;
                engine.sceneManager.activeScene->MakeDirty();
            }

            ImGui::Dummy(ImVec2(0.0f, 6.0f));
        }

        // for each component
        for(auto& [type, component] : editor.selectedGameObject->components) {
            if (component->GetType() == ComponentType::Mesh) {
                MeshComponent* meshComp = static_cast<MeshComponent*>(component.get());
                bool compOpen = ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_DefaultOpen);

                if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    showDeleteComponentPopup = true;
                    typeToDelete = type;
                }

                if (compOpen) {
                    ImGui::Text("Mesh UUID: %s", UUID::toString(meshComp->meshUUID).c_str());
                    if (ImGui::BeginDragDropTarget()) {

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_UUID")) {
                            const auto* asset = static_cast<const AssetDragPayload*>(payload->Data);
                            if (asset->type == AssetType::MESH) {
                                meshComp->meshUUID = UUID::fromString(asset->uuid);
                                engine.sceneManager.activeScene->MakeDirty();
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }
                    ImGui::Dummy(ImVec2(0.0f, 6.0f));
                }
            
            } else if (component->GetType() == ComponentType::Light) {
                LightComponent* lightComp = static_cast<LightComponent*>(component.get());
                
                bool compOpen = ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen);
                
                if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    showDeleteComponentPopup = true;
                    typeToDelete = type;
                }

                if (compOpen) {
                    const char* lightTypes[] = { "Point", "Directional" }; // makes sure its same order as LightType enum
                    lightTypeIdx = static_cast<int>(lightComp->type);
                    if (ImGui::Combo("Light Type", &lightTypeIdx, lightTypes, IM_ARRAYSIZE(lightTypes))) {
                        lightComp->type = static_cast<LightType>(lightTypeIdx);
                        engine.sceneManager.activeScene->MakeDirty();
                    }

                    if (ImGui::ColorEdit3("Color", &lightComp->color.x)) {
                        engine.sceneManager.activeScene->MakeDirty();
                    }

                    if (ImGui::DragFloat("Intensity", &lightComp->intensity, 0.1f)) {
                        engine.sceneManager.activeScene->MakeDirty();
                    }

                    if (lightComp->type == LightType::POINT) {
                        if (ImGui::DragFloat("Range", &lightComp->range, 0.1f)) {
                            engine.sceneManager.activeScene->MakeDirty();
                        }
                    }
                    
                    ImGui::Dummy(ImVec2(0.0f, 6.0f));
                }

            }
        }


        if(ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x,24))) 
            showAddComponentPopup = true;
        
    } else if (editor.selectedAssetMeta) {
        ImGui::InputText("Name", (char*)editor.selectedAssetMeta->m_path.filename().string().c_str(), editor.selectedAssetMeta->m_path.string().length() + 1, ImGuiInputTextFlags_ReadOnly);

    } else {
        ImGui::Text("No GameObject or Asset selected.");
    }

    ImGui::End();
}
