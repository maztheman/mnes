#pragma once

#include <array>
#include <functional>
#include <mutex>

#include "Renderer.h"

struct GLFWwindow;

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

    /*void updateProjection(int width, int height)
    {
        m_Renderer.updateProjection(width, height);
    }*/

    void updateMainTexture();

private:

    void reset();
    void init();
    void GUIStuff();

    static Application* mainApplication;

    GLFWwindow* m_window;
    Renderer m_Renderer;
    std::mutex m_mutex;
};

const std::array<int, 8>& getKeysInOrder();


