#pragma once

#include <array>
#include <functional>
#include <mutex>

#include "Renderer.h"

#include "FileBrowser.h"

struct GLFWwindow;

//add some sort of rom state struct to keep some of the globals locally here.

class Application
{
public:
    Application()
    {
        mainApplication = this;
    }
    Application(const Application&) = delete;
    Application(Application&&) = delete;

    //later we can add something to the application to add pattern buffer table stuff

    //blocks until end
    int run();

    static Application* getApplication() 
    {
        return mainApplication;
    }

    std::vector<uint8_t>& getScreenBuffer()
    {
        return m_Renderer.screenBuffer();
    }

    int getKeyState(int key);

    void updateMainTexture();

private:

    void reset();
    void init();
    void GUIStuff();
    void addMenu();
    void showFileBrowser();
    void readIni();
    void writeIni(std::filesystem::path lastDir);

    static Application* mainApplication;

    GLFWwindow* m_window;
    Renderer m_Renderer;
    std::mutex m_mutex;
    bool m_bShowFileBrowser{false};
    FileBrowser m_Browser;
};

const std::array<int, 8>& getKeysInOrder();


