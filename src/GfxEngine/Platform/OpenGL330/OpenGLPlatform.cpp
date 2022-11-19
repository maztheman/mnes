#include "OpenGLPlatform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLImGui.h"

#include <mutex>

#include <glm/glm.hpp>

namespace GfxEngine::Platform::OpenGL330
{

static std::once_flag initialize, gladInit;

static void InitializeGlad()
{
    std::call_once(gladInit, []() {
        if (gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)) == 0) {
            printf("Failed to initialize GLAD\n");
        } else {
            printf("gladInit()\n");
        }
    });
}

static void WindowCloseCallback(GLFWwindow* window);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void WindowSizeCallback(GLFWwindow* window, int width, int height);

void OpenGL330Platform::Init()
{
    std::call_once(initialize, []() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    });
}

void OpenGL330Platform::Shutdown()
{
    OpenGLImGui::Shutdown();
    glfwTerminate();
}

void OpenGL330Platform::PollEvents()
{
    glfwPollEvents();
}

Window::WindowHandle OpenGL330Platform::Create(int width, int height, WindowData* pUserData, std::string_view title/* = ""sv*/)
{
    Init();
    WindowHandle window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    InitializeGlad();
    glfwSwapInterval(0);
    glfwSetWindowUserPointer(window, pUserData);
    glfwSetWindowCloseCallback(window, WindowCloseCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);

    static glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);

    glViewport(0, 0, width, height);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);   

    return window;
}

void OpenGL330Platform::InitializeImGuiLayer(WindowHandle window)
{
    OpenGLImGui::Initialize(window);
}

void OpenGL330Platform::ImGuiBeginRender()
{
    OpenGLImGui::BeginRender();
}

void OpenGL330Platform::ImGuiEndRender()
{
    OpenGLImGui::EndRender();
}

void OpenGL330Platform::SwapBuffers(WindowHandle window)
{
    glfwSwapBuffers(window);
}

static constexpr std::array<int, 78> KEY_MAP = {
    GLFW_KEY_A,
    GLFW_KEY_W,
    GLFW_KEY_D,
    GLFW_KEY_S,
    GLFW_KEY_APOSTROPHE,
    GLFW_KEY_SEMICOLON,
    GLFW_KEY_L,
    GLFW_KEY_K
};


bool OpenGL330Platform::IsKeyPressed(WindowHandle window, Key key)
{
    return glfwGetKey(window, KEY_MAP[key]) == GLFW_PRESS;
}


void OpenGL330Platform::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}


void OpenGL330Platform::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

//callbacks
static void WindowCloseCallback(GLFWwindow* window)
{
    auto pData = reinterpret_cast<OpenGL330Platform::WindowData*>(glfwGetWindowUserPointer(window));
    if (pData && pData->onWindowCloseFn)
    {
        pData->onWindowCloseFn();
    }
}


static constexpr Key GLFWKeyToKey(int key)
{
    switch(key)
    {
    case GLFW_KEY_A:
    return Key::A;
    case GLFW_KEY_W:
    return Key::W;
    case GLFW_KEY_D:
    return Key::D;
    case GLFW_KEY_S:
    return Key::S;
    case GLFW_KEY_APOSTROPHE:
    return Key::Apos;
    case GLFW_KEY_SEMICOLON:
    return Key::Semicolon;
    case GLFW_KEY_L:
    return Key::L;
    case GLFW_KEY_K:
    return Key::K;
        default: 
        return Key::Invalid;
    }
}


static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto pData = reinterpret_cast<OpenGL330Platform::WindowData*>(glfwGetWindowUserPointer(window));
    if (pData && pData->onKeyEvent)
    {
        pData->onKeyEvent(GLFWKeyToKey(key), action == GLFW_PRESS ? 1 : 0, mods);
    }
}


static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

}