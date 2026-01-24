#define _DEBUG
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "utils/Logger.hpp"
#include "utils/WinMsg.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "utils/Utils.hpp"
#include "utils/globals.hpp"
#include "gui/ScenePanel.hpp"
#include "gui/AssetsBrowserPanel.hpp"
#include "windows.h"

#include "core/GameObject.hpp"

void drawImgui();
void Update();
void Start();

// Global handle to the main window for cross-function access
static GLFWwindow* gMainWindow = nullptr;

int main() {
    glfwSetErrorCallback(Utils::GLFWErrorCallback);
    if(!glfwInit()) return 1;
        
    // context hints (you already have these)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // important for multi-viewport behavior
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    
    GLFWwindow* window = glfwCreateWindow(1200, 700, "Game Engine", nullptr, nullptr);
    if (!window) {glfwTerminate(); return 1; };
    gMainWindow = window;

    glfwMakeContextCurrent(window);
    if(!gladLoadGL(glfwGetProcAddress)) return 1;

    Utils::initImGui(window);
    Utils::genSceneFramebuffers();
    litShader = Shader("../src/shaders/lit/shader.frag", "../src/shaders/lit/shader.vert");
    unlitShader = Shader("../src/shaders/unlit/shader.frag", "../src/shaders/unlit/shader.vert");

    // Initialize panel icons after GL is ready
    panels::assetsBrowserPanel.InitIcons();

    Start();

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // NOTE: if fbo bounded, opengl draws into it. else backbuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, sceneView.framebuffObj);
        glViewport(0, 0, sceneView.SCENEVIEW_WIDTH, sceneView.SCENEVIEW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        Update();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        drawImgui();
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyPlatformWindows();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void Start() {
  
}

void Update() {

}

void drawImgui()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Fullscreen DockSpace window -------------------------
    ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("DockSpaceHost", nullptr, host_window_flags);
    ImGui::PopStyleVar(2);

    // DockSpace itself
    ImGuiID dockspace_id = ImGui::GetID("EngineDockspace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    // Menu bar (optional)
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Changes (Ctrl + S)")) {
                WinMsg::Info("Success", "Changes Saved!");
            } else if(ImGui::MenuItem("Exit Editor")) {
                if (gMainWindow) glfwSetWindowShouldClose(gMainWindow, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End(); // end DockSpaceHost

    panels::scenePanel.Render();
    panels::assetsBrowserPanel.Render();
    panels::heriarchyPanel.Render();
    panels::inspectorPanel.Render();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // When viewports are enabled, render additional platform windows
    if (ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_context);
    }
}
