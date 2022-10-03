#pragma once

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

constexpr unsigned int ID_FILE					= WM_USER + 1;
constexpr unsigned int ID_FILE_OPEN				= WM_USER + 2;
constexpr unsigned int ID_FILE_EXIT				= WM_USER + 3;
constexpr unsigned int ID_EMULATION				= WM_USER + 4;
constexpr unsigned int ID_VIDEO					= WM_USER + 5;
constexpr unsigned int ID_EMULATION_START		= WM_USER + 6;
constexpr unsigned int ID_EMULATION_STOP		= WM_USER + 7;
constexpr unsigned int ID_EMULATION_RESUME		= WM_USER + 8;
constexpr unsigned int ID_EMULATION_RESET		= WM_USER + 9;
constexpr unsigned int ID_EMULATION_RESET_HARD	= WM_USER + 10;
constexpr unsigned int ID_EMULATION_RESET_SOFT	= WM_USER + 11;


namespace gfx {



class CMainframeMenu
{
	HMENU m_hMenu;

	HMENU m_hFileMenu;
	HMENU m_hEmulationMenu;
	HMENU m_hResetMenu;
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

		int fileMenuPos = 0, emulationMenuPos = 0, resetMenuPos = 0;
		m_hFileMenu = ::CreateMenu();
		m_hResetMenu = ::CreateMenu();
		m_hEmulationMenu = ::CreateMenu();

		::InsertMenuA(m_hResetMenu, resetMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_EMULATION_RESET_HARD, "&Hard");
		::InsertMenuA(m_hResetMenu, resetMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_EMULATION_RESET_SOFT, "&Soft");
		
		MENUITEMINFOA resetMenu = { 0 };
		resetMenu.cbSize = sizeof(MENUITEMINFOA);
		resetMenu.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STRING;
		resetMenu.wID = ID_EMULATION_RESET;
		resetMenu.hSubMenu = m_hResetMenu;
		resetMenu.dwTypeData = "Reset";

		::InsertMenuA(m_hEmulationMenu, emulationMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_EMULATION_START, "&Start");
		::InsertMenuA(m_hEmulationMenu, emulationMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_EMULATION_STOP, "&Stop");
		::InsertMenuA(m_hEmulationMenu, emulationMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_EMULATION_RESUME, "&Resume");
		::InsertMenuItemA(m_hEmulationMenu, emulationMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, &resetMenu);

		MENUITEMINFOA emulationMenu = { 0 };
		emulationMenu.cbSize = sizeof(MENUITEMINFOA);
		emulationMenu.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STRING;
		emulationMenu.wID = ID_EMULATION;
		emulationMenu.hSubMenu = m_hEmulationMenu;
		emulationMenu.dwTypeData = "Emulation";

		::InsertMenuA(m_hFileMenu, fileMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_FILE_OPEN, "&Open");
		::InsertMenuA(m_hFileMenu, fileMenuPos++, MF_BYPOSITION | MF_ENABLED | MF_STRING, ID_FILE_EXIT, "&Exit");

		MENUITEMINFOA fileMenu = { 0 };
		fileMenu.cbSize = sizeof(MENUITEMINFOA);
		fileMenu.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STRING;
		fileMenu.wID = ID_FILE;
		fileMenu.hSubMenu = m_hFileMenu;
		fileMenu.dwTypeData = "File";

		::InsertMenuItemA(m_hMenu, pos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, &fileMenu);
		::InsertMenuItemA(m_hMenu, pos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, &emulationMenu);
		::InsertMenu(m_hMenu, pos++, MF_BYPOSITION | MF_ENABLED | MF_POPUP | MF_STRING, ID_VIDEO, "&Video");


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
