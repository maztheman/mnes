#include "processor.h"


#include <common/global.h>
#include <common/control_flags.h>

#include <cpu/cpu.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <cpu/memory_registers.h>
#include <cpu/cpu_opcodes.h>

#include <gfx/gfx.h>
#include <gfx/platform.h>

#include <ppu/ppu.h>
#include <ppu/ppu_registers.h>
#include <ppu/ppu_memory.h>

#include <sound/apu.h>

#include <GLFW/glfw3.h>

#define FIRST   0

//56
//45
//75
//88
//98

extern void cpu_reset();

vuchar g_pPatternTableBuffer(0x30000);

void CPUProcess();


static COpenGLWrapper& mainframe()
{
    static COpenGLWrapper instance(CGfxManager::getMainWindow());
    return instance;
}

void draw_frame()
{
    
	g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = true;
	mainframe().MakeCurrent();
	SetTextureData(g_pScreenBuffer, g_txMainWindow);
}

static void Process()
{
	if (!g_bCpuRunning) {
		return;
	}
	if (g_PPURegisters.scanline == FIRST) {
		g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = false;
	} 
	CPUProcess();
}

void InitializeProcessor()
{
    mainframe().MakeCurrent();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SMOOTH);
	glGenTextures(1, &g_txMainWindow );
	glBindTexture(GL_TEXTURE_2D, g_txMainWindow );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, g_pScreenBuffer );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	mainframe().SetCalculateFunc( Process );
	mainframe().SetDisplayFunc( DrawFrame );

    cpu_initialize(nullptr, nullptr);
	ppu_initialize();
	apu_initialize();
}

void Stop()
{
    g_bCpuRunning = false;
}

void Start()
{
    memory_intialize();
	cpu_reset();
	g_bCpuRunning = true;
}

void Resume()
{
    g_bCpuRunning = true;
}

void CPUProcess()
{
	uint sn = g_PPURegisters.scanline;
	while (sn == g_PPURegisters.scanline) {
		memory_pc_process(); //should be cycle accurate
    }
}
