#include "cpu.h"
#include "cpu_opcodes.h"
#include "cpu_registers.h"
#include "memory.h"

#include <common/Log.h>
#include <gfx/input.h>
#include <ppu/ppu.h>
#include <sound/apu.h>

#include <algorithm>

#include "gfx/MainLayer.h"

#include "joystick.cpp"

static int64_t cpu_cycle = 0;


void cpu_initialize()
{
  // joy1.create(hInstance, hWnd);
}

// tick!
void cpu_do_cycle()
{
  // this is where hijacking a interrupt can happen
  GRegisters().prev_nmi = GRegisters().nmi;
  // GRegisters().prev_irq = (IF_INTERRUPT() == false) ? GRegisters().irq : 0;

  // 1 apu cycles per 2 cpu cycle
  apu_do_cycle();

  // 3 cycles for every cpu cycle
  ppu_do_cycle();
  ppu_do_cycle();
  ppu_do_cycle();

  current_mapper()->do_cpu_cycle();

  cpu_cycle--;
}

void cpu_cycle_reset(int64_t count) { cpu_cycle += count; }

void cpu_cycle_set(int64_t count) { cpu_cycle = count; }

int64_t cpu_get_cycle() { return cpu_cycle; }

void cpu_reset()
{
  GRegisters().tick_count = 0;
  GRegisters().delayed = delayed_i::empty;
  GRegisters().a = GRegisters().y = GRegisters().x = 0;
  GRegisters().irq = 0;
  GRegisters().status = IRQ_DISABLE_FLAG_MASK;
  GRegisters().stack = 0xFD;
  GRegisters().pc = (memory_main_read(RESETLO)) | (memory_main_read(RESETHI) << 8);
  apu_reset();
  ppu_reset();
  fmt::print(stderr, "cpu reset\n");
}
