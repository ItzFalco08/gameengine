#include "../../utils/Utils.hpp"
#include "../../utils/globals.hpp"
#include "AssetsBrowserPanel.hpp"
#include <SDL3/SDL.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <cstring>
#include <future>
#include <json/json.hpp>
#include <vector>
#include "glad/gl.h"

namespace {
    bool showYesNoDialog(const char* title, const char* message) {
        const SDL_MessageBoxButtonData buttons[] = {
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No" }
        };
        const SDL_MessageBoxData data = {
            SDL_MESSAGEBOX_INFORMATION,
            nullptr,
            title,
            message,
            2,
            buttons,
            nullptr
        };

        int buttonId = 0;
        return SDL_ShowMessageBox(&data, &buttonId) && buttonId == 1;
    }
}

void AssetsBrowser::renderPanels() {
	if (ImGui::BeginPopupModal("CreateFolder", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char inputBuffer[256] = { 0 };
        ImGui::InputText("Folder Name", inputBuffer, sizeof(inputBuffer));
        if (inputBuffer[0] == '\0') {
            strncpy_s(inputBuffer, "new folder", sizeof(inputBuffer) - 1);
            inputBuffer[sizeof(inputBuffer) - 1] = '\0';
        }

        if (ImGui::Button("Create")) {
			createFolder(currentPath / inputBuffer);
            ImGui::CloseCurrentPopup();
            inputBuffer[0] = '\0';
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            inputBuffer[0] = '\0';
        }
		ImGui::EndPopup();
	}

    if (ImGui::BeginPopupModal("CreateScene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char inputBuffer[256] = { 0 };
        ImGui::InputText("Scene Name", inputBuffer, sizeof(inputBuffer));
        if (inputBuffer[0] == '\0') {
            strncpy_s(inputBuffer, "myscene", sizeof(inputBuffer) - 1);
            inputBuffer[sizeof(inputBuffer) - 1] = '\0';
        }

        if (ImGui::Button("Create")) {
            createScene(currentPath / inputBuffer / ".scene");
            ImGui::CloseCurrentPopup();
            inputBuffer[0] = '\0';
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            inputBuffer[0] = '\0';
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("CreateMaterial", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

    }

    if (ImGui::BeginPopupModal("CreateTexture", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

    }

    if (ImGui::BeginPopup("RenamePopup")) {
        static char inputBuffer[256] = { 0 };
        ImGui::InputText("New Name", inputBuffer, sizeof(inputBuffer));
        if (inputBuffer[0] == '\0') {
            strncpy_s(inputBuffer, "new name", sizeof(inputBuffer) - 1);
            inputBuffer[sizeof(inputBuffer) - 1] = '\0';
        }

        if (ImGui::Button("Rename")) {
            rename(std::string(inputBuffer));
            ImGui::CloseCurrentPopup();
            inputBuffer[0] = '\0';
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            inputBuffer[0] = '\0';
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("ImportModelPopup")) {
        ImGui::Text("Import Model");

        unsigned int flags = 0;

        struct AssimpFlagOption {
            const char* label;
            unsigned int flag;
            bool enabled;
            const char* description;
        };

        static AssimpFlagOption s_assimpFlags[] = {
            { "Triangulate",           aiProcess_Triangulate,          true,  "Convert polygons to triangles" },
            { "Generate Smooth Normals (smooth shade)", aiProcess_GenSmoothNormals,   true,  "interpolated normals (smooth shade in blender) if normals are missing" },
            { "Calc Tangent Space",    aiProcess_CalcTangentSpace,     true,  "Needed for normal mapping" },
            { "Join Identical Vertices", aiProcess_JoinIdenticalVertices, true, "Dedupe verts for indexed rendering" },
            { "Flip UVs",              aiProcess_FlipUVs,               false, "Flip V coordinate (OpenGL vs DirectX)" },
            { "Limit Bone Weights",    aiProcess_LimitBoneWeights,      false, "Cap bone influences per vertex" },
            { "Convert To Left Handed", aiProcess_ConvertToLeftHanded,  false, "Switch coordinate handedness" },
            { "Optimize Meshes",       aiProcess_OptimizeMeshes,        false, "Merge meshes for fewer draw calls" },
            { "Optimize Graph",        aiProcess_OptimizeGraph,         false, "Flatten node hierarchy" },
            { "Split Large Meshes",    aiProcess_SplitLargeMeshes,      false, "Split meshes exceeding vertex/index limits" },
            { "Remove Redundant Materials", aiProcess_RemoveRedundantMaterials, false, "Merge duplicate materials" },
        };

		for (auto& option : s_assimpFlags) {
			ImGui::Checkbox(option.label, &option.enabled);
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("%s", option.description);
			}
        };

        if (ImGui::Button("Import")) {
            // Import the model
            
            unsigned int flags = 0;
             
            for (auto& option : s_assimpFlags) {
                if (option.enabled) {
                    flags |= option.flag;
                };
            }

            if (!(flags & aiProcess_GenSmoothNormals)) flags |= aiProcess_GenNormals; // Generate normals if smooth normals are not requested

            std::future<void> importFuture = std::async(std::launch::async, [&]() {
                // load model from file
                const aiScene* scene = editor.importer.ReadFile(importModelPath.string(), flags);

                if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                    progress = -1;
                    return;
                }

                std::unique_lock<std::mutex> impprogressLock(impProgressMtx);

                // --- EXTRACT ASSETS ---

                const fs::path modelDir = currentPath / (scene->mName.length > 0 ? scene->mName.C_Str() : importModelPath.stem().string());
                const fs::path materialsDir = modelDir / "materials";
                const fs::path texturesDir = modelDir / "textures";

                fs::create_directories(materialsDir);
                fs::create_directories(texturesDir);

                // assimp material index -> material asset uuid
				std::unordered_map<unsigned int, UUID> materialMap;

                impprogressLock.lock();
                progress += 2;
                impprogressLock.unlock();

                // materials
                for (int i = 0; i < scene->mNumMaterials; i++) {
                    aiMaterial* material = scene->mMaterials[i];
                    aiColor3D baseColor(1.0f, 1.0f, 1.0f);
                    aiColor3D diffuse(1.0f, 1.0f, 1.0f);
                    aiColor3D specular(1.0f, 1.0f, 1.0f);
                    float shininess = 32.0f;

                    material->Get(AI_MATKEY_BASE_COLOR, baseColor);
                    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
                    material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
                    material->Get(AI_MATKEY_SHININESS, shininess);

                    fs::path matPath = materialsDir / ((material->GetName().length > 0 ? material->GetName().C_Str() : "material_" + std::to_string(i)) + ".mat");
                    fs::path tempPath = matPath / ".tmp";
                    std::ofstream matFile(tempPath, std::ios::binary);

                    if (!matFile.is_open()) {
                        progress = -1;
                        std::filesystem::remove_all(modelDir);
                        LOG::EditorError("Failed to write material file: " + tempPath.string());
                        return;
                    }

                    UUID matUUID = generateUUID();
                    nlohmann::json matMetaJson;
                    matMetaJson["uuid"] = UUID::toString(matUUID);
                    matMetaJson["ambientColor"] = { baseColor.r, baseColor.g, baseColor.b };
                    matMetaJson["diffuseColor"] = { diffuse.r, diffuse.g, diffuse.b };
                    matMetaJson["specularColor"] = { specular.r, specular.g, specular.b };
                    matMetaJson["shininess"] = shininess;

                    matMetaJson["ambientMap"] = "";
                    matMetaJson["diffuseMap"] = "";
                    matMetaJson["specularMap"] = "";
                    matMetaJson["normalMap"] = "";

                    materialMap[i] = matUUID;

                    auto createTextureAsset = [&](aiTextureType textureType, const std::string& mapKey) {
                        if (material->GetTextureCount(textureType) > 0) {
                            aiString texturePath;
                            material->GetTexture(textureType, 0, &texturePath);
                            fs::path srcPath = texturesDir / fs::path(texturePath.C_Str()).filename();
                            fs::path metaPath = srcPath.string() + ".tex";
                            UUID texUUID;
                            if (fs::exists(srcPath)) {
                                // existing texture asset
                                std::ifstream file(metaPath);
                                nlohmann::json j;
                                file >> j;
                                texUUID = UUID::fromString(j["uuid"].get<std::string>());
                            }
                            else {
                                // new texture asset
                                texUUID = generateUUID();
                                fs::copy_file(texturePath.C_Str(), srcPath);
                                std::ofstream textureMetaFile(metaPath, std::ios::binary);
                                nlohmann::json textureMetaJson;
                                textureMetaJson["uuid"] = UUID::toString(texUUID);
                                textureMetaJson["textureType"] = "Texture2D";
                                textureMetaJson["wrapU"] = GL_REPEAT;
                                textureMetaJson["wrapV"] = GL_REPEAT;
                                textureMetaJson["minFilter"] = GL_LINEAR_MIPMAP_LINEAR;
                                textureMetaJson["magFilter"] = GL_LINEAR;
                                textureMetaFile << textureMetaJson.dump(4);
                                textureMetaFile.close();
                            }
                            matMetaJson[mapKey] = UUID::toString(texUUID);
                        }
                    };

                    createTextureAsset(aiTextureType_AMBIENT, "ambientMap");
                    createTextureAsset(aiTextureType_DIFFUSE, "diffuseMap");
                    createTextureAsset(aiTextureType_SPECULAR, "specularMap");
                    createTextureAsset(aiTextureType_NORMALS, "normalMap");

                    impprogressLock.lock();
                    progress += 3;
                    impprogressLock.unlock();
                }

                // mesh
                std::ofstream meshFile(modelDir / importModelPath.stem().string() / ".mesh", std::ios::binary);
                UUID uuid = generateUUID();
                meshFile.write(reinterpret_cast<const char*>(&uuid), sizeof(UUID));

                struct Vertex {
                    float x, y, z;
                    float u, v;
                    float nx, ny, nz;
                };

                // write nV
                uint32_t nV = 0;
                for (int i = 0; i < scene->mNumMeshes; i++) {
                    const aiMesh* mesh = scene->mMeshes[i];
                    nV += mesh->mNumVertices;
                }
                meshFile.write(reinterpret_cast<const char*>(&nV), sizeof(uint32_t));

                // write nI
                uint32_t nI = 0;
                for (int i = 0; i < scene->mNumMeshes; i++) {
                    const aiMesh* mesh = scene->mMeshes[i];
                    nI += mesh->mNumFaces * 3;
                }
                meshFile.write(reinterpret_cast<const char*>(&nI), sizeof(uint32_t));

                // write vertices
                for (int i = 0; i < scene->mNumMeshes; i++) {
                    const aiMesh* mesh = scene->mMeshes[i];

                    for (int iv = 0; iv < mesh->mNumVertices; iv++) {
                        Vertex vertex = { mesh->mVertices[iv].x, mesh->mVertices[iv].y, mesh->mVertices[iv].z, mesh->mTextureCoords[0][iv].x, mesh->mTextureCoords[0][iv].y, mesh->mNormals[iv].x, mesh->mNormals[iv].y, mesh->mNormals[iv].z };
                        meshFile.write(reinterpret_cast<const char*>(&vertex), sizeof(Vertex));
                    }

                }

                impprogressLock.lock();
                progress += 2;
                impprogressLock.unlock();

                uint32_t lastVertIdx = 0;
                uint32_t lastIndicesIdx = 0;
                // meshIdx -> lastIndicesIdx
                std::vector<uint32_t> lastIndicesIdxMap(scene->mNumMeshes);

                // write indices
                for (int i = 0; i < scene->mNumMeshes; i++) {
                    const aiMesh* mesh = scene->mMeshes[i];

                    lastIndicesIdxMap[i] = lastIndicesIdx;

                    for (int f = 0; f < mesh->mNumFaces; f++) {
                        const aiFace& face = mesh->mFaces[f];
                        for (int j = 0; j < face.mNumIndices; j++) {
                            uint32_t index = lastVertIdx + face.mIndices[j];
                            meshFile.write(reinterpret_cast<const char*>(&index), sizeof(uint32_t));
                        }
                    }

                    lastVertIdx += mesh->mNumVertices;
                    lastIndicesIdx += mesh->mNumFaces * 3; // assuming triangulated meshes
                }

                impprogressLock.lock();
                progress += 2;
                impprogressLock.unlock();

                // write submeshes
                for (int i = 0; i < scene->mNumMeshes; i++) {
                    const aiMesh* mesh = scene->mMeshes[i];

                    uint32_t indicesOffset = lastIndicesIdxMap[i];
                    uint32_t indicesSize = mesh->mFaces->mNumIndices * mesh->mNumFaces;

                    char buffer[512] = { 0 };
                    aiString materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName();
                    strncpy(buffer, materialName.C_Str(), sizeof(buffer) - 1);

                    meshFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
                    meshFile.write(reinterpret_cast<const char*>(&indicesOffset), sizeof(uint32_t));
                    meshFile.write(reinterpret_cast<const char*>(&indicesSize), sizeof(uint32_t));
                    meshFile.write(reinterpret_cast<const char*>(&materialMap[i]), sizeof(UUID));
                }

                impprogressLock.lock();
                progress += 1;
                impprogressLock.unlock();

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                impprogressLock.lock();
                progress = -2;
                impprogressLock.unlock();

                return;
                
            });
            
            ImGui::CloseCurrentPopup();
			ImGui::OpenPopup("ImportProgressPopup");
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("ImportProgressPopup")) {
        std::unique_lock<std::mutex> lock(impProgressMtx);

        if (progress == -1) {
            lock.unlock();

            ImGui::Text("Import failed.");

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();

                lock.lock();
                progress = 0;
                lock.unlock();
            }
		}
		else if (progress == -2) {
			lock.unlock();
			ImGui::Text("Import complete.");
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
				lock.lock();
				progress = 0;
				lock.unlock();
			}
		}
        else {
            int currentProgress = progress;
            lock.unlock();

            ImGui::ProgressBar(
                (float)currentProgress / 10.0f,
                ImVec2(-1, 0),
                currentProgress == 2 ? "Loading Model" : "Importing Assets"
            );
        }

        ImGui::EndPopup();
    }
}

void AssetsBrowser::createScene(const fs::path& path) {
    if (fs::exists(path.string())) { 
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "File Already Exists", "A file or folder with this name already exists.", appState.window );
        return; 
    };

	nlohmann::json sceneJson;
	sceneJson["gameObjects"] = nlohmann::json::array();
	std::ofstream(path.string()) << sceneJson.dump(4);
}

void AssetsBrowser::createFolder(const fs::path& path) {
    if (fs::exists(path)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "File Already Exists", "A file or folder with this name already exists.", appState.window);
        return;
    };
    fs::create_directory(path);
}

void AssetsBrowser::deleteAsset(const fs::path& path)
{
}

std::vector<AssetItem> AssetsBrowser::getAssetItems(const fs::path& path)
{
    std::vector<AssetItem> assets;

    for (const auto& entry : fs::directory_iterator(path)) {
        std::string ext = entry.path().extension().string();

		if (entry.is_directory()) {
			assets.push_back({ entry.path(), AssetItemType::Folder });
		} else if(ext == ".scene") {
            assets.push_back({ entry.path(), AssetItemType::Scene });
        } else if (ext == ".mesh") {
            assets.push_back({ entry.path(), AssetItemType::Mesh });
        } else if (ext == ".mat") {
            assets.push_back({ entry.path(), AssetItemType::Material });
        } else if (ext == ".tex") {
            assets.push_back({ entry.path(), AssetItemType::Texture });
        }
    }

    return assets;
}

void AssetsBrowser::InitIcons() {
    currentPath = ASSETS_DIR;
    TexDets texDets;
    texDets.minFilter = GL_NEAREST;
    fileTex = Texture2D(ROOT_DIR "src/textures/File.png", texDets);
    folderTex = Texture2D(ROOT_DIR "src/textures/Folder.png", texDets);
    folderEmptyTex = Texture2D(ROOT_DIR "src/textures/FolderEmpty.png", texDets);
}

void AssetsBrowser::handleFileDrop(const char* filePath)
{
    importModelPath = filePath;
    if (importModelPath.extension() == ".fbx" || importModelPath.extension() == ".obj" || importModelPath.extension() == ".dae" ||
        importModelPath.extension() == ".gltf" || importModelPath.extension() == ".glb" || importModelPath.extension() == ".stl") {

        ImGui::BeginPopup("ImportModelPopup");
    }
}

void AssetsBrowser::Render() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("AssetManager");
    ImGui::PopStyleVar();
    
    // Custom toolbar panel at top
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
    
    float toolbarHeight = 28;

        ImGui::BeginChild("##toolbar", ImVec2(0, toolbarHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        
        
        if(currentPath.string() != (ROOT_DIR "/Assets")) {
            if(ImGui::Button("<-", ImVec2(20, 20))) {
                
                currentPath = currentPath.parent_path();
            }
        } 

        ImGui::SameLine();
        ImGui::Text(currentPath.filename().string().c_str());

        ImGui::SameLine(ImGui::GetWindowWidth() - 270);
        ImGui::Text("icon size");
        ImGui::SameLine();

        ImGui::PushItemWidth(180);
        ImGui::SliderFloat("##iconSize", &iconSize, 50.0f, 200.0f);
        ImGui::PopItemWidth();
        
        ImGui::EndChild();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

        ImGui::BeginChild("##content", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::Indent(8);
        ImGui::Dummy(ImVec2(0, 8));


        float panelWidth = ImGui::GetContentRegionAvail().x;

        // right click
        if(ImGui::BeginPopupContextWindow()) {
            if(ImGui::BeginMenu("Create")) {
                if(ImGui::MenuItem("Folder")) ImGui::OpenPopup("CreateFolder");
                if(ImGui::MenuItem("Scene")) ImGui::OpenPopup("CreateScene");
                if(ImGui::MenuItem("Material")) ImGui::OpenPopup("CreateMaterial");
                if(ImGui::MenuItem("Texture")) ImGui::OpenPopup("CreateTexture");
                ImGui::EndMenu();
            }
            
            std::string openTxt = std::string("Open ") + currentPath.filename().string() + std::string(" Folder");

            if(ImGui::MenuItem(openTxt.c_str())) {
                OpenFile(fs::absolute(currentPath).string());
            }
            ImGui::EndPopup();
        }

        std::vector<AssetItem> items = getAssetItems(currentPath);

        float cursorW = 0;
        float itemWidthWithSpacing = iconSize + ImGui::GetStyle().ItemSpacing.x;
        
        // assets
        for (auto& item : items) {
            ImTextureID icon;
            
            if (item.assetType == AssetItemType::Folder) {
                std::error_code ec;
                bool isEmpty = fs::is_empty(item.itemPath, ec);
                icon = (ImTextureID)(intptr_t)(isEmpty && !ec ? folderEmptyTex.TexId : folderTex.TexId);
            } else {
                icon = (ImTextureID)(intptr_t)fileTex.TexId;
            }
            
            // Check if THIS item would overflow - if so, wrap to new line
            if (cursorW > 0 && cursorW + itemWidthWithSpacing > panelWidth) {
                cursorW = 0; // Start new line
            }
            
            // If not first item on line, call SameLine
            if (cursorW > 0) {
                ImGui::SameLine();
            }
            
            DrawAssetItem(icon, item.itemPath.filename().string().c_str(), {iconSize, iconSize}, item.itemPath);

            cursorW += itemWidthWithSpacing;
        }

        // P A N E L S
        renderPanels();

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            selectedItem = ""; // deselect on empty area click
        }
        
        ImGui::Unindent(8);
        ImGui::EndChild();

    
    ImGui::End();
}

void AssetsBrowser::OpenScene(fs::path& path) {
    LOG::EditorInfo("Opening Scene: ", path.filename().string());
    engine.sceneManager.SetScene(path);
}

void AssetsBrowser::rename(std::string input) {
    // Build the new path using the user-provided name and original parent directory
    fs::rename(toRename, toRename.parent_path() / (input + toRename.extension().string()));
}

void AssetsBrowser::DrawAssetItem(
    ImTextureID icon,
    const char* label,
    ImVec2 iconSize,
    fs::path itemPath
)
{
    ImGui::BeginGroup(); // makes image + text behave as one item

    bool isActive = (selectedItem == itemPath);
    
    // Active style
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
    } else {
        // Transparent style you already use
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered , ImVec4(0.18f, 0.18f, 0.18f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
    }

    bool clicked = ImGui::ImageButton(
        label,          // unique ID
        icon,
        iconSize, {0,1}, {1,0}, ImVec4(0, 0, 0, 0)
    );

    if (ImGui::BeginDragDropSource()) {
        const wchar_t* itemPathW = itemPath.c_str();
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPathW, (wcslen(itemPathW) + 1) * sizeof(wchar_t));
        ImGui::Text("%s", label); 
        ImGui::EndDragDropSource();
    }

    ImGui::PopStyleColor(3);

    if (clicked) {
        if (!isActive) {
            selectedItem = itemPath;
        } else if (fs::is_directory(selectedItem)) {
            currentPath = selectedItem;
        } else if (selectedItem.extension() == ".scene") {
            engine.sceneManager.SetScene(selectedItem);
        }
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Open"))
        {
            OpenFile(fs::absolute(itemPath).string()); 
        }

        if (ImGui::MenuItem("Rename"))
        {
			ImGui::BeginPopup("RenamePopup");
            toRename = itemPath;
        }

        if (ImGui::MenuItem("Delete"))
        {
            if(showYesNoDialog("Confirm", "This will permanently delete this item.\nContinue?"))
                deleteAsset(itemPath);
        }

        ImGui::EndPopup();
    }


    // Center text under image
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float itemWidth = iconSize.x;

    ImGui::SetCursorPosX(
        ImGui::GetCursorPosX() + (itemWidth - textSize.x) * 0.5f
    );
    ImGui::TextUnformatted(label);

    ImGui::EndGroup();

}



void AssetsBrowser::OpenFile(std::string filePath)
{
    const std::string url = "file:///" + fs::path(filePath).generic_string();
    SDL_OpenURL(url.c_str());
}
