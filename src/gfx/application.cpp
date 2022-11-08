#include "application.h"

#include <mutex>
#include <functional>

#include "processor.h"
#include <ppu/ppu.h>
#include <cpu/cpu.h>
#include <cpu/cpu_registers.h>
#include <sound/apu.h>
#include <common/global.h>
#include <imgui.h>

#include <thread>
#include <fstream>

#include <common/FileLoader.h>

#include "MainLayer.h"

constexpr const int WINDOW_WIDTH = 800;
constexpr const int WINDOW_HEIGHT = 600;

using namespace std::string_view_literals;

MnesApplication::MnesApplication()
: GfxEngine::Core::Application<ApplicationPlatform>(WINDOW_WIDTH, WINDOW_HEIGHT, "mnes 1.0.0"sv)
{
    
}

#if 0
void Application::showFileBrowser()
{
    if (m_bShowFileBrowser)
    {
        bool shouldMoveDir = false;
        std::filesystem::directory_entry movedDir;

        if (ImGui::Begin("FileRom", &m_bShowFileBrowser))
        {
            if (ImGui::BeginListBox("##CurrentFiles", ImVec2{-1.0f, -1.0f}))
            {
                if (ImGui::Selectable(".."))
                {
                    shouldMoveDir = true;
                    movedDir = std::filesystem::directory_entry(m_Browser.getCurrentPath().parent_path());
                }
                else
                {
                    for(const auto& entry : m_Browser.getFilesFromCurrent())
                    {
                        if (entry.is_regular_file() && (entry.path().extension() == ".nes" || entry.path().extension() == ".NES"))
                        {
                            if (ImGui::Selectable(entry.path().filename().c_str()))
                            {
                                Stop();
                                if (CFileLoader::LoadRom(entry.path()))
                                {
                                    reset();
                                    Start();
                                    m_bShowFileBrowser = false;
                                }
                            }
                        } 
                        else if (entry.is_directory())
                        {
                            if (ImGui::Selectable(fmt::format("[D] {}", entry.path().filename().c_str()).c_str()))
                            {
                                shouldMoveDir = true;
                                movedDir = entry;
                            }
                        }
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::End();
        }

        if (shouldMoveDir)
        {
            if (movedDir.is_directory())
            {
                m_Browser.moveTo(movedDir);
                writeIni(movedDir.path());
            }
        }
    }
}


void Application::GUIStuff()
{
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    addMenu();
    showFileBrowser();

    ImGuiWindowFlags outputFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar;

    if (ImGui::Begin("Output", nullptr, outputFlags))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Tick: %ld  ", g_Registers.tick_count);
        ImGui::SameLine();
        ImGui::Text("Update tx Tick: %ld", m_Renderer.getUpdateTick());
        ImGui::End();
    }

    

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::updateMainTexture()
{
    m_Renderer.updateMainWindowTexture();
}


void Application::addMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", nullptr, &m_bShowFileBrowser))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Emulation"))
        {
            if (ImGui::MenuItem("Start"))
            {
                Start();
            }
            if (ImGui::MenuItem("Stop"))
            {
                Stop();
            }
            if (ImGui::MenuItem("Resume"))
            {
                Resume();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Application::readIni()
{
    if (std::ifstream settings("mnes.ini", std::ios::in | std::ios::app); settings)
    {
        std::string lastDir;
        std::getline(settings, lastDir);
        if (!lastDir.empty())
        {
            m_Browser.moveTo(std::filesystem::directory_entry{lastDir});
        }
    }
}

void Application::writeIni(std::filesystem::path lastDir)
{
    std::ofstream settings("mnes.ini", std::ios::out | std::ios::ate);
    settings << lastDir.generic_string();
}


int Application::run()
{
    init();
    readIni();
    m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "mnes 0.3.0", nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    if (gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)) == 0) {
        printf("Failed to initialize GLAD\n");
    } else {
        printf("gladInit()\n");
    }
    glfwSetWindowUserPointer(m_window, this);
    //glfwSwapInterval(0);

    const char* glsl_version = "#version 330";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    m_Renderer.init();

    while(glfwWindowShouldClose(m_window) == 0) {
        glfwPollEvents();
        glfwMakeContextCurrent(m_window);
        Process();
        GUIStuff();
        glfwSwapBuffers(m_window);
    }

    apu_destroy();

	VBufferCollection().ResetContent();
	VLogCollection().ResetContent();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    
    glfwTerminate();

    return 0;
}


#endif
