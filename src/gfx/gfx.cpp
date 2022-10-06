#include "gfx.h"
#include "platform.h"

#include <common/control_flags.h>

//uint32_t g_txMainWindow = {0};
uint32_t g_txPatternTable = {0};
uint32_t g_txNameTable = {0};





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