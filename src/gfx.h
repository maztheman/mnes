#ifndef __GFX_H__
#define __GFX_H__

#include "platform.h"
#include <gl/GLU.h>

extern gfx::COpenGLWrapper* g_pMainFrame;
extern gfx::COpenGLWrapper* g_pPatternTableFrame;
extern gfx::COpenGLWrapper* g_pNameTableFrame;

void DrawFrame();
void DrawPatternTableFrame();
void DrawNameTableFrame();

void SetTextureData(void* pTextureData, GLuint texture);

extern GLuint g_txMainWindow;
extern GLuint g_txPatternTable;
extern GLuint g_txNameTable;

#endif