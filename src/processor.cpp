#include "processor.h"


#include <common/global.h>
#include <common/control_flags.h>

#include <cpu/cpu.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <cpu/memory_registers.h>
#include <cpu/cpu_opcodes.h>

#include <gfx/application.h>

#include <ppu/ppu.h>
#include <ppu/ppu_registers.h>
#include <ppu/ppu_memory.h>

#include <sound/apu.h>

#include <cstring>

#define FIRST   0

extern void cpu_reset();

vuchar g_pPatternTableBuffer(0x30000);

void CPUProcess();

void UpdateTextureFromPPU()
{
	static Application* app = Application::getApplication();
	g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = true;
	app->updateMainTexture();
}

void Process()
{
	if (!g_bCpuRunning) 
	{
		return;
	}
	if (g_PPURegisters.scanline == FIRST) 
	{
		g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = false;
	} 
	CPUProcess();
}

void Stop()
{
    g_bCpuRunning = false;
}

void Start()
{
	printf("init memory\n");
	g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = false;
    memory_intialize();
	g_bCpuRunning = true;
}

void Resume()
{
    g_bCpuRunning = true;
}

void CPUProcess()
{
	//gotta process until the display is ready. hopefully works in all cases
	while(g_bDisplayReady == false)
	{
		memory_pc_process(); //should be cycle accurate
	}
}
