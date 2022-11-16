#include "cpu.h"
#include "cpu_opcodes.h"
#include "cpu_registers.h"
#include "memory.h"

#include <ppu/ppu.h>
#include <sound/apu.h>
#include <common/Log.h>
#include <gfx/input.h>

#include <algorithm>

#include "gfx/MainLayer.h"

#include "joystick.cpp"

static int64_t cpu_cycle = 0;


void cpu_initialize()
{
	//joy1.create(hInstance, hWnd);
}

//tick!
void cpu_do_cycle()
{
	//this is where hijacking a interrupt can happen
	g_Registers.prev_nmi = g_Registers.nmi;
	//g_Registers.prev_irq = (IF_INTERRUPT() == false) ? g_Registers.irq : 0;

	//1 apu cycles per 2 cpu cycle
	apu_do_cycle();
	
	//3 cycles for every cpu cycle
	ppu_do_cycle();
	ppu_do_cycle();
	ppu_do_cycle();

	current_mapper()->do_cpu_cycle();

	cpu_cycle--;
}

void cpu_cycle_reset(int64_t count)
{
	cpu_cycle += count;
}

void cpu_cycle_set(int64_t count)
{
	cpu_cycle = count;
}

int64_t cpu_get_cycle()
{
	return cpu_cycle;
}

void cpu_reset()
{
	g_Registers.tick_count = 0;
	g_Registers.delayed = delayed_i::empty;
	g_Registers.a = g_Registers.y = g_Registers.x = 0;
	g_Registers.irq = 0;
	g_Registers.status = IRQ_DISABLE_FLAG_MASK;
	g_Registers.stack = 0xFD;
	g_Registers.pc = (memory_main_read(RESETLO)) | (memory_main_read(RESETHI) << 8);
	apu_reset();
	ppu_reset();
	fmt::print(stderr, "cpu reset\n");
}
