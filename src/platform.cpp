#include "platform.h"
#include "control_flags.h"

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
		int n = 0;
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