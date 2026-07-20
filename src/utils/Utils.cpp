#include "Utils.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Logger.hpp"
#include "globals.hpp"

void SetEditorStyle();

void Utils::GUI::initImGui(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags  |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Apply custom style AFTER setting config flags
    SetEditorStyle();
    
    // Initialize backends (example GLSL 330)
    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Utils::GLFWErrorCallback(int errorCode, const char* message) {
    LOG::Error("GLFW Error. Code: ", errorCode, "\n", message);
}

void Utils::genSceneFramebuffers() {
    glGenTextures(1, &sceneView.textureObj);
    glGenFramebuffers(1, &sceneView.framebuffObj);
    glGenRenderbuffers(1, &sceneView.depthbuffObj);

    // TEXTURE OBJECT
    glBindTexture(GL_TEXTURE_2D, sceneView.textureObj);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneView.SCENEVIEW_WIDTH, sceneView.SCENEVIEW_HEIGHT, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    // DEPTH BUFFER
    glBindRenderbuffer(GL_RENDERBUFFER, sceneView.depthbuffObj);
    glRenderbufferStorage(
        GL_RENDERBUFFER, 
        GL_DEPTH24_STENCIL8,   // depth + stencil format
        sceneView.SCENEVIEW_WIDTH, sceneView.SCENEVIEW_HEIGHT
    );

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // FRAME BUFF
    glBindFramebuffer(GL_FRAMEBUFFER, sceneView.framebuffObj);
    
    // Attach color texture
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        sceneView.textureObj,
        0
    );

    // Attach depth-stencil RBO
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        sceneView.depthbuffObj
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO failed!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Utils::updateFBODimensions() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, sceneView.framebuffObj);

    // Resize COLOR texture
    glBindTexture(GL_TEXTURE_2D, sceneView.textureObj);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA8,
        sceneView.SCENEVIEW_WIDTH,
        sceneView.SCENEVIEW_HEIGHT,
        0,
        GL_RGBA, GL_UNSIGNED_BYTE,
        nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);

    // Resize DEPTH-STENCIL
    glBindRenderbuffer(GL_RENDERBUFFER, sceneView.depthbuffObj);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        sceneView.SCENEVIEW_WIDTH,
        sceneView.SCENEVIEW_HEIGHT
    );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void defaultDark()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* c = style.Colors;

    c[ImGuiCol_Text]                 = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    c[ImGuiCol_TextDisabled]         = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Darker main panels
    c[ImGuiCol_WindowBg]             = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    c[ImGuiCol_ChildBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_PopupBg]              = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

    // Subtle borders
    c[ImGuiCol_Border]               = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);

    // Frames (Inputs, dropdowns)
    c[ImGuiCol_FrameBg]              = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    c[ImGuiCol_FrameBgHovered]       = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    c[ImGuiCol_FrameBgActive]        = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

    // Title bars
    c[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_TitleBgActive]        = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);

    c[ImGuiCol_MenuBarBg]            = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);

    // Scrollbars
    c[ImGuiCol_ScrollbarBg]          = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);

    // Buttons
    c[ImGuiCol_Button]               = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    c[ImGuiCol_ButtonHovered]        = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    c[ImGuiCol_ButtonActive]         = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);

    // Unity blue accents
    c[ImGuiCol_CheckMark]            = ImVec4(0.17f, 0.55f, 0.90f, 1.00f);
    c[ImGuiCol_SliderGrab]           = ImVec4(0.18f, 0.50f, 0.85f, 1.00f);
    c[ImGuiCol_SliderGrabActive]     = ImVec4(0.20f, 0.60f, 0.95f, 1.00f);

    // Headers (collapsing sections)
    c[ImGuiCol_Header]               = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    c[ImGuiCol_HeaderHovered]        = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    c[ImGuiCol_HeaderActive]         = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);

    // Tabs
    c[ImGuiCol_Tab]                  = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_TabHovered]           = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    c[ImGuiCol_TabActive]            = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

    // Style
    style.WindowRounding = 2.0f;
    style.FrameRounding  = 3.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding   = 3.0f;
    style.TabRounding       = 0.0f;
}

namespace Pal
{
    static constexpr ImVec4 BG          = { 0.082f, 0.082f, 0.082f, 1.f }; // #151515
    static constexpr ImVec4 BG_WIDGET   = { 0.110f, 0.110f, 0.110f, 1.f }; // #1c1c1c
    static constexpr ImVec4 BG_GROUP    = { 0.125f, 0.125f, 0.125f, 1.f }; // #202020
    static constexpr ImVec4 BORDER      = { 0.2f, 0.2f, 0.2f, 1.f }; // #383838
    static constexpr ImVec4 BORDER_DIM  = { 0.150f, 0.150f, 0.150f, 1.f }; // #262626
    static constexpr ImVec4 TEXT        = { 0.780f, 0.780f, 0.780f, 1.f }; // #c7c7c7
    static constexpr ImVec4 TEXT_DIM    = { 0.450f, 0.450f, 0.450f, 1.f }; // #737373
    static constexpr ImVec4 ACCENT      = { 0.220f, 0.480f, 0.780f, 1.f }; // #387ac7 blue
    static constexpr ImVec4 ACCENT_DIM  = { 0.120f, 0.280f, 0.500f, 1.f }; // #1f4780 darker blue
    static constexpr ImVec4 ACCENT_URL  = { 0.400f, 0.680f, 1.000f, 1.f }; // #66adff bright blue
    static constexpr ImVec4 TRANSPARENT = { 0.f, 0.f, 0.f, 0.f };

    static ImU32 U(ImVec4 v) { return ImGui::ColorConvertFloat4ToU32(v); }
}
 
void MortyDark()
{
    ImGuiStyle& s = ImGui::GetStyle();
 
    // ── Geometry ──────────────────────────────
    s.WindowPadding        = { 8, 8 };
    s.FramePadding         = { 4, 2 };
    s.CellPadding          = { 4, 2 };
    s.ItemSpacing          = { 6, 4 };
    s.ItemInnerSpacing     = { 4, 4 };
    s.IndentSpacing        = 12.f;
    s.ScrollbarSize        = 8.f;
    s.GrabMinSize          = 6.f;
 
    s.WindowBorderSize     = 1.f;
    s.ChildBorderSize      = 1.f;
    s.PopupBorderSize      = 1.f;
    s.FrameBorderSize      = 1.f;
    s.TabBorderSize        = 0.f;
 
    s.WindowRounding       = 0.f;
    s.ChildRounding        = 0.f;
    s.FrameRounding        = 0.f;
    s.PopupRounding        = 0.f;
    s.ScrollbarRounding    = 0.f;
    s.GrabRounding         = 0.f;
    s.TabRounding          = 0.f;
 
    s.WindowTitleAlign     = { 0.f, 0.5f };
    s.WindowMenuButtonPosition = ImGuiDir_None;
 
    // ── Colours ───────────────────────────────
    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                  = Pal::TEXT;
    c[ImGuiCol_TextDisabled]          = Pal::TEXT_DIM;
    c[ImGuiCol_WindowBg]              = Pal::BG;
    c[ImGuiCol_ChildBg]               = Pal::BG_GROUP;
    c[ImGuiCol_PopupBg]               = Pal::BG_WIDGET;
    c[ImGuiCol_Border]                = Pal::BORDER;
    c[ImGuiCol_BorderShadow]          = Pal::TRANSPARENT;
    c[ImGuiCol_FrameBg]               = Pal::BG_WIDGET;
    c[ImGuiCol_FrameBgHovered]        = { 0.15f,0.15f,0.15f,1.f };
    c[ImGuiCol_FrameBgActive]         = { 0.18f,0.18f,0.18f,1.f };
    c[ImGuiCol_TitleBg]               = Pal::BG;
    c[ImGuiCol_TitleBgActive]         = Pal::BG;
    c[ImGuiCol_TitleBgCollapsed]      = Pal::BG;
    c[ImGuiCol_MenuBarBg]             = Pal::BG;
    c[ImGuiCol_ScrollbarBg]           = Pal::BG;
    c[ImGuiCol_ScrollbarGrab]         = Pal::BORDER;
    c[ImGuiCol_ScrollbarGrabHovered]  = Pal::ACCENT_DIM;
    c[ImGuiCol_ScrollbarGrabActive]   = Pal::ACCENT;
    c[ImGuiCol_CheckMark]             = Pal::ACCENT;
    c[ImGuiCol_SliderGrab]            = Pal::ACCENT;
    c[ImGuiCol_SliderGrabActive]      = Pal::ACCENT_URL;
    c[ImGuiCol_Button]                = Pal::BG_WIDGET;
    c[ImGuiCol_ButtonHovered]         = { 0.16f,0.16f,0.16f,1.f };
    c[ImGuiCol_ButtonActive]          = Pal::ACCENT_DIM;
    c[ImGuiCol_Header]        = { 0.18f, 0.18f, 0.18f, 1.f }; // dark gray resting
    c[ImGuiCol_HeaderHovered] = { 0.22f, 0.22f, 0.22f, 1.f }; // slightly lighter on hover
    c[ImGuiCol_HeaderActive]  = { 0.26f, 0.26f, 0.26f, 1.f }; // a bit lighter on click
    c[ImGuiCol_Separator]             = Pal::BORDER_DIM;
    c[ImGuiCol_SeparatorHovered]      = Pal::ACCENT_DIM;
    c[ImGuiCol_SeparatorActive]       = Pal::ACCENT;
    c[ImGuiCol_ResizeGrip]            = Pal::TRANSPARENT;
    c[ImGuiCol_ResizeGripHovered]     = Pal::ACCENT_DIM;
    c[ImGuiCol_ResizeGripActive]      = Pal::ACCENT;
    c[ImGuiCol_Tab]                   = Pal::BG;
    c[ImGuiCol_TabHovered]            = { 0.14f,0.14f,0.14f,1.f };
    c[ImGuiCol_TabActive]             = Pal::BG_GROUP;
    c[ImGuiCol_TabUnfocused]          = Pal::BG;
    c[ImGuiCol_TabUnfocusedActive]    = Pal::BG;
    c[ImGuiCol_PlotLines]             = Pal::ACCENT;
    c[ImGuiCol_PlotLinesHovered]      = Pal::ACCENT_URL;
    c[ImGuiCol_PlotHistogram]         = Pal::ACCENT;
    c[ImGuiCol_PlotHistogramHovered]  = Pal::ACCENT_URL;
    c[ImGuiCol_TableHeaderBg]         = Pal::BG_WIDGET;
    c[ImGuiCol_TableBorderStrong]     = Pal::BORDER;
    c[ImGuiCol_TableBorderLight]      = Pal::BORDER_DIM;
    c[ImGuiCol_TableRowBg]            = Pal::TRANSPARENT;
    c[ImGuiCol_TableRowBgAlt]         = { 1,1,1,0.03f };
    c[ImGuiCol_TextSelectedBg] = { 0.22f, 0.48f, 0.78f, 0.35f }; // #387ac7 blue selection tint
    c[ImGuiCol_DragDropTarget]        = Pal::ACCENT;
    c[ImGuiCol_NavHighlight]          = Pal::ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = { 1,1,1,0.7f };
    c[ImGuiCol_NavWindowingDimBg]     = { 0,0,0,0.2f };
    c[ImGuiCol_ModalWindowDimBg]      = { 0,0,0,0.4f };
}

void SetEditorStyle()
{
    MortyDark();
}

// G U I 

void Utils::GUI::ShowTextInputDialoge(const char* title, const char* defaultValue, bool& isActive, std::function<void(std::string)> onOk /* takes both lambda and func ptr */) {
    ImGui::SetNextWindowFocus();
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Always);
    ImGui::SetNextWindowPos(
        ImGui::GetMainViewport()->GetCenter(),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove;
    ImGui::Begin(title, nullptr, flags);

    // Persist what the user types; only seed the buffer the first time (or after manual clear)
    static char inputBuffer[256] = {0};
    if (inputBuffer[0] == '\0') {
        strncpy_s(inputBuffer, defaultValue, sizeof(inputBuffer) - 1);
        inputBuffer[sizeof(inputBuffer) - 1] = '\0';
    }
    
    ImGui::InputText("Enter Text", inputBuffer, sizeof(inputBuffer));

    if (ImGui::Button("OK")) {
        LOG::Info("OK pressed");
        onOk(inputBuffer);
        isActive = false;
        inputBuffer[0] = '\0'; // reset buffer for next open
    }

    ImGui::SameLine();
    
    if (ImGui::Button("Cancel")) {
        LOG::Info("Cancel pressed");
        isActive = false;
        inputBuffer[0] = '\0'; // reset buffer on cancel
    }

    ImGui::End();
}