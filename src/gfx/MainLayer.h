#pragma once

#include "FileBrowser.h"
#include "common/Cyclediff.h"

#include <Core/LayerT.h>

#include <filesystem>


class MainLayer : public GfxEngine::Core::LayerT<ApplicationPlatform>
{
public:
    virtual ~MainLayer();
    void OnAttach(WindowHandle window) override;
    void OnUpdate(Timediff ts) override;
    void OnDetach() override;
    void OnImGui() override;

    void UpdateTexture();

    bool IsKeyPressed(Key key) const;

private:

    void OnKeyEvent(Key key, int action, int extra, bool& handled) override;

    void readIni();
    void writeIni(std::filesystem::path lastDir);
    void addMenu();

    bool m_bShowFileBrowser;
    std::shared_ptr<TextureType> m_Texture;
    WindowHandle m_Window;
    FileBrowser m_Browser;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point last = now;
    int m_ZoomLevel{1};
    bool m_bFileLoaded{false};
    std::string m_RomName;
};


const std::shared_ptr<MainLayer>& Main();