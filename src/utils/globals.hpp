#pragma once
#include "SDL3/SDL.h"
#include "Shader.hpp"
#include "../core/SceneManager.hpp"
#include "../core/Camera.hpp"
#include "../core/Renderer.hpp"
#include "../core/InputManager.hpp"
#include "../core/assets/AssetsRegistry.hpp"
#include "../core/assets/RuntimeAssetsRegistry.hpp"
#include "../editor/SceneView.hpp"
#include "../editor/gui/HierarchyPanel.hpp"
#include "../editor/gui/InspectorPanel.hpp"
#include "../editor/gui/ScenePanel.hpp"
#include "../editor/gui/AssetsBrowserPanel.hpp"
#include "../editor/gui/ConsolePanel.hpp"
#include "../editor/gui/AssetsRegistryPanel.hpp"
#include <assimp/Importer.hpp>

struct AppState {
    SDL_Window* window = nullptr;
    SDL_Cursor* cursor = nullptr;
	SDL_Cursor* loadingCursor = nullptr;
    SDL_GLContext glContext = nullptr;
};

struct Engine {
    SceneManager sceneManager;

    Renderer renderer;
    Shader litShader;
    Shader unlitShader;
	InputManager inputManager;
	AssetsRegistry assetsRegistry;
	RuntimeAssetsRegistry runtimeAssetsRegistry;
};

struct Editor {
    Camera editorCamera;
    GameObject* selectedGameObject = nullptr;
    AssetMeta* selectedAssetMeta = nullptr;

    Assimp::Importer importer;
    SceneView sceneView;
    ScenePanel scenePanel;
    AssetsBrowser assetsBrowserPanel;
	AssetsRegistryPanel assetsRegistryPanel;
    HierarchyPanel hierarchyPanel;
    InspectorPanel inspectorPanel;
    ConsolePanel consolePanel;
};

extern Engine engine;
extern Editor editor;
extern AppState appState;