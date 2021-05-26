#include "pch.h"
#include "Tools\PC\GraphicalUserInterface.h"
#include "Core/EngineCore.h"
#include "Platform/PC/PCDevice.h"

#include "Core/FileIO.h"

#include "Tools/PC/Window/RenderSettings.h"
#include "Tools/PC/Window/EntityList.h"
#include "Tools/PC/Window/ComponentInspector.h"

#include <DearImGui\imgui.h>
#include <DearImGui\imgui_impl_glfw.h>
#include <DearImGui\imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <cmath> // Used for flooring a float

namespace Frac {
	GraphicalUserInterface::GraphicalUserInterface() : m_menuItemViewFlag(0)
	{
        EngineCore* engineCore = &GetEngineConnector().get()->GetEngine();
        engineCore->GetFrameListenerManager().AddFrameListener(this);

		PCDevice* pcDevice = dynamic_cast<PCDevice*>(&engineCore->GetDevice());
        int videoResX = pcDevice->GetWindowWidth();
        int videoResY = pcDevice->GetWindowHeight();

        // Default res = 1920 width x 1080 height
        m_guiScale = videoResX / 1920.0f;

        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.Fonts->AddFontFromFileTTF(engineCore->GetResourceManager().GetFileIO().GetPathFromWildcard("[Fonts]NotoMono-Regular.ttf").c_str(), roundf(15.0f * m_guiScale));
        //io.Fonts->AddFontFromMemoryTTF(GetEngineConnector().get()->GetEngine().GetResourceManager().GetFileIO().Read("[Fonts]NotoSans-Regular.ttf"));

        // Setup Dear ImGui style
        SetupImGuiStyle();
        m_style->ScaleAllSizes(m_guiScale);

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(dynamic_cast<PCDevice*>(&engineCore->GetDevice())->GetWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        m_renderSettings = std::make_unique<RenderSettings>();
        m_entityList = std::make_unique<EntityList>(engineCore->GetWorld());
        m_componentInspector = std::make_unique<ComponentInspector>(engineCore->GetWorld(), *dynamic_cast<EntityList*>(m_entityList.get()));

        m_menuItemViewFlag += MenuItemViewFlags::ENTITY_LIST;
        m_entityList->SetWindowState(true);
        m_menuItemViewFlag += MenuItemViewFlags::COMPONENT_INSPECTOR;
        m_componentInspector->SetWindowState(true);
	}

	GraphicalUserInterface::~GraphicalUserInterface()
	{
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
	}

    void GraphicalUserInterface::FrameStarted()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UpdateDocking();
    }

    void GraphicalUserInterface::Update()
    {
        ImGui::BeginMainMenuBar();
        // Menu
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", nullptr, nullptr)) {
                GetEngineConnector().get()->GetEngine().Shutdown();

            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Render Settings", nullptr, m_menuItemViewFlag & MenuItemViewFlags::RENDER_SETTINGS)) {
                if (m_menuItemViewFlag & MenuItemViewFlags::RENDER_SETTINGS) {
                    m_menuItemViewFlag -= MenuItemViewFlags::RENDER_SETTINGS;
                    m_renderSettings->SetWindowState(false);
                }
                else {
                    m_menuItemViewFlag += MenuItemViewFlags::RENDER_SETTINGS;
                    m_renderSettings->SetWindowState(true);
                }
            }
            if (ImGui::MenuItem("Entity List", nullptr, m_menuItemViewFlag & MenuItemViewFlags::ENTITY_LIST)) {
                if (m_menuItemViewFlag & MenuItemViewFlags::ENTITY_LIST) {
                    m_menuItemViewFlag -= MenuItemViewFlags::ENTITY_LIST;
                    m_entityList->SetWindowState(false);
                }
                else {
                    m_menuItemViewFlag += MenuItemViewFlags::ENTITY_LIST;
                    m_entityList->SetWindowState(true);
                }
            }
            if (ImGui::MenuItem("Component Inspector", nullptr, m_menuItemViewFlag & MenuItemViewFlags::COMPONENT_INSPECTOR)) {
                if (m_menuItemViewFlag & MenuItemViewFlags::COMPONENT_INSPECTOR) {
                    m_menuItemViewFlag -= MenuItemViewFlags::COMPONENT_INSPECTOR;
                    m_componentInspector->SetWindowState(false);
                }
                else {
                    m_menuItemViewFlag += MenuItemViewFlags::COMPONENT_INSPECTOR;
                    m_componentInspector->SetWindowState(true);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::Text("|  Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::EndMainMenuBar();

        m_renderSettings.get()->Update();
        m_entityList.get()->Update();
        m_componentInspector.get()->Update();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    float GraphicalUserInterface::GetGUIScale() const
    {
        return m_guiScale;
    }

    void GraphicalUserInterface::UpdateDocking()
    {
        bool p_open{ true };
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &p_open, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        ImGui::End();
    }

    // Style from https://github.com/ocornut/imgui/issues/707
    void GraphicalUserInterface::SetupImGuiStyle()
    {
        m_style = &ImGui::GetStyle();
        ImVec4* colors = m_style->Colors;

        colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
        colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
        colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
        colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
        colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

        m_style->ChildRounding = 4.0f;
        m_style->FrameBorderSize = 1.0f;
        m_style->FrameRounding = 2.0f;
        m_style->GrabMinSize = 7.0f;
        m_style->PopupRounding = 2.0f;
        m_style->ScrollbarRounding = 12.0f;
        m_style->ScrollbarSize = 13.0f;
        m_style->TabBorderSize = 1.0f;
        m_style->TabRounding = 0.0f;
        m_style->WindowRounding = 4.0f;
    }
} // namespace Frac