#include "MainLayer.h"

#include "data.h"
#include "processor.h"
#include <cpu/cpu.h>
#include <cpu/cpu_registers.h>
#include <ppu/ppu.h>
#include <sound/apu.h>

#include <common/FileLoader.h>
#include <common/Log.h>

#include <imgui.h>

#include <execution>
#include <fstream>
#include <inttypes.h>

using namespace mnes;

namespace {
  void reset();
  void KeepAspectRatio(ImGuiSizeCallbackData *data);
  void KeepItASquare(ImGuiSizeCallbackData *data);
}


const std::shared_ptr<MainLayer> &Main()
{
  static std::shared_ptr<MainLayer> instance = std::make_shared<MainLayer>();
  return instance;
}

namespace {
void reset()
{
  cpu::reset();
  ppu::initialize();
  apu::initialize();
  cpu::initialize();
}

void KeepAspectRatio(ImGuiSizeCallbackData *data)
{
  float aspect_ratio = *(float *)data->UserData;
  data->DesiredSize.x = std::max(data->CurrentSize.x, data->CurrentSize.y);
  data->DesiredSize.y = std::round((data->DesiredSize.x / aspect_ratio) + 0.5f);
}

void KeepItASquare(ImGuiSizeCallbackData *data)
{
  data->DesiredSize.x = data->DesiredSize.y = std::max(data->CurrentSize.x, data->CurrentSize.y);
}
}

MainLayer::~MainLayer() {}

void MainLayer::OnAttach(WindowHandle window)
{
  m_Window = window;
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

void MainLayer::OnDetach() {}

bool MainLayer::IsKeyPressed(Key key) const { return API::IsKeyPressed(m_Window, key); }

void MainLayer::OnKeyEvent(Key key, int action, int extra, bool &handled) { handled = true; }

void MainLayer::readIni()
{
  log::main()->info("Reading mnes.ini from path {}", std::filesystem::current_path().generic_string());
  if (std::ifstream settings(std::filesystem::current_path() / "mnes.ini", std::ios::in); settings) {
    std::string lastDir;
    std::getline(settings, lastDir);
    if (!lastDir.empty()) { m_Browser.moveTo(std::filesystem::directory_entry{ lastDir }); }
  }
}

void MainLayer::writeIni(std::filesystem::path lastDir)
{
  std::ofstream settings(std::filesystem::current_path() / "mnes.ini", std::ios::out | std::ios::ate);
  settings << lastDir.generic_string();
}

void MainLayer::addMenu()
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open", nullptr, &m_bShowFileBrowser)) {}
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Emulation")) {
      if (ImGui::MenuItem("Start")) { Start(); }
      if (ImGui::MenuItem("Stop")) { Stop(); }
      if (ImGui::MenuItem("Resume")) { Resume(); }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Zoom")) {
      if (ImGui::MenuItem("1x")) { m_ZoomLevel = 1.0f; }
      if (ImGui::MenuItem("2x")) { m_ZoomLevel = 2.0f; }
      if (ImGui::MenuItem("3x")) { m_ZoomLevel = 3.0f; }
      if (ImGui::MenuItem("4x")) { m_ZoomLevel = 4.0f; }

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Log")) {
      if (ImGui::BeginMenu("PPU")) {
          static auto ppulogger = log::ppu();
          if (ImGui::MenuItem("Off")) ppulogger->set_level(spdlog::level::off);
          if (ImGui::MenuItem("Critical")) ppulogger->set_level(spdlog::level::critical);
          if (ImGui::MenuItem("Error")) ppulogger->set_level(spdlog::level::err);
          if (ImGui::MenuItem("Warning")) ppulogger->set_level(spdlog::level::warn);
          if (ImGui::MenuItem("Info")) ppulogger->set_level(spdlog::level::info);
          if (ImGui::MenuItem("Debug")) ppulogger->set_level(spdlog::level::debug);
          if (ImGui::MenuItem("Trace")) ppulogger->set_level(spdlog::level::trace);
          ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Main")) {
          static auto mainlogger = log::main();
          if (ImGui::MenuItem("Off")) mainlogger->set_level(spdlog::level::off);
          if (ImGui::MenuItem("Critical")) mainlogger->set_level(spdlog::level::critical);
          if (ImGui::MenuItem("Error")) mainlogger->set_level(spdlog::level::err);
          if (ImGui::MenuItem("Warning")) mainlogger->set_level(spdlog::level::warn);
          if (ImGui::MenuItem("Info")) mainlogger->set_level(spdlog::level::info);
          if (ImGui::MenuItem("Debug")) mainlogger->set_level(spdlog::level::debug);
          if (ImGui::MenuItem("Trace")) mainlogger->set_level(spdlog::level::trace);
          ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}




void MainLayer::OnImGui()
{
  if (ImGuiWindowFlags outputFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar;
      ImGui::Begin("Output", nullptr, outputFlags)) {
    ImGui::Text(
      "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Tick: %" PRIu64, cpu::mnes_tick_count());
    ImGui::End();
  }

  addMenu();

  if (m_bShowFileBrowser) {
    bool shouldMoveDir = false;
    std::filesystem::directory_entry movedDir;

    if (ImGui::Begin("FileRom", &m_bShowFileBrowser)) {
      if (ImGui::BeginListBox("##CurrentFiles", ImVec2{ -1.0f, -1.0f })) {
        if (ImGui::Selectable("..")) {
          shouldMoveDir = true;
          movedDir = std::filesystem::directory_entry(m_Browser.getCurrentPath().parent_path());
        } else {
          for (const auto &entry : m_Browser.getFilesFromCurrent()) {
            if (entry.is_regular_file() && (entry.path().extension() == ".nes" || entry.path().extension() == ".NES")) {
              if (ImGui::Selectable(entry.path().filename().string().c_str())) {
                Stop();
                if (file::load_nes_file(entry.path())) {
                  reset();
                  Start();
                  m_bShowFileBrowser = false;
                  m_bFileLoaded = true;
                  m_RomName = entry.path().stem().generic_string();
                }
              }
            } else if (entry.is_directory()) {
              if (ImGui::Selectable(fmt::format("[D] {}", entry.path().filename().string()).c_str())) {
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

    if (shouldMoveDir) {
      if (movedDir.is_directory()) {
        m_Browser.moveTo(movedDir);
        writeIni(movedDir.path());
      }
    }
  }

  if (m_bFileLoaded) {
    ImGui::SetNextWindowSize(ImVec2(256.0f * m_ZoomLevel, 224.0f * m_ZoomLevel));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    ImGui::Begin(m_RomName.c_str(), &m_bFileLoaded, 0);// ImGuiWindowFlags_NoTitleBar
    ImGui::PopStyleVar();
    // Get the size of the child (i.e. the whole draw size of the windows).
    ImVec2 wsize = ImGui::GetWindowSize();
    wsize.y -= ImGui::GetFrameHeight();

    static constexpr ImVec2 txTL = { 0.0f, 8.0f / 256.0f };
    static constexpr ImVec2 txBR = { 1.0f, 231.0f / 256.0f };

    // Because I use the texture from OpenGL, I need to invert the V from the UV.
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(m_Texture->GetTextureID())), wsize, txTL, txBR);
    ImGui::End();

    if (m_bFileLoaded == false) { Stop(); }
  }
}

void MainLayer::UpdateTexture()
{
  gfx::clear_off_screen_data();
  m_Texture->SetData(gfx::screen_data());
}
