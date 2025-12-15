#include "Utils.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Logger.hpp"
#include "globals.hpp"

void SetEditorStyle();

void Utils::initImGui(GLFWwindow* window) {
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


void Utils::loadObj(std::vector<Vertex>& vertices, const char* objPath) {
    // positions[0] and uvs[0] is the x, y, z, u, v of first vertex
    std::ifstream objFile(objPath);

    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;

    std::string line;

    while(std::getline(objFile, line)) {
        std::string prefix;
        std::stringstream ss(line);
        ss >> prefix;
        
        if (prefix == "v") {
            float x, y, z;
            ss >> x >> y >> z;

            tempPositions.push_back(glm::vec3(x,y,z));
            // LOG::Info("added vertex pos: ", x, ", ", y,", ", z);
            continue;
        }

        if (prefix == "vt") {
            float u, v;
            ss >> u >> v;

            tempUvs.push_back(glm::vec2(u,v));
            // LOG::Info("added uv: ", u, ", ", v);
            continue;
        }
        
        if (prefix == "vn") {
            float x, y, z;
            ss >> x >> y >> z;

            tempNormals.push_back(glm::vec3(x,y,z));
            // LOG::Info("added vertex normal : ", x, ", ", y,", ", z);
            continue;
        }

        if (prefix == "f") {
            std::vector<std::string> faceVerts;
            std::string vert;
            while(ss >> vert) {
                faceVerts.push_back(vert);
            }


            if (faceVerts.size() > 3) {
                std::array<int, 6> order = {0, 1, 2, 0, 2 ,3}; // convert to 2 triangles

                for (int i = 0; i < order.size(); i++) { // 0 -> 5
                    int v, vt, vn;
                    std::stringstream vertexSS(faceVerts[order[i]]);
                    std::string token;

                    std::getline(vertexSS, token, '/'); v = stoi(token);
                    std::getline(vertexSS, token, '/'); vt = stoi(token);
                    std::getline(vertexSS, token, '/'); vn = stoi(token);

                    Vertex vertex;
                    vertex.position = tempPositions[v - 1];
                    vertex.uv = tempUvs[vt - 1];
                    vertex.normal = tempNormals[vn - 1];
                    vertices.push_back(vertex);
                    
                    // LOG::Info(
                    //     "Vertex Added: ", "pos: ", vertex.position.x, ", ", vertex.position.y, ", ", vertex.position.z, "\n",
                    //     "uv: ", vertex.uv.x, ", ", vertex.uv.y, "\n",
                    //     "normal: ", vertex.normal.x, ", ", vertex.normal.y, ", ", vertex.normal.z
                    // );
                }
            } else {
                for (int i = 0; i <= 2; i++) { // 0, 1, 2
                    int v, vt, vn;
                    std::stringstream vertexSS(faceVerts[i]);
                    std::string token;

                    std::getline(vertexSS, token, '/'); v = stoi(token);
                    std::getline(vertexSS, token, '/'); vt = stoi(token);
                    std::getline(vertexSS, token, '/'); vn = stoi(token);

                    Vertex vertex;
                    vertex.position = tempPositions[v - 1];
                    vertex.uv = tempUvs[vt - 1];
                    vertex.normal = tempNormals[vn - 1];
                    vertices.push_back(vertex);
                    
                    LOG::Info(
                        "Vertex Added: ", "pos: ", vertex.position.x, ", ", vertex.position.y, ", ", vertex.position.z, "\n",
                        "uv: ", vertex.uv.x, ", ", vertex.uv.y, "\n",
                        "normal: ", vertex.normal.x, ", ", vertex.normal.y, ", ", vertex.normal.z
                    );
                }
            }
        }
    }
}


void SetEditorStyle()
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
    c[ImGuiCol_TabActive]            = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);

    // Style
    style.WindowRounding = 2.0f;
    style.FrameRounding  = 3.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding   = 3.0f;
}

