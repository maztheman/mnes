#include "gfx.h"
#include "platform.h"
#include "control_flags.h"
#include <GLFW/glfw3.h>

uint32_t g_txMainWindow = {0};
uint32_t g_txPatternTable = {0};
uint32_t g_txNameTable = {0};

void SetTextureData( void* pTextureData, uint32_t texture )
{
	glBindTexture(GL_TEXTURE_2D, GLuint(texture) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pTextureData);
}

void DrawFrame()
{
	if ( !g_bDisplayReady) return;
	
	int nWidth = 0;
	int nHeight = 0;
	COpenGLWrapper mainframe(CGfxManager::getMainWindow());

	mainframe.GetSize( nWidth, nHeight );
	mainframe.MakeCurrent();
	glClearColor (0.0,0.0,0.0,1.0);
	glClear (GL_COLOR_BUFFER_BIT);
	glBindTexture (GL_TEXTURE_2D, g_txMainWindow);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);glVertex2i(0,0);
	glTexCoord2i(1,0);glVertex2i(nWidth,0);
	glTexCoord2i(1,1);glVertex2i(nWidth,nHeight);
	glTexCoord2i(0,1);glVertex2i(0,nHeight);
	glEnd();
	mainframe.Swap();
	
}

/*void DrawPatternTableFrame()
{
	if ( !g_bPatternTableReady ) return;

	g_pPatternTableFrame->MakeCurrent();
	int nWidth = 0;
	int nHeight = 0;
	g_pPatternTableFrame->GetSize( nWidth, nHeight );
	glClearColor (0.0,0.0,0.0,1.0);
	glClear (GL_COLOR_BUFFER_BIT);
	glBindTexture (GL_TEXTURE_2D, g_txPatternTable);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);glVertex2i(0,0);
	glTexCoord2i(1,0);glVertex2i(nWidth,0);
	glTexCoord2i(1,1);glVertex2i(nWidth,nHeight);
	glTexCoord2i(0,1);glVertex2i(0,nHeight);
	glEnd();
	g_pPatternTableFrame->Swap();
}

void DrawNameTableFrame()
{
	if ( !g_bNameTableReady ) return;

	g_pNameTableFrame->MakeCurrent();
	int nWidth = 0;
	int nHeight = 0;
	g_pNameTableFrame->GetSize( nWidth, nHeight );
	glClearColor (0.0,0.0,0.0,1.0);
	glClear (GL_COLOR_BUFFER_BIT);
	glBindTexture (GL_TEXTURE_2D, g_txPatternTable);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);glVertex2i(0,0);
	glTexCoord2i(1,0);glVertex2i(nWidth,0);
	glTexCoord2i(1,1);glVertex2i(nWidth,nHeight);
	glTexCoord2i(0,1);glVertex2i(0,nHeight);
	glEnd();
	g_pNameTableFrame->Swap();
}
*/