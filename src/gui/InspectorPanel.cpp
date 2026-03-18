#include "InspectorPanel.hpp"
#include "../core/GameObject.hpp"
#include "../core/SceneManager.hpp"
#include "glm/glm.hpp"
#include "../core/components/Mesh.hpp"
#include "../core/components/Material.hpp"
#include "../core/Texture.hpp"
#include <cstdio>
extern GameObject* selectedGameObject;
extern SceneManager sceneManager;


extern std::map<std::string, std::function<std::unique_ptr<Component>()>> componentRegistry;


void InspectorPanel::Render() {
    ImGui::Begin("Inspector");

    if (selectedGameObject) {
        ImGui::InputText("Name", selectedGameObject->name.data(), selectedGameObject->name.size() + 1, ImGuiInputTextFlags_ReadOnly);

        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        ImGui::Separator();

        ImGui::Dummy(ImVec2(0.0f, 4.0f)); 

        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::DragFloat3("position", &selectedGameObject->transform->position.x, 0.1f)) {
                selectedGameObject->transform->modelDirty = true;
                sceneManager.activeScene->MakeDirty();
            }

            if (ImGui::DragFloat3("scale", &selectedGameObject->transform->scale.x, 0.1f)) {
                selectedGameObject->transform->modelDirty = true;
                selectedGameObject->transform->normalDirty= true;
                sceneManager.activeScene->MakeDirty();
            }

            glm::vec3 gEulars = selectedGameObject->transform->gEulars;
            if (ImGui::DragFloat3("Rotation", &gEulars.x)) {
                selectedGameObject->transform->SetRotationGlobal(gEulars);
                selectedGameObject->transform->modelDirty = true;
                selectedGameObject->transform->normalDirty= true;
                sceneManager.activeScene->MakeDirty();
            }

            ImGui::Dummy(ImVec2(0.0f, 6.0f));
        }

        for (auto& [key, component] : selectedGameObject->components) {
            ImGui::PushID(key.c_str());
            if(ImGui::CollapsingHeader(key.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                // Delete Component Right Click Context
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Remove Component")) {
                        selectedGameObject->components.erase(key);
                        sceneManager.activeScene->MakeDirty();
                    }
                    ImGui::EndPopup();
                }

                if (key == "Mesh") {
                    if (auto* mesh = dynamic_cast<Mesh*>(component.get())) {
                        const char* items[] = { "FRONT", "BACK", "BOTH" };
                        if(ImGui::Combo("Face Culling", &faceCullingIdx, items, IM_ARRAYSIZE(items))) {
                            switch (faceCullingIdx) {
                                case 0: mesh->cullDir = GL_FRONT; break;
                                case 1: mesh->cullDir = GL_BACK; break;
                                case 2: mesh->cullDir = GL_FRONT_AND_BACK; break;
                            }
                            sceneManager.activeScene->MakeDirty();
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
                                    if(mesh->Initialize(p.string().c_str())) sceneManager.activeScene->MakeDirty();
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }

                        ImGui::SameLine();
                        ImGui::Text("Mesh Object");
                    }
                } else if (key == "Material") {
                    Material* mat = static_cast<Material*>(component.get());
                    MaterialType matType = static_cast<MaterialType>(mat->matprops->GetMatType());
                    shaderTypeIdx = static_cast<int>(matType);

                    // S H A D E R _ T Y P E
                    static std::array<const char*, 2> items = { "Lit", "Unlit" };

                    if(ImGui::Combo("Shader Type", &shaderTypeIdx, items.data(), (int)items.size())) {
                        mat->SetMaterialType(static_cast<MaterialType>(shaderTypeIdx));
                        matType = static_cast<MaterialType>(mat->matprops->GetMatType());
                        sceneManager.activeScene->MakeDirty();
                    }

                    // M A T E R I A L _ P R O P E R T I E S

                    switch (matType)
                    {
                    case MaterialType::LIT: {
                        LitMaterial* litMaterial = static_cast<LitMaterial*>(mat->matprops.get());

                        bool change = false;
                        
                        change |= ImGui::ColorEdit3("Ambient Color", &litMaterial->ambientColor.x);
                        change |= ImGui::DragFloat("ambientStrength", &litMaterial->ambientStrength, 0.005f, 0.0f, 1.0f);
                        change |= ImGui::DragFloat("diffuseStrength", &litMaterial->diffuseStrength, 0.005f, 0.0f, 1.0f);
                        change |= ImGui::DragFloat("specularStrength", &litMaterial->specularStrength, 0.005f, 0.0f, 1.0f);
                        change |= ImGui::DragFloat("shininess", &litMaterial->shininess, 0.1f, 0.0f, 64.0f);
                        if(change) sceneManager.activeScene->MakeDirty();

                        // T E X T U R E

                        ImGui::SeparatorText("Texture");

                        Texture* texture = resourceManager.LoadAndGetTexture(litMaterial->texturePath); // cur tex
                        std::string tPathTxt = texture ? texture->texturePath.c_str() : "Drop texture (png/jpg/jpeg)";

                        ImGui::InputText("Texture Path", tPathTxt.data(), tPathTxt.size(), ImGuiInputTextFlags_ReadOnly);

                        if(ImGui::BeginDragDropTarget()) {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                                const wchar_t* path = (const wchar_t*)payload->Data;
                                std::filesystem::path p(path);
                                
                                if ((p.extension() == ".png" || p.extension() == ".jpg" || p.extension() == ".jpeg") && litMaterial->texturePath != p.string().c_str()) {
                                    resourceManager.DeleteTexture(litMaterial->texturePath);
                                    litMaterial->texturePath = p.string().c_str();
                                    sceneManager.activeScene->MakeDirty();
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }

                        if (texture && texture->TexId != GL_NONE) {
                            ImGui::Text("Preview");
                            ImGui::Image((ImTextureID)(intptr_t)texture->TexId, ImVec2(96, 96));
                        }

                        // T E X T U R E _ P R O P E R T I E S
                        if (texture == nullptr) break;
                        if(ImGui::TreeNode("Texture Properties")) {
                            static std::array<const char*, 4> wrapArr = {"REPEAT", "MIRRORED_REPEAT", "CLAMP_TO_EDGE", "CLAMP_TO_BORDER"};
                            static std::array<unsigned int, 4> wrapValues = {GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER};

                            static std::array<const char*,6> minFilterArr = {"NEAREST", "LINEAR", "NEAREST_MIPMAP_NEAREST", "LINEAR_MIPMAP_NEAREST", "NEAREST_MIPMAP_LINEAR", "LINEAR_MIPMAP_LINEAR"};
                            static std::array<unsigned int, 6> minFilterValues = {GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
                            
                            static std::array<const char*, 2> magFilterArr = {"NEAREST", "LINEAR"};
                            static std::array<unsigned int, 2> magFilterValues = {GL_NEAREST, GL_LINEAR};

                            TexDets& tProps = litMaterial->texProps;
                            wrapSIdx = GetIdx(wrapValues.data(), (int)wrapValues.size(), tProps.wrapS);
                            wrapTIdx = GetIdx(wrapValues.data(), (int)wrapValues.size(), tProps.wrapT);

                            minFilterIdx = GetIdx(minFilterValues.data(), (int)minFilterValues.size(), tProps.minFilter);
                            magFilterIdx = GetIdx(magFilterValues.data(), (int)magFilterValues.size(), tProps.magFilter);

                            bool changed = false;

                            if(ImGui::Combo("wrapS", &wrapSIdx,  wrapArr.data(), (int)wrapArr.size())) {
                                dets.wrapS = wrapValues[wrapSIdx];
                                changed |= true;
                            }

                            if(ImGui::Combo("wrapT", &wrapTIdx,  wrapArr.data(), (int)wrapArr.size())) {
                                dets.wrapT = wrapValues[wrapTIdx];
                                changed |= true;
                            }

                            if(ImGui::Combo("minFilter", &minFilterIdx, minFilterArr.data(), (int)minFilterArr.size())) {
                                dets.minFilter = minFilterValues[minFilterIdx];
                                changed |= true;
                            };
                            
                            if(ImGui::Combo("magFilter", &magFilterIdx, magFilterArr.data(), (int)magFilterArr.size())) {
                                dets.magFilter = magFilterValues[magFilterIdx];
                                changed |= true;
                            };

                            if (changed) {
                                texture->applyParams(dets);
                                litMaterial->texProps = dets;
                                sceneManager.activeScene->MakeDirty();
                            };

                            ImGui::TreePop();
                        };


                        break;
                    }

                    case MaterialType::UNLIT: {
                        UnlitMaterial* matProps = static_cast<UnlitMaterial*>(mat->matprops.get());
                        
                        bool change = false;
                        change |= ImGui::ColorEdit3("Ambient Color", &matProps->ambientColor.x);
                        if(change) sceneManager.activeScene->MakeDirty();
                        break;
                    }

                    default:
                        break;
                    }

                } else if (key == "Light") {
                    Light* lightComp = static_cast<Light*>(component.get());

                    static std::array<const char*, 2> items = { "POINT", "DIRECTIONAL" };
                    if(ImGui::Combo("Light Type", &lightTypeIdx, items.data(), (int)items.size())) {
                        lightComp->setLight(static_cast<LightType>(lightTypeIdx));
                        sceneManager.activeScene->MakeDirty();
                    };

                    switch (lightComp->lightType) {
                        case LightType::POINT: {
                            PointLight* l = static_cast<PointLight*>(lightComp->lightProps.get());

                            bool change = false;

                            change |= ImGui::ColorEdit3("lightColor", &l->lightColor[0]);
                            change |= ImGui::DragFloat("range", &l->range, 0.1f, 0.0f, 500.0f);
                            change |= ImGui::DragFloat("intensity", &l->intensity, 0.1f, 0.0f, 500.0f);
                            
                            if(change) sceneManager.activeScene->MakeDirty();
                            break;
                        }

                        case LightType::DIRECTIONAL: {
                            DirLight* l = static_cast<DirLight*>(lightComp->lightProps.get());
                            bool change = ImGui::ColorEdit3("lightColor", &l->lightColor[0]);
                            if(change) sceneManager.activeScene->MakeDirty();
                            break;
                        }
                    }
                } else if (key == "Script") {
                    ImGui::Text("Mesh");

                } else if (key == "Camera") {
                    ImGui::Text("Mesh");

                }

                // gap
                ImGui::Dummy(ImVec2(0, 4));
            }
            ImGui::PopID(); // Ensure PopID is called for every PushID
        
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
                        selectedGameObject->components[key]->parent = selectedGameObject;
                        sceneManager.activeScene->MakeDirty();
                    }
                }
            };

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}


int InspectorPanel::GetIdx(const unsigned int* arr, int size, unsigned int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value)
            return i;
    }
    return 0; // fallback (or -1 if you handle it)
}