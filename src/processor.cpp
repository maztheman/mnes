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

#include <cstring>

#define FIRST   0

extern void cpu_reset();

vuchar g_pPatternTableBuffer(0x30000);

void CPUProcess();

static COpenGLWrapper& mainframe()
{
    static COpenGLWrapper instance(CGfxManager::getMainWindow());
    return instance;
}

void UpdateTextureFromPPU()
{
	g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = true;
	mainframe().MakeCurrent();
	UpdateMainWindowTexture();
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
	memset(&g_pScreenBuffer[0], 128, 0x30000);
	printf("InitializeProcessor\n");
    mainframe().MakeCurrent();
	printf("make current\n");
	InitializeRenderer();
	mainframe().SetCalculateFunc( Process );
	mainframe().SetDisplayFunc( DrawFrame );
	printf("InitializeProcessor gl setup\n");
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
	printf("Starting emulation\n");
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
