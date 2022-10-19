#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

#include <imgui_impl_glfw.cpp>
#include <imgui_impl_opengl3.cpp>


constexpr const int WINDOW_WIDTH = 800;
constexpr const int WINDOW_HEIGHT = 600;


Application* Application::mainApplication = nullptr;

const std::array<int, 8>& getKeysInOrder()
{
    static std::array<int, 8> s_Keys = {
        GLFW_KEY_APOSTROPHE, GLFW_KEY_SEMICOLON,  //A, B
        GLFW_KEY_K, GLFW_KEY_L, //select, start
        GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D //up, down, left, right
    };
    return s_Keys;
}

void Application::reset()
{
    cpu_reset();
}

void Application::init()
{
    printf("glfwInit()\n");
	if (GLFW_FALSE == glfwInit())
	{
		printf("Error initting glfw\n");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int Application::getKeyState(int key)
{
    return glfwGetKey(m_window, key) == GLFW_PRESS ? 1 : 0;
}


void Application::GUIStuff()
{
    extern bool g_bDisplayReady;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags outputFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar;


    if (ImGui::Begin("Output", nullptr, outputFlags))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Tick: %ld  ", g_Registers.tick_count);
        ImGui::SameLine();
        ImGui::Text("Update tx Tick: %ld", m_Renderer.getUpdateTick());
        ImGui::End();
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
            ImGui::Image((ImTextureID)m_Renderer.getMainTextureID(), wsize);
            ImGui::EndChild();
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();

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


int Application::run()
{
    Start();
    
    init();

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

    ppu_initialize();
    apu_initialize();
    cpu_initialize();

    reset();

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
