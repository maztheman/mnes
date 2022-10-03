#pragma once

//#include "platform.h"
//#include <gl/GLU.h>

//extern gfx::COpenGLWrapper* g_pMainFrame;
//extern gfx::COpenGLWrapper* g_pPatternTableFrame;
//extern gfx::COpenGLWrapper* g_pNameTableFrame;

void DrawFrame();
//void DrawPatternTableFrame();
//void DrawNameTableFrame();


void SetTextureData(void* pTextureData, uint32_t texture);

extern uint32_t g_txMainWindow;
extern uint32_t g_txPatternTable;
extern uint32_t g_txNameTable;
