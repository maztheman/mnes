#ifdef _WIN32

#include "platform.h"

#include "control_flags.h"
#include "FileLoader.h"
#include "processor.h"

using gfx::COpenGLWrapper;
using gfx::COpenGLWrapperPtrVector;
using gfx::CHwndOpenGLWrapperPtrMap;

COpenGLWrapperPtrVector g_OpenGLWindows;
CHwndOpenGLWrapperPtrMap g_WindowMapper;


gfx::CMainframeMenu g_MainframeMenu;

COpenGLWrapper::COpenGLWrapper()
	: GL_WINDOW_CLASS("GL_MAZ_WINDOW")
	, m_hOpenGLContext(nullptr)
	, m_pfnDraw(nullptr)
	, m_pfnCalculate(nullptr)
	, m_hMenu(nullptr)
{
	WNDCLASSEX wsex = {0};

	if ( 0 == GetClassInfoEx( GetModuleHandle(NULL), GL_WINDOW_CLASS.c_str(), &wsex ) )
	{
		wsex.cbSize = sizeof(WNDCLASSEX);
		wsex.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wsex.hInstance = GetModuleHandle(NULL);
		wsex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wsex.lpfnWndProc = &COpenGLWrapper::StaticProc;
		wsex.lpszClassName = GL_WINDOW_CLASS.c_str();
		ATOM hum = RegisterClassEx( &wsex );
	}
}

int COpenGLWrapper::Create(unsigned int nHeight, unsigned int nWidth, const string& sTitle)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	m_hWnd = ::CreateWindow( GL_WINDOW_CLASS.c_str(), sTitle.c_str(), WS_CAPTION, 100, 100, nWidth, nHeight, NULL, m_hMenu, hInstance, this );

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
		24,                        //Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                        //Number of bits for the depthbuffer
		8,                        //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	m_hDC = GetDC( m_hWnd );

	int pixelFormat = ::ChoosePixelFormat( m_hDC, &pfd );

	if ( ::SetPixelFormat( m_hDC, pixelFormat, &pfd ) == FALSE )
		return -1;

	m_hOpenGLContext = ::wglCreateContext( m_hDC );

	::ShowWindow( m_hWnd, SW_SHOW );

	return m_nWindowID;
}


COpenGLWrapper::~COpenGLWrapper()
{
	if ( m_hOpenGLContext != nullptr )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_hOpenGLContext );
		m_hOpenGLContext = nullptr;
	}
}

void COpenGLWrapper::MakeCurrent()
{
	wglMakeCurrent( m_hDC, m_hOpenGLContext ); 
}

void COpenGLWrapper::Draw()
{
	OnPreDraw();
	m_pfnDraw();
}

void COpenGLWrapper::Swap()
{
	SwapBuffers( m_hDC );
}

void COpenGLWrapper::Resize( int nWidth, int nHeight )
{
	::SetWindowPos( m_hWnd, HWND_TOP, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER );
}

void COpenGLWrapper::GetSize( int& nWidth, int& nHeight )
{
	nWidth = m_nWidth;
	nHeight = m_nHeight;
}

void COpenGLWrapper::OnPreDraw()
{
	OnCalculate();
	wglMakeCurrent( m_hDC, m_hOpenGLContext ); 
}

void OpenARom(HWND hWnd)
{
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "*.nes (ines 1.0 format)\0*.nes\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		// use ofn.lpstrFile
		CFileLoader::LoadRom(ofn.lpstrFile);
	}
}


LRESULT WINAPI COpenGLWrapper::StaticProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	if (Msg == WM_NCCREATE) {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		COpenGLWrapper* pWrapper = reinterpret_cast<COpenGLWrapper*>(pCreate->lpCreateParams);
		g_OpenGLWindows.push_back( pWrapper );
		pWrapper->m_nWindowID = g_OpenGLWindows.size() - 1;
		g_WindowMapper.insert(std::make_pair(hWnd, pWrapper));
	}

	switch(Msg)
	{
	default:
		break;
	case WM_SIZE:
		{
			COpenGLWrapper* pWrapper = g_WindowMapper[ hWnd];
			pWrapper->MakeCurrent();
			int nWidth = pWrapper->m_nWidth = LOWORD(lParam);
			int nHeight = pWrapper->m_nHeight = HIWORD(lParam);
			glViewport(0, 0, nWidth, nHeight);
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity();
			glOrtho (0, nWidth, nHeight, 0, 0, 1);
			glMatrixMode (GL_MODELVIEW);
			glLoadIdentity();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (wParam) {
		case ID_FILE_EXIT:
			::DestroyWindow(hWnd);
			break;
		case ID_FILE_OPEN:
			Stop();
			OpenARom(hWnd);
			break;
		case ID_EMULATION_START:
			Start();
			break;
		case ID_EMULATION_STOP:
			Stop();
			break;
		case ID_EMULATION_RESUME:
			Resume();
			break;
		case ID_EMULATION_RESET_HARD:
			Start();
			break;
		case ID_EMULATION_RESET_SOFT:
			Start();
			break;
		}
		break;
	case WM_KEYDOWN:
		{
			if ( wParam == VK_ESCAPE )
			{
				::DestroyWindow( hWnd );
			}
		}
		break;
	}
		
	return DefWindowProc( hWnd, Msg, wParam, lParam );
}

void mazMainLoop()
{
	MSG msg;
	bool done = false;
	while (!done) {
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				done = true;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			if (g_OpenGLWindows.empty()) {
				continue;
			}
			COpenGLWrapper** ppWindows = g_OpenGLWindows.data();
			for(size_t n = 0; n < g_OpenGLWindows.size(); n++) {
				ppWindows[n]->Draw();
			}
		}
	}
	g_WindowMapper.clear();
	g_OpenGLWindows.ResetContent();
}

void COpenGLWrapper::OnCalculate()
{
	if ( m_pfnCalculate != nullptr )
	{
		m_pfnCalculate();				
	}
}


/*
	
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
 //declare functions
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;


//get extensions of graphics card
	char* extensions = (char*)glGetString(GL_EXTENSIONS);
	
	if (strstr(extensions,"WGL_EXT_swap_control"))
	{
		wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
		wglSwapIntervalEXT(1);//enable v-sync baby
	}

	memset( &patternTableBuff[0], 128, 0x30000);

	g_nPatternTable = glutCreateWindow("Pattern Table $0000");
	glutSetWindow( g_nPatternTable );
	glutDisplayFunc( PatternTableDraw );
	glutReshapeFunc( ResizeWindow );
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &patternTable );
	glBindTexture(GL_TEXTURE_2D, patternTable );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, patternTableBuff );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	*/

#else

#include "platform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <mutex>
#include <functional>

#include "fragment.cpp"
#include "vertex.cpp"

constexpr const int WINDOW_WIDTH = 800;
constexpr const int WINDOW_HEIGHT = 600;


const std::array<int, 8>& getKeysInOrder()
{
    static std::array<int, 8> s_Keys = {
        GLFW_KEY_APOSTROPHE, GLFW_KEY_SEMICOLON,  //A, B
        GLFW_KEY_K, GLFW_KEY_L, //select, start
        GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D //up, down, left, right
    };

    return s_Keys;
}

static void nop()
{

}

struct WindowData
{
	GLFWwindow* window = nullptr;
	std::function<void()> m_Calculate = nop;
	std::function<void()> m_Display = nop;
};

struct RenderData
{
	GLuint quadVAO;
	GLuint mainwindowTextureId;
	GLuint program;

	static constexpr const char* UNIFORM_PROJECTION = "projection";
	static constexpr const char* UNIFORM_MODEL = "model";
	static constexpr const char* UNIFORM_IMAGE = "image";

	void initVertexBuffers()
	{
		unsigned int VBO;
		float verficies[] = {
				0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,

				0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				1.0f, 0.0f, 1.0f, 0.0f

		};

		glGenVertexArrays(1, &quadVAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verficies), verficies, GL_STATIC_DRAW);

		glBindVertexArray(quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void initMainWindowTexture()
	{
		extern uchar* g_pScreenBuffer;

		glGenTextures(1, &mainwindowTextureId );
		glBindTexture(GL_TEXTURE_2D, mainwindowTextureId );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, g_pScreenBuffer );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Filtering

	}

	void init()
	{
		initVertexBuffers();
		initMainWindowTexture();
		createProgramAndShaders();
		initUniforms();
	}

	void updateMainWindowTexture()
	{
		extern uchar* g_pScreenBuffer;

		glBindTexture (GL_TEXTURE_2D, mainwindowTextureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, g_pScreenBuffer);
	}


	void createProgramAndShaders()
	{
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShader);

		GLint success;

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
			char infoLog[1024];
            glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
			printf("[VertexShader]: Error %s\n", infoLog);
			exit(-1);
        }

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &FRAGMENT_SOURCE, nullptr);
    	glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
			char infoLog[1024];
            glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
			printf("[FragmentShader]: Error %s\n", infoLog);
			exit(-1);
		}

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
			char infoLog[1024];
			glGetProgramInfoLog(program, 1024, NULL, infoLog);
			printf("[Program]: Error %s\n", infoLog);
			exit(-1);
		}

		glDeleteShader(vertexShader);
    	glDeleteShader(fragmentShader);	
	}

	void initUniforms()
	{
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, UNIFORM_IMAGE), 0);
	}

	void updateProjection(int width, int height)
	{
		glm::mat4 projection = glm::ortho(0.0f, float(width), float(height), 0.0f, -1.0f, 1.0f);
		glUseProgram(program);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(float(width), float(height), 1.0f));

		//set the uniform "model"
		glUniformMatrix4fv(glGetUniformLocation(program, UNIFORM_MODEL), 1, false, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(program, UNIFORM_PROJECTION), 1, false, glm::value_ptr(projection));
	}

	void draw()
	{

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mainwindowTextureId);
		
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
};

static RenderData& renderData()
{
	static RenderData instance;
	return instance;
}

struct AllWindowData
{
	std::vector<WindowData> windows;
};

std::vector<WindowData>& windows()
{
	static AllWindowData instance;
	return instance.windows;
}

CGfxManager::CGfxManager()
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

CGfxManager::~CGfxManager()
{
	windows().clear();//don't dangle
	glfwTerminate();
}

// should probably rethink how we create this window. Maybe use ImGui to

uint64_t CGfxManager::getMainWindow()
{
	static std::once_flag once;
	static uint64_t mainWindowIndex;

	std::call_once(once, []() {
		//initialize first
		(void)CGfxManager::instance();
		WindowData data;
		data.window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "mnes 0.3.0", nullptr, nullptr);
		glfwMakeContextCurrent(data.window);
		if (gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)) == 0)
		{
			printf("Failed to initialize GLAD\n");
		} else {
			printf("gladInit()\n");
		}
		mainWindowIndex = windows().size();
		windows().push_back(data);
	});


	return mainWindowIndex;
}

void setupProjection(int width, int height)
{
	//glViewport(0, 0, width, height);
	renderData().updateProjection(width, height);
}



void CGfxManager::mainLoop()
{
	static COpenGLWrapper main(getMainWindow());
	printf("mainloop\n");
	main.MakeCurrent();
	setupProjection(WINDOW_WIDTH, WINDOW_HEIGHT);
		
	WindowData& data = windows()[getMainWindow()];
	glfwSetFramebufferSizeCallback(data.window, [](GLFWwindow*, int width, int height)
	{
		glViewport(0,0,width,height);
		setupProjection(width,height);
	});
		
    while(!main.ShouldClose()) {
        glfwPollEvents();
		main.Draw();
	}
}

void COpenGLWrapper::GetSize(int& nWidth, int& nHeight) const
{
	WindowData& data = windows()[m_index];
	glfwGetWindowSize(data.window, &nWidth, &nHeight);
}

void COpenGLWrapper::Resize(int nWidth, int nHeight) const
{
	WindowData& data = windows()[m_index];
	glfwSetWindowSize(data.window, nWidth, nHeight);
	setupProjection(nWidth, nHeight);
}

void COpenGLWrapper::SetDisplayFunc(DisplayFunc function) const
{
	WindowData& data = windows()[m_index];
	data.m_Display = std::move(function);
}

void COpenGLWrapper::SetCalculateFunc(CalculateFunc function) const
{
	WindowData& data = windows()[m_index];
	data.m_Calculate = std::move(function);
}


void COpenGLWrapper::MakeCurrent() const
{
	WindowData& data = windows()[m_index];
	glfwMakeContextCurrent(data.window);
}

void COpenGLWrapper::Draw() const
{
	WindowData& data = windows()[m_index];
	OnPreDraw();
	data.m_Display();
}

void COpenGLWrapper::OnCalculate() const
{
	WindowData& data = windows()[m_index];
	data.m_Calculate();
}

void COpenGLWrapper::Swap() const
{
	WindowData& data = windows()[m_index];
	glfwSwapBuffers(data.window);

}

void COpenGLWrapper::OnPreDraw() const
{
	OnCalculate();
	MakeCurrent();
}

bool COpenGLWrapper::ShouldClose() const
{
	WindowData& data = windows()[m_index];
	return glfwWindowShouldClose(data.window) != 0;
}

int COpenGLWrapper::GetKeyState(int key)
{
	WindowData& data = windows()[m_index];
	return glfwGetKey(data.window, key) == GLFW_PRESS ? 1 : 0;
}

void COpenGLWrapperPtrVector::add(std::unique_ptr<COpenGLWrapper> value)
{


	m_data.push_back(std::move(value));
}


void UpdateMainWindowTexture()
{
	renderData().updateMainWindowTexture();
}

void InitializeRenderer()
{
	renderData().init();
}


void DrawFrame()
{
	extern bool g_bDisplayReady;
	static COpenGLWrapper mainframe(CGfxManager::getMainWindow());

	if ( !g_bDisplayReady) return;
	mainframe.MakeCurrent();
	renderData().draw();
	mainframe.Swap();
}

#endif
