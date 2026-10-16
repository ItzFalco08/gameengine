#include "../../utils/UUID.hpp"
#include "../../utils/globals.hpp"
#include "AssetsBrowserPanel.hpp"
#include "glad/gl.h"
#include <SDL3/SDL.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <future>
#include <limits>
#include <json/json.hpp>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include "../../utils/utils.hpp"
#include "../../core/assets/meta/MaterialAsset.hpp"
#include "../../core/assets/runtime/Mesh.hpp"

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
	if (showCreateFolderPopup) { ImGui::OpenPopup("CreateFolder"); showCreateFolderPopup = false; }
	if (showCreateScenePopup) { ImGui::OpenPopup("CreateScene"); showCreateScenePopup = false; }
	if (showImportModelPopup) { ImGui::OpenPopup("ImportModelPopup"); showImportModelPopup = false; }
	if (showRenamePopup) { ImGui::OpenPopup("Rename"); showRenamePopup = false; }
	if (showImportProgressPopup) { ImGui::OpenPopup("ImportProgressPopup"); showImportProgressPopup = false; }

    centerWindow();
	if (ImGui::BeginPopupModal("CreateFolder", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        static char inputBuffer[256] = { 0 };
        ImGui::InputText("Folder Name", inputBuffer, sizeof(inputBuffer));
        if (inputBuffer[0] == '\0') {
            strncpy(inputBuffer, "new folder", sizeof(inputBuffer) - 1);
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

    centerWindow();
    if (ImGui::BeginPopupModal("CreateScene", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        static char inputBuffer[256] = { 0 };
        ImGui::InputText("Scene Name", inputBuffer, sizeof(inputBuffer));
        if (inputBuffer[0] == '\0') {
            strncpy(inputBuffer, "myscene", sizeof(inputBuffer) - 1);
            inputBuffer[sizeof(inputBuffer) - 1] = '\0';
        }

        if (ImGui::Button("Create")) {
            createScene(currentPath / (std::string(inputBuffer) + std::string(".scene")));
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

    centerWindow();
    if (ImGui::BeginPopupModal("RenamePopup", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        static char inputBuffer[256] = { 0 };
        ImGui::InputText("New Name", inputBuffer, sizeof(inputBuffer));
        if (inputBuffer[0] == '\0') {
            strncpy(inputBuffer, "new name", sizeof(inputBuffer) - 1);
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

    centerWindow();
    if (ImGui::BeginPopupModal("ImportModelPopup", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        const std::string importLabel = std::string("Import ") + importModelPath.filename().string();
        ImGui::Text("%s", importLabel.c_str());

        struct AssimpFlagOption {
            const char* label;
            unsigned int flag;
            bool enabled;
            const char* description;
        };

        static AssimpFlagOption s_assimpFlags[] = {
            { "Triangulate",           aiProcess_Triangulate,          true,  "Convert polygons to triangles" },
            { "Calc Tangent Space",    aiProcess_CalcTangentSpace,     true,  "Needed for normal mapping" },
            { "Join Identical Vertices", aiProcess_JoinIdenticalVertices, true, "Dedupe verts for indexed rendering" },
            { "Generate Smooth Normals (smooth shade)", aiProcess_GenSmoothNormals,   false,  "interpolated normals (smooth shade in blender) if normals are missing" },
            { "Flip UVs",              aiProcess_FlipUVs,               false, "Flip V coordinate (OpenGL vs DirectX)" },
            { "Limit Bone Weights",    aiProcess_LimitBoneWeights,      false, "Cap bone influences per vertex" },
            { "Convert To Left Handed", aiProcess_ConvertToLeftHanded,  false, "Switch coordinate handedness" },
            { "Optimize Meshes",       aiProcess_OptimizeMeshes,        false, "Merge meshes for fewer draw calls" },
            { "Optimize Graph",        aiProcess_OptimizeGraph,         false, "Flatten node hierarchy" },
            { "Split Large Meshes",    aiProcess_SplitLargeMeshes,      false, "Split meshes exceeding vertex/index limits" },
            { "Remove Redundant Materials", aiProcess_RemoveRedundantMaterials, false, "Merge duplicate materials" },
        };

		for (auto& option : s_assimpFlags) {

			if (option.flag == aiProcess_Triangulate || option.flag == aiProcess_CalcTangentSpace) {
				ImGui::BeginDisabled();
                ImGui::Checkbox(option.label, &option.enabled);
				ImGui::EndDisabled();
            }
            else {
                ImGui::Checkbox(option.label, &option.enabled);
            }
			
            if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("%s", option.description);
			}
        };

        if (ImGui::Button("Import")) {
            unsigned int flags = 0;
            for (auto& option : s_assimpFlags) {
                if (option.enabled) {
                    flags |= option.flag;
                }
            }

            flags |= aiProcess_Triangulate | aiProcess_CalcTangentSpace;
            if (!(flags & aiProcess_GenSmoothNormals)) flags |= aiProcess_GenNormals;

            const fs::path sourceModelPath = importModelPath;
            const fs::path outputRoot = currentPath;
            const std::string modelName = sourceModelPath.stem().string();

            importFuture = std::async(std::launch::async, [this, flags, sourceModelPath, outputRoot, modelName]() {
                fs::path modelDir;
                bool createdModelDir = false;
                auto failImport = [&](const std::string& message) {
                    if (createdModelDir) {
                        std::error_code ec;
                        fs::remove_all(modelDir, ec);
                    }
                    LOG::EditorError(message);
                    progress = -1;
                };

                try {
                    const aiScene* scene = editor.importer.ReadFile(sourceModelPath.string(), flags);
                    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode || scene->mNumMeshes == 0) {
                        failImport(std::string("Failed to import model: ") + editor.importer.GetErrorString());
                        return;
                    }

                    modelDir = outputRoot / modelName;
                    if (fs::exists(modelDir)) {
                        failImport("Import destination already exists: " + modelDir.string());
                        return;
                    }
                    fs::create_directories(modelDir);
                    createdModelDir = true;
                    fs::create_directories(modelDir / "materials");
                    fs::create_directories(modelDir / "textures");
                    progress = 2;

                    const fs::path materialsDir = modelDir / "materials";
                    const fs::path texturesDir = modelDir / "textures";
                    std::unordered_map<unsigned int, UUID> materialMap;

                    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
                        const aiMaterial* material = scene->mMaterials[i];
                        // base color ignored
                        aiColor3D diffuse(1.0f, 1.0f, 1.0f);
                        aiColor3D specular(1.0f, 1.0f, 1.0f);
                        float shininess = 32.0f;

                        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
                        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
                        material->Get(AI_MATKEY_SHININESS, shininess);

                        const UUID materialUUID = generateUUID();
                        materialMap.emplace(i, materialUUID);
                        nlohmann::json materialJson = {
                            {"uuid", UUID::toString(materialUUID)},
                            {"diffuseColor", {diffuse.r, diffuse.g, diffuse.b}},
                            {"specularColor", {specular.r, specular.g, specular.b}},
                            {"shininess", shininess},
                            {"shaderType", ShaderType::LIT},
                            {"diffuseMap", ""},
                            {"specularMap", ""},
                            {"normalMap", ""}
                        };

                        auto importTexture = [&](aiTextureType textureType, const char* mapKey) {
                            if (material->GetTextureCount(textureType) == 0) return;
                            aiString textureRef;
                            if (material->GetTexture(textureType, 0, &textureRef) != AI_SUCCESS) return;

                            fs::path sourceTexture(textureRef.C_Str());
                            fs::path textureOutput;
                            if (const aiTexture* embedded = scene->GetEmbeddedTexture(textureRef.C_Str())) {
                                std::string extension = embedded->achFormatHint;
                                if (extension.empty()) return;
                                if (extension.front() != '.') extension.insert(extension.begin(), '.');
                                textureOutput = texturesDir / ("texture_" + std::to_string(i) + "_" + mapKey + extension);
                                if (embedded->mHeight == 0) {
                                    std::ofstream out(textureOutput, std::ios::binary);
                                    out.write(reinterpret_cast<const char*>(embedded->pcData), embedded->mWidth);
                                    if (!out) throw std::runtime_error("Failed to extract embedded texture: " + textureOutput.string());
                                } else {
                                    return; // Uncompressed embedded textures are left unassigned.
                                }
                            } else {
                                if (sourceTexture.is_relative()) sourceTexture = sourceModelPath.parent_path() / sourceTexture;
                                if (!fs::exists(sourceTexture) || !fs::is_regular_file(sourceTexture)) return;
                                std::string extension = sourceTexture.extension().string();
                                if (extension.empty()) extension = ".img";
                                textureOutput = texturesDir / ("texture_" + std::to_string(i) + "_" + mapKey + extension);
                                fs::copy_file(sourceTexture, textureOutput, fs::copy_options::overwrite_existing);
                            }

                            const UUID textureUUID = generateUUID();
                            nlohmann::json textureJson = {
                                {"uuid", UUID::toString(textureUUID)},
                                {"texturePath", fs::relative(textureOutput, fs::path(ASSETS_DIR)).generic_string()},
                                {"textureType", "Texture2D"},
                                {"wrapU", GL_REPEAT}, {"wrapV", GL_REPEAT},
                                {"minFilter", GL_LINEAR_MIPMAP_LINEAR}, {"magFilter", GL_LINEAR}
                            };
                            fs::path textureMetaPath = textureOutput;
                            textureMetaPath += ".tex";
                            std::ofstream textureMetaFile(textureMetaPath);
                            textureMetaFile << textureJson.dump(4);
                            if (!textureMetaFile) throw std::runtime_error("Failed to write texture metadata: " + textureMetaPath.string());
                            materialJson[mapKey] = UUID::toString(textureUUID);
                        };

                        importTexture(aiTextureType_BASE_COLOR, "diffuseMap");
                        importTexture(aiTextureType_SPECULAR, "specularMap");
                        importTexture(aiTextureType_NORMALS, "normalMap");
                        if (materialJson["diffuseMap"] == "") importTexture(aiTextureType_DIFFUSE, "diffuseMap");
                        if (materialJson["normalMap"] == "") importTexture(aiTextureType_HEIGHT, "normalMap");

                        // material path :skull:
                        aiString materialName;
                        material->Get(AI_MATKEY_NAME, materialName);
                        std::string safeMaterialName = materialName.length > 0 ? materialName.C_Str() : "material_" + std::to_string(i);
                        for (char& c : safeMaterialName) if (c == '/' || c == '\\' || c == ':') c = '_';
                        fs::path materialPath = materialsDir / (safeMaterialName + (materialName.length > 0 ? ("_" + std::to_string(i)) : "") + ".mat");
                        
                        std::ofstream materialFile(materialPath);

                        materialFile << materialJson.dump(4);
                        if (!materialFile) throw std::runtime_error("Failed to write material file: " + materialPath.string());
                        progress = 2 + (scene->mNumMaterials ? static_cast<int>((i + 1) * 3 / scene->mNumMaterials) : 3);
                    }

                    uint32_t vertexCount = 0;
                    uint32_t indexCount = 0;
                    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
                        const aiMesh* mesh = scene->mMeshes[i];
                        vertexCount += mesh->mNumVertices;
                        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) indexCount += mesh->mFaces[f].mNumIndices;
                    }

                    if (vertexCount == 0 || indexCount == 0 || vertexCount > std::numeric_limits<uint32_t>::max() || indexCount > std::numeric_limits<uint32_t>::max())
                        throw std::runtime_error("Imported model exceeds supported mesh size.");

                    const fs::path meshDataPath = modelDir / modelName;
                    std::ofstream meshData(meshDataPath, std::ios::binary);
                    if (!meshData) throw std::runtime_error("Failed to create mesh data: " + meshDataPath.string());
                    meshData.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
                    meshData.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

                    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
                        const aiMesh* mesh = scene->mMeshes[i];
                        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
                            Vertex vertex{};
                            vertex.position = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
                            if (mesh->HasNormals()) vertex.normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};
                            if (mesh->HasTextureCoords(0)) vertex.uv = {mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y};
                            if (mesh->HasTangentsAndBitangents()) {
                                const glm::vec3 normal(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
                                const glm::vec3 tangent(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
                                const glm::vec3 bitangent(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
                                const float handedness = glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;
                                vertex.tangent = {tangent.x, tangent.y, tangent.z, handedness};
                            }
                            meshData.write(reinterpret_cast<const char*>(&vertex), sizeof(vertex));
                        }
                    }
                    progress = 7;

                    std::vector<uint32_t> indexOffsets(scene->mNumMeshes);
                    uint32_t vertexOffset = 0;
                    uint32_t indexOffset = 0;
                    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
                        const aiMesh* mesh = scene->mMeshes[i];
                        indexOffsets[i] = indexOffset;
                        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
                            const aiFace& face = mesh->mFaces[f];
                            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                                const uint32_t index = vertexOffset + face.mIndices[j];
                                meshData.write(reinterpret_cast<const char*>(&index), sizeof(index));
                            }
                            indexOffset += face.mNumIndices;
                        }
                        vertexOffset += mesh->mNumVertices;
                    }
                    progress = 9;

                    const uint32_t submeshCount = scene->mNumMeshes;
                    meshData.write(reinterpret_cast<const char*>(&submeshCount), sizeof(submeshCount));
                    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
                        const aiMesh* mesh = scene->mMeshes[i];
                        Submesh submesh{};
                        aiString meshName = mesh->mName;
                        if (meshName.length == 0) meshName.Set("mesh_" + std::to_string(i));
                        std::strncpy(submesh.name, meshName.C_Str(), sizeof(submesh.name) - 1);
                        submesh.indicesOffset = indexOffsets[i];
                        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) submesh.indicesSize += mesh->mFaces[f].mNumIndices;
                        if (auto it = materialMap.find(mesh->mMaterialIndex); it != materialMap.end()) submesh.material = it->second;
                        meshData.write(reinterpret_cast<const char*>(&submesh), sizeof(submesh));
                    }
                    meshData.close();
                    if (!meshData) throw std::runtime_error("Failed while writing mesh data: " + meshDataPath.string());

                    const UUID meshUUID = generateUUID();
                    std::ofstream meshMeta(modelDir / (modelName + ".mesh"), std::ios::binary);
                    if (!meshMeta) throw std::runtime_error("Failed to create mesh metadata file.");

                    nlohmann::json meshMetaJson = {
                        {"uuid", UUID::toString(meshUUID)},
                        {"vertexCount", vertexCount},
                        {"indexCount", indexCount},
                        {"submeshCount", submeshCount},
                    };

                    meshMeta << meshMetaJson.dump(4);
                    meshMeta.close();

                    progress = -2;
                } catch (const std::exception& e) {
                    failImport(std::string("Model import failed: ") + e.what());
                }
            });

            ImGui::CloseCurrentPopup();
            showImportProgressPopup = true;
        }
        ImGui::EndPopup();
    }

    centerWindow();
    if (ImGui::BeginPopupModal("ImportProgressPopup", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {

        if (progress == -1) {

            ImGui::Text("Import failed. Check editor console for details.");

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();

                progress = 0;
            }
		}
		else if (progress == -2) {
			ImGui::Text("Boom Baam! Import complete.");
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
				progress = 0;
			}
		}
        else {
            int currentProgress = progress;

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
    std::cout << "creating scene: " << path;
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
    if (!fs::exists(path)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "File Not Found", "The file or folder you are trying to delete does not exist.", appState.window);
        return;
    }
    try {
        // Your filesystem operation
        fs::remove_all(path);
    }
    catch (const fs::filesystem_error& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to delete asset", (std::string("An error occurred while trying to delete the asset. Error: ") + std::string(e.what())).c_str(), appState.window);
    }
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
    TexDets texDets;
    texDets.minFilter = GL_NEAREST;
    fileTex.Initialize(SRC_DIR "editor/resources/File.png", texDets);
    folderTex.Initialize(SRC_DIR "editor/resources/Folder.png", texDets);
    folderEmptyTex.Initialize(SRC_DIR "editor/resources/FolderEmpty.png", texDets);
}

void AssetsBrowser::handleFileDrop(const char* filePath)
{
    importModelPath = filePath;

    if (importModelPath.extension() == ".fbx" || importModelPath.extension() == ".obj" || importModelPath.extension() == ".dae" ||
        importModelPath.extension() == ".gltf" || importModelPath.extension() == ".glb" || importModelPath.extension() == ".stl") {

        showImportModelPopup = true;
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
        
        static const fs::path assetsPath = fs::path(ROOT_DIR "Assets");

        if(currentPath != assetsPath) {
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
                if(ImGui::MenuItem("Folder")) showCreateFolderPopup = true;
                if(ImGui::MenuItem("Scene")) showCreateScenePopup = true;
                if(ImGui::MenuItem("Model")) showImportModelPopup = true;
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
        const char* itemPathW = itemPath.c_str();
        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPathW, (strlen(itemPathW) + 1) * sizeof(char));
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
        } else {
            // an asset opened.
            std::ifstream selectedItemFile(selectedItem, std::ios::binary);
            if(!selectedItemFile) {
                LOG::EditorError("Failed to open asset file: ", selectedItem.string());
                return;
            }
            
            nlohmann::json assetMetaJson;
            selectedItemFile >> assetMetaJson;
            
            editor.selectedAssetMeta = engine.assetsRegistry.getAsset<AssetMeta>(
                UUID::fromString(assetMetaJson.at("uuid").get<std::string>())
            );
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
