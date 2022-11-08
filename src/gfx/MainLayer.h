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

private:

    void OnKeyEvent(Key key, int action, int extra, bool& handled) override;

    void readIni();
    void writeIni(std::filesystem::path lastDir);
    void addMenu();

    bool m_bShowFileBrowser;
    std::shared_ptr<TextureType> m_Texture;
    FileBrowser m_Browser;
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point last = now;
};


const std::shared_ptr<MainLayer>& Main();