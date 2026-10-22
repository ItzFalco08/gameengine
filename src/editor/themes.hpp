#pragma once

namespace Pal
{
    static constexpr ImVec4 BG = { 0.082f, 0.082f, 0.082f, 1.f }; // #151515
    static constexpr ImVec4 BG_WIDGET = { 0.110f, 0.110f, 0.110f, 1.f }; // #1c1c1c
    static constexpr ImVec4 BG_GROUP = { 0.125f, 0.125f, 0.125f, 1.f }; // #202020
    static constexpr ImVec4 BORDER = { 0.2f, 0.2f, 0.2f, 1.f }; // #383838
    static constexpr ImVec4 BORDER_DIM = { 0.150f, 0.150f, 0.150f, 1.f }; // #262626
    static constexpr ImVec4 TEXT = { 0.780f, 0.780f, 0.780f, 1.f }; // #c7c7c7
    static constexpr ImVec4 TEXT_DIM = { 0.450f, 0.450f, 0.450f, 1.f }; // #737373
    static constexpr ImVec4 ACCENT = { 0.220f, 0.480f, 0.780f, 1.f }; // #387ac7 blue
    static constexpr ImVec4 ACCENT_DIM = { 0.120f, 0.280f, 0.500f, 1.f }; // #1f4780 darker blue
    static constexpr ImVec4 ACCENT_URL = { 0.400f, 0.680f, 1.000f, 1.f }; // #66adff bright blue
    static constexpr ImVec4 TRANSPARENT = { 0.f, 0.f, 0.f, 0.f };

    static ImU32 U(ImVec4 v) { return ImGui::ColorConvertFloat4ToU32(v); }
}

void MortyDark()
{
    ImGuiStyle& s = ImGui::GetStyle();

    // ── Geometry ──────────────────────────────
    s.WindowPadding = { 8, 8 };
    s.FramePadding = { 4, 2 };
    s.CellPadding = { 4, 2 };
    s.ItemSpacing = { 6, 4 };
    s.ItemInnerSpacing = { 4, 4 };
    s.IndentSpacing = 12.f;
    s.ScrollbarSize = 8.f;
    s.GrabMinSize = 6.f;

    s.WindowBorderSize = 1.f;
    s.ChildBorderSize = 1.f;
    s.PopupBorderSize = 1.f;
    s.FrameBorderSize = 1.f;
    s.TabBorderSize = 0.f;

    s.WindowRounding = 0.f;
    s.ChildRounding = 0.f;
    s.FrameRounding = 0.f;
    s.PopupRounding = 0.f;
    s.ScrollbarRounding = 0.f;
    s.GrabRounding = 0.f;
    s.TabRounding = 0.f;

    s.WindowTitleAlign = { 0.f, 0.5f };
    s.WindowMenuButtonPosition = ImGuiDir_None;

    // ── Colours ───────────────────────────────
    ImVec4* c = s.Colors;
    c[ImGuiCol_Text] = Pal::TEXT;
    c[ImGuiCol_TextDisabled] = Pal::TEXT_DIM;
    c[ImGuiCol_WindowBg] = Pal::BG;
    c[ImGuiCol_ChildBg] = Pal::BG_GROUP;
    c[ImGuiCol_PopupBg] = Pal::BG_WIDGET;
    c[ImGuiCol_Border] = Pal::BORDER;
    c[ImGuiCol_BorderShadow] = Pal::TRANSPARENT;
    c[ImGuiCol_FrameBg] = Pal::BG_WIDGET;
    c[ImGuiCol_FrameBgHovered] = { 0.15f,0.15f,0.15f,1.f };
    c[ImGuiCol_FrameBgActive] = { 0.18f,0.18f,0.18f,1.f };
    c[ImGuiCol_TitleBg] = Pal::BG;
    c[ImGuiCol_TitleBgActive] = Pal::BG;
    c[ImGuiCol_TitleBgCollapsed] = Pal::BG;
    c[ImGuiCol_MenuBarBg] = Pal::BG;
    c[ImGuiCol_ScrollbarBg] = Pal::BG;
    c[ImGuiCol_ScrollbarGrab] = Pal::BORDER;
    c[ImGuiCol_ScrollbarGrabHovered] = Pal::ACCENT_DIM;
    c[ImGuiCol_ScrollbarGrabActive] = Pal::ACCENT;
    c[ImGuiCol_CheckMark] = Pal::ACCENT;
    c[ImGuiCol_SliderGrab] = Pal::ACCENT;
    c[ImGuiCol_SliderGrabActive] = Pal::ACCENT_URL;
    c[ImGuiCol_Button] = Pal::BG_WIDGET;
    c[ImGuiCol_ButtonHovered] = { 0.16f,0.16f,0.16f,1.f };
    c[ImGuiCol_ButtonActive] = Pal::ACCENT_DIM;
    c[ImGuiCol_Header] = { 0.18f, 0.18f, 0.18f, 1.f }; // dark gray resting
    c[ImGuiCol_HeaderHovered] = { 0.22f, 0.22f, 0.22f, 1.f }; // slightly lighter on hover
    c[ImGuiCol_HeaderActive] = { 0.26f, 0.26f, 0.26f, 1.f }; // a bit lighter on click
    c[ImGuiCol_Separator] = Pal::BORDER_DIM;
    c[ImGuiCol_SeparatorHovered] = Pal::ACCENT_DIM;
    c[ImGuiCol_SeparatorActive] = Pal::ACCENT;
    c[ImGuiCol_ResizeGrip] = Pal::TRANSPARENT;
    c[ImGuiCol_ResizeGripHovered] = Pal::ACCENT_DIM;
    c[ImGuiCol_ResizeGripActive] = Pal::ACCENT;
    c[ImGuiCol_Tab] = Pal::BG;
    c[ImGuiCol_TabHovered] = { 0.14f,0.14f,0.14f,1.f };
    c[ImGuiCol_TabActive] = Pal::BG_GROUP;
    c[ImGuiCol_TabUnfocused] = Pal::BG;
    c[ImGuiCol_TabUnfocusedActive] = Pal::BG;
    c[ImGuiCol_PlotLines] = Pal::ACCENT;
    c[ImGuiCol_PlotLinesHovered] = Pal::ACCENT_URL;
    c[ImGuiCol_PlotHistogram] = Pal::ACCENT;
    c[ImGuiCol_PlotHistogramHovered] = Pal::ACCENT_URL;
    c[ImGuiCol_TableHeaderBg] = Pal::BG_WIDGET;
    c[ImGuiCol_TableBorderStrong] = Pal::BORDER;
    c[ImGuiCol_TableBorderLight] = Pal::BORDER_DIM;
    c[ImGuiCol_TableRowBg] = Pal::TRANSPARENT;
    c[ImGuiCol_TableRowBgAlt] = { 1,1,1,0.03f };
    c[ImGuiCol_TextSelectedBg] = { 0.22f, 0.48f, 0.78f, 0.35f }; // #387ac7 blue selection tint
    c[ImGuiCol_DragDropTarget] = Pal::ACCENT;
    c[ImGuiCol_NavHighlight] = Pal::ACCENT;
    c[ImGuiCol_NavWindowingHighlight] = { 1,1,1,0.7f };
    c[ImGuiCol_NavWindowingDimBg] = { 0,0,0,0.2f };
    c[ImGuiCol_ModalWindowDimBg] = { 0,0,0,0.4f };
}


void defaultDark()
{
	// Windark style by DestroyerDarkNess from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO io = ImGui::GetIO();
    io.Fonts->AddFontDefaultVector();
	
	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 8.4f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 3.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 3.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 3.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 5.6f;
	style.ScrollbarRounding = 18.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 3.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1254902f, 0.1254902f, 0.1254902f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1254902f, 0.1254902f, 0.1254902f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1254902f, 0.1254902f, 0.1254902f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1254902f, 0.1254902f, 0.1254902f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1254902f, 0.1254902f, 0.1254902f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3019608f, 0.3019608f, 0.3019608f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.34901962f, 0.34901962f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.47058824f, 0.84313726f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.47058824f, 0.84313726f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.32941177f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3019608f, 0.3019608f, 0.3019608f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3019608f, 0.3019608f, 0.3019608f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3019608f, 0.3019608f, 0.3019608f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.2509804f, 0.2509804f, 0.2509804f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.16862746f, 0.16862746f, 0.16862746f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.21568628f, 0.21568628f, 0.21568628f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.47058824f, 0.84313726f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.0f, 0.32941177f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.47058824f, 0.84313726f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.0f, 0.32941177f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.47058824f, 0.84313726f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}



void SetEditorStyle()
{
    defaultDark();
}