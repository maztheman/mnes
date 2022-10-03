#include "gfx.h"
#include "platform.h"
#include "control_flags.h"

GLuint g_txMainWindow = {0};
GLuint g_txPatternTable = {0};
GLuint g_txNameTable = {0};

gfx::COpenGLWrapper* g_pMainFrame;
gfx::COpenGLWrapper* g_pPatternTableFrame;
gfx::COpenGLWrapper* g_pNameTableFrame;

void SetTextureData( void* pTextureData, GLuint texture )
{
	glBindTexture(GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pTextureData );
}

void DrawFrame()
{
	if ( !g_bDisplayReady) return;
	
	int nWidth = 0;
	int nHeight = 0;
	g_pMainFrame->GetSize( nWidth, nHeight );
	g_pMainFrame->MakeCurrent();
	glClearColor (0.0,0.0,0.0,1.0);
	glClear (GL_COLOR_BUFFER_BIT);
	glBindTexture (GL_TEXTURE_2D, g_txMainWindow);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);glVertex2i(0,0);
	glTexCoord2i(1,0);glVertex2i(nWidth,0);
	glTexCoord2i(1,1);glVertex2i(nWidth,nHeight);
	glTexCoord2i(0,1);glVertex2i(0,nHeight);
	glEnd();
	g_pMainFrame->Swap();
	
}

void DrawPatternTableFrame()
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
