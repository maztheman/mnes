#include "MainLayer.h"

#include "processor.h"
#include <ppu/ppu.h>
#include <cpu/cpu.h>
#include <cpu/cpu_registers.h>
#include <sound/apu.h>
#include <common/global.h>

#include <common/FileLoader.h>

#include <imgui.h>

#include <fstream>

const std::shared_ptr<MainLayer>& Main()
{
    static std::shared_ptr<MainLayer> instance = std::make_shared<MainLayer>();
    return instance;
}

static void reset()
{
    cpu_reset();
    ppu_initialize();
    apu_initialize();
    cpu_initialize();
}

MainLayer::~MainLayer()
{

}

void MainLayer::OnAttach(WindowHandle window)
{
    m_Texture = std::make_shared<TextureType>(GfxEngine::PixelType::RGB8, 256, 256);
    readIni();
}

void MainLayer::OnUpdate(Timediff ts)
{
    last = now;
    now = std::chrono::high_resolution_clock::now();

    Cyclediff cycles = std::chrono::duration_cast<Cyclediff>(now - last);

    Process(cycles.count());
}

void MainLayer::OnDetach()
{

}

void MainLayer::OnKeyEvent(Key key, int action, int extra, bool& handled)
{
    KEYS[key].second = action == 1 ? true : false;
    handled = true;
}

void MainLayer::readIni()
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

void MainLayer::writeIni(std::filesystem::path lastDir)
{
    std::ofstream settings("mnes.ini", std::ios::out | std::ios::ate);
    settings << lastDir.generic_string();
}

void MainLayer::addMenu()
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

void MainLayer::OnImGui()
{
    if (ImGuiWindowFlags outputFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar; ImGui::Begin("Output", nullptr, outputFlags))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Tick: %ld  ", g_Registers.tick_count);
        ImGui::End();
    }

    addMenu();

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


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f,0.0f});
    if (ImGui::Begin("GameWindow", nullptr, ImGuiWindowFlags_NoTitleBar))
    {
        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        if (ImGui::BeginChild("GameRender"))
        {
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(m_Texture->GetTextureID())), wsize);
            ImGui::EndChild();
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();
}

void MainLayer::UpdateTexture()
{
    m_Texture->SetData(getScreenData());
}

