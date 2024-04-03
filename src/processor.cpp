#include "processor.h"


#include <common/control_flags.h>
#include <common/global.h>

#include <cpu/cpu.h>
#include <cpu/cpu_opcodes.h>
#include <cpu/cpu_registers.h>
#include <cpu/memory.h>
#include <cpu/memory_registers.h>

#include <gfx/application.h>

#include <ppu/ppu.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

#include <sound/apu.h>

#include <gfx/MainLayer.h>

#include <cstring>

#define FIRST 0

using namespace mnes;

vuchar g_pPatternTableBuffer(0x30000);

void CPUProcess(int64_t cyclesToExecute);

void Process(int64_t cyclesToExecute)
{
  if (!g_bCpuRunning) { return; }

  CPUProcess(cyclesToExecute);
}

void Stop() { g_bCpuRunning = false; }

void Start()
{
  printf("init memory\n");
  g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = false;
  memory::intialize();
  g_bCpuRunning = true;
}

void Resume() { g_bCpuRunning = true; }

void CPUProcess(int64_t cyclesToExecute)
{
  // we want to execute #cyclesToExecute, but realistically we will overflow by a certain amount
  cpu::cycle_reset(cyclesToExecute);

  while (cpu::get_cycle() > 0) {
    memory::pc_process();// should be cycle accurate

    if (g_bDisplayReady) {
      g_bDisplayReady = g_bPatternTableReady = g_bNameTableReady = false;
      // at the end of the frame lets just not care ? maybe we never care?
      break;// so we can write the texture
    }
  }
}
