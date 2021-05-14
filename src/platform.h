#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include <Windows.h>
#include <gl/GLU.h>

#include <string>
#include <map>
#include <vector>
using std::map;
using std::vector;
using std::string;

typedef void  (__cdecl *DisplayFunc)();
typedef void  (__cdecl *CalculateFunc)();

void mazMainLoop();

const unsigned int ID_FILE_MENU				= WM_USER + 1;
const unsigned int ID_FILE_OPEN				= WM_USER + 2;
const unsigned int ID_FILE_EXIT				= WM_USER + 3;
const unsigned int ID_EMULATION_MENU		= WM_USER + 4;
const unsigned int ID_VIDEO_MENU			= WM_USER + 5;
const unsigned int ID_EMULATION_START		= WM_USER + 6;
const unsigned int ID_EMULATION_STOP		= WM_USER + 7;
const unsigned int ID_EMULATION_RESET		= WM_USER + 8;



namespace gfx {



class CMainframeMenu
{
	HMENU m_hMenu;
public:
	CMainframeMenu() : m_hMenu(nullptr) {
		Create();
	}

	HMENU GetSafeHmenu() {
		return m_hMenu;
	}

private:
	void Create()
	{
		m_hMenu = ::CreateMenu();
		unsigned int i = 100;
		unsigned int pos = 0;

		HMENU hFile = ::CreatePopupMenu();
		HMENU hEmulation = ::CreatePopupMenu();
		HMENU hVideo = ::CreatePopupMenu();

		//::InsertMenu(hFile, 0, MF_BYPOSITION | MF_ENABLED | MF_STRING, 

		::InsertMenu(m_hMenu, pos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, ID_FILE_MENU, "&File");
		::InsertMenu(m_hMenu, pos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, ID_EMULATION_MENU, "&Emulation");
		::InsertMenu(m_hMenu, pos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, ID_VIDEO_MENU, "&Video");

	}
};

class COpenGLWrapper
{
	const string GL_WINDOW_CLASS;

public:
	COpenGLWrapper();
	~COpenGLWrapper();

	int Create(unsigned int nHeight, unsigned int nWidth, const string& sTitle);
	
	void SetDisplayFunc( DisplayFunc function )	{ m_pfnDraw = function; }
	void SetCalculateFunc( CalculateFunc function ) { m_pfnCalculate = function; }

	void MakeCurrent();
	void Draw();
	void OnCalculate();
	void Swap();
	void Resize( int nWidth, int nHeight );
	void GetSize( int& nWidth, int& nHeight );
	void SetMenu(HMENU hMenu) { m_hMenu = hMenu; }

	HWND GetWindowHandle() const {
		return m_hWnd;
	}

private:

	void OnPreDraw();

	static LRESULT WINAPI StaticProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

	int m_nWindowID;
	HWND m_hWnd;
	HGLRC m_hOpenGLContext;
	DisplayFunc m_pfnDraw;
	CalculateFunc m_pfnCalculate;
	HDC m_hDC;
	int m_nWidth;
	int m_nHeight;
	HMENU m_hMenu;
};

class CHwndOpenGLWrapperPtrMap : public map<HWND, COpenGLWrapper*>
{
public:
	CHwndOpenGLWrapperPtrMap() {}
	~CHwndOpenGLWrapperPtrMap() {}
};

class COpenGLWrapperPtrVector : public vector<COpenGLWrapper*>
{
public:
	COpenGLWrapperPtrVector() {}
	~COpenGLWrapperPtrVector()
	{
		ResetContent();
	}

	void ResetContent()
	{
		if (empty()) return;

		for (auto& p : *this) {
			delete p;
		}

		clear();

		COpenGLWrapperPtrVector().swap(*this);
	}
};

}

extern gfx::CMainframeMenu g_MainframeMenu;
inline gfx::CMainframeMenu& VMainframeMenu() { return g_MainframeMenu; }

#endif