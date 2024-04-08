#include "cpu.h"
#include "cpu_opcodes.h"
#include "cpu_registers.h"
#include "memory.h"

#include <common/Log.h>
#include <ppu/ppu.h>
#include <sound/apu.h>

#include <algorithm>

#include "gfx/MainLayer.h"

using namespace mnes;
using namespace mnes::cpu;

// clang-format off
#include "joystick.cpp"
// clang-format on

namespace {
int64_t cpu_cycle = 0;
}

void mnes::cpu::initialize()
{
  // joy1.create(hInstance, hWnd);
}

// tick!
void mnes::cpu::do_cycle()
{
  // this is where hijacking a interrupt can happen
  copy_nmi_to_prev();
  // cpureg.prev_irq = (IF_INTERRUPT() == false) ? cpureg.irq : 0;

  // 1 apu cycles per 2 cpu cycle
  apu::do_cycle();

  // 3 cycles for every cpu cycle
  ppu::do_cycle();
  ppu::do_cycle();
  ppu::do_cycle();

  mappers::current()->do_cpu_cycle();

  cpu_cycle--;
}

void mnes::cpu::cycle_reset(int64_t count) { cpu_cycle += count; }

void mnes::cpu::cycle_set(int64_t count) { cpu_cycle = count; }

int64_t mnes::cpu::get_cycle() { return cpu_cycle; }

void mnes::cpu::reset()
{
  reset_registers();
  apu::reset();
  ppu::reset();
  fmt::print(stderr, "cpu reset\n");
}
