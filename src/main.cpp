#include <iostream>
#include <glad/gl.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"
#include "utils/globals.hpp"
#include "core/Renderer.hpp"
#include "editor/gui/ScenePanel.hpp"
#include "editor/gui/AssetsBrowserPanel.hpp"
#include "editor/themes.hpp"
#include "SDL3/SDL.h"

void drawScreen();
void Update();
void Start();
void calcDeltaTime();
void genSceneFramebuffers();

double lastLogTime = 0.0;
double deltaTime = 0.0;

int main() {
#pragma region Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, SDL_GL_TRUE);
#endif

    appState.window = SDL_CreateWindow(
        "replife",
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!appState.window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(appState.window);
        SDL_Quit();
        return 1;
    }

    appState.glContext = SDL_GL_CreateContext(appState.window);
    if (!appState.glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(appState.window);
        SDL_Quit();
        return 1;
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) return 1;

	appState.cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    appState.loadingCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
#pragma endregion

#pragma region Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    SetEditorStyle();

    // SDL3 backend
    ImGui_ImplSDL3_InitForOpenGL(
        appState.window,
        appState.glContext
    );

    // OpenGL 3 backend
    ImGui_ImplOpenGL3_Init("#version 330");
    // -------------------
#pragma endregion

#pragma region Init
    genSceneFramebuffers();
    engine.litShader = Shader(ROOT_DIR "src/shaders/lit/shader.frag", ROOT_DIR "src/shaders/lit/shader.vert");
    engine.unlitShader = Shader(ROOT_DIR "src/shaders/unlit/shader.frag", ROOT_DIR "src/shaders/unlit/shader.vert");

    // Initialize panel icons after GL is ready
    editor.assetsBrowserPanel.InitIcons();
    editor.scenePanel.initTextures();

    Start();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#pragma endregion
    
#pragma region Main Loop
    SDL_Event event;
    bool running = true;

    while (running) {
        InputManager::clearFrameStates();
        while (SDL_PollEvent(&event)) {
            InputManager::processEvent(event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_DROP_FILE) {
                const char* path = event.drop.data;
                editor.assetsBrowserPanel.handleFileDrop(path);
            }
        }
        // NOTE: if fbo bounded, opengl draws into it. else backbuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, editor.sceneView.framebuffObj);
        glViewport(0, 0, editor.sceneView.SCENEVIEW_WIDTH, editor.sceneView.SCENEVIEW_HEIGHT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Update();
        EditorUpdate();
        SDL_GL_SwapWindow(appState.window);
    }
   
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(appState.window);
    SDL_DestroyCursor(appState.cursor);
    SDL_Quit();
    return 0;
#pragma endregion

}

void calcDeltaTime() {
    double curTime = static_cast<double>(SDL_GetTicks()) / 1000.0;
    deltaTime = curTime - lastLogTime;
    lastLogTime = curTime;
}

// Start
void Start() {

}

// renderer update
void Update() {
    calcDeltaTime();
    engine.renderer.OpenGLRenderer(sceneManager.activeScene.get());
}

// engine update (gui/states)
void EditorUpdate() {
    if (engine.assetsRegistry.m_syncFuture.valid() &&
        engine.assetsRegistry.m_syncFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {

        engine.assetsRegistry.m_syncFuture.get();
        SDL_SetCursor(appState.cursor);
    }

    drawScreen();
}

// draw editor
void drawScreen()
{
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    // Fullscreen DockSpace window -------------------------
    ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    const ImGuiViewport* viewport = ImGui::GetMainViewport(); 
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    // P A N E L - DockSpaceHost 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("DockSpaceHost", nullptr, host_window_flags);
    ImGui::PopStyleVar(3);

    // DockSpace itself
    ImGuiID dockspace_id = ImGui::GetID("EngineDockspace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    // Menu bar (optional)
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Changes (Ctrl + S)")) {
                sceneManager.activeScene->MakeDirty();
            } 
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End(); // end DockSpaceHost

    editor.scenePanel.Render();
    editor.assetsBrowserPanel.Render();
    editor.hierarchyPanel.Render();
    editor.inspectorPanel.Render();
    editor.consolePanel.Render();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // When viewports are enabled, render additional platform windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        SDL_GLContext backup_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(appState.window, backup_context);
    }
}


void genSceneFramebuffers() {
    glGenTextures(1, &editor.sceneView.textureObj);
    glGenFramebuffers(1, &editor.sceneView.framebuffObj);
    glGenRenderbuffers(1, &editor.sceneView.depthbuffObj);

    // TEXTURE OBJECT
    glBindTexture(GL_TEXTURE_2D, editor.sceneView.textureObj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, editor.sceneView.SCENEVIEW_WIDTH, editor.sceneView.SCENEVIEW_HEIGHT, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    // DEPTH BUFFER
    glBindRenderbuffer(GL_RENDERBUFFER, editor.sceneView.depthbuffObj);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,   // depth + stencil format
        editor.sceneView.SCENEVIEW_WIDTH, editor.sceneView.SCENEVIEW_HEIGHT
    );

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // FRAME BUFF
    glBindFramebuffer(GL_FRAMEBUFFER, editor.sceneView.framebuffObj);

    // Attach color texture
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        editor.sceneView.textureObj,
        0
    );

    // Attach depth-stencil RBO
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        editor.sceneView.depthbuffObj
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO failed!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

