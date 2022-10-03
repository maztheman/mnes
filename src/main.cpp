#include "global.h"

#include <iostream>
#include "FileLoader.h"
#include "platform.h"
#include "processor.h"
#include "mapper.h"
#include "apu.h"
#include "msound.h"

#ifdef _DEBUG
#pragma comment(lib, "libminputd.lib")
#pragma comment(lib , "libmsound32d.lib")
#else
#pragma comment(lib, "libminput.lib")
#pragma comment(lib , "libmsound32.lib")
#endif

class CMainMenu
{
public:
	CMainMenu()
	{
	}

	void Initialize()
	{
		/*
		int nFile = glutCreateMenu( File );
		glutAddMenuEntry( "Open", ID_FILE_OPEN );
		glutAddMenuEntry( "Close", ID_FILE_CLOSE );
		int nEmulation = glutCreateMenu( Emulation );
		glutAddMenuEntry( "Start", ID_EMULATION_START );
		glutAddMenuEntry( "Stop", ID_EMULATION_STOP );
		glutAddMenuEntry( "Reset (HARD)", ID_EMULATION_RESET_HARD );
		glutAddMenuEntry( "Reset (SOFT)", ID_EMULATION_RESET_SOFT );
		glutCreateMenu( BaseMenu );
		glutAddSubMenu( "File", nFile );
		glutAddSubMenu( "Emulation", nEmulation );
		glutAttachMenu( GLUT_RIGHT_BUTTON );
		*/
	}

	void SimulateEmulationStart()
	{
		Emulation( ID_EMULATION_START );
	}

	int GetMenuID() const { return m_nMenuID; }

private:

	static void __cdecl File( int nItem )
	{
		switch( nItem )
		{
		default:
			break;
		case ID_FILE_OPEN:
			break;
		case ID_FILE_CLOSE:
			break;
		}
	}

	static void __cdecl Emulation( int nItem )
	{
		static int nState = 0;
		switch( nItem )
		{
		default:
			break;
		case ID_EMULATION_START:
			Start();
			break;
		case ID_EMULATION_STOP:
			Stop();
			break;
		case ID_EMULATION_RESET_HARD:
			Start();
			break;
		case ID_EMULATION_RESET_SOFT:
			Start();
			break;
		}
	}

	static void __cdecl BaseMenu( int nItem )
	{

	}


	int m_nMenuID{ -1 };
};

extern mapper_t* g_mapper;

int main( int argc, char* argv[] )
{
#ifdef _WIN32
	::ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif
	
	bool bLoaded = false;

	if (argc == 2) {
		bLoaded = CFileLoader::LoadRom(argv[1]);
	}

	msound_initialize();

	InitializeProcessor();

	if (bLoaded) {
		CMainMenu menu;
		if (g_mapper != nullptr) {
			menu.SimulateEmulationStart();
		}
	}

	mazMainLoop();

	VBufferCollection().ResetContent();
	VLogCollection().ResetContent();
	
	apu_destroy();

	msound_finalized();

	return 0;
}