#include "memory.h"
#include "cpu_opcodes.h"
#include "cpu_registers.h"
#include "memory_registers.h"

#include <common/Log.h>

#include <ppu/ppu.h>
#include <ppu/ppu_memory.h>
#include <ppu/ppu_registers.h>

#include "cpu.h"

#include <sound/apu.h>

//internal namespace not exposed to the world
namespace {
  namespace mnes_::cpu {
    enum class delayed_i {
      empty = -1,
      no = 0,
      yes = 1,
    };

    enum class doing_irq { none = 0, nmi, irq, brk };

    struct Registers
    {
      uint32_t a{ 0U }, y{ 0U }, x{ 0U };
      uint32_t opCode{ 0U };
      uint32_t pc{ 0U };
      uint32_t byteLatch{ 0U };
      uint32_t addressLatch{ 0U };
      uint32_t status{ 0U };
      uint32_t stack{ 0U };
      bool nmi{ false };
      bool prev_nmi{ false };
      uint32_t irq{ 0U };
      delayed_i delayed{ delayed_i::empty };
      doing_irq actual_irq{ doing_irq::none };
      uint64_t tick_count{ 0ULL };
    };

    constexpr uint32_t CARRY_FLAG_MASK = 0x01;
    constexpr uint32_t ZERO_FLAG_MASK = 0x02;
    constexpr uint32_t IRQ_DISABLE_FLAG_MASK = 0x04;
    constexpr uint32_t DECIMAL_FLAG_MASK = 0x08;
    constexpr uint32_t BFLAG_01_MASK = 0x10;
    constexpr uint32_t BFLAG_10_MASK = 0x20;
    constexpr uint32_t OVERFLOW_FLAG_MASK = 0x40;
    constexpr uint32_t NEGATIVE_FLAG_MASK = 0x80;

    // Used in PLP and RTI because these bits are ignored
    constexpr uint32_t BFLAG_CLEAR_MASK = 0xCF;


    namespace irq {
    // irq flags
      constexpr uint32_t dpcm = 0x01U;
      constexpr uint32_t apu_frame = 0x02U;
      constexpr uint32_t mapper1 = 0x04U;
      constexpr uint32_t mapper2 = 0x08U;
      constexpr uint32_t fds = 0x10U;
    }// namespace irq

    // imp_opcodes, could be another layer of namespace and pulled into mnes::cpu
    void clc();
    void cld();
    void cli();
    void clv();
    void sec();
    void sed();
    void sei();
    void tax();
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();
    void inx();
    void iny();
    void dex();
    void dey();
    // r_opcodes
    void lda();
    void ldx();
    void ldy();
    void eor();
    void and_();
    void ora();
    void adc();
    void sbc();
    void cmp();
    void cpx();
    void cpy();
    void bit();
    void lax();
    void nop();
    void arr();
    void axs();
    void alr();
    void xaa();
    void oal();
    void lae();
    void shs();
    void nop();
    // rel_opcodes
    bool bcc();
    bool bcs();
    bool beq();
    bool bmi();
    bool bne();
    bool bpl();
    bool bvc();
    bool bvs();
    // rwm_opcodes
    namespace internals {
      void asl(uint32_t &value);
      void lsr(uint32_t &value);
      void rol(uint32_t &value);
      void ror(uint32_t &value);
    }// namespace internals
    void asl();
    void lsr();
    void rol();
    void ror();
    void inc();
    void dec();
    void slo();
    void lse();
    void rla();
    void rra();
    void isc();
    void dcp();
    // stack_opcodes
    void brk();
    void rti();
    void rts();
    void pha();
    void php();
    void pla();
    void plp();
    void jsr();
    // w_opcodes
    void sta();
    void stx();
    void sty();
    void sax();

    // helpers
    void check_for_hardware_irq();
    void check_for_software_irq();
    void check_for_delayed_i_flag();
  }
  namespace mnes_::memory {
    struct MemoryRegisters
    {
      uint32_t r2000{ 0U };
      uint32_t r2001{ 0U };
      uint32_t r2002{ 0U };
      uint32_t r2003{ 0U };
      uint32_t r2006{ 0U };
      bool r2006Latch{ false };
      uint32_t r2006Temp{ 0U };
      uint32_t r2006ByteLatch{ 0U };
      uint32_t TileXOffset{ 0U };
      bool oam_clear_reads{ false };
      uint32_t ppu_addr_bus{ 0U };
      uint32_t ppu_latch_byte{ 0U };
    };

    void inc_stack();
    void dec_stack();
    void push_byte(uint32_t nValue);
    void push_pc();
    uint32_t pc_peek(uint32_t address);
    uint32_t pop_byte();
    void pop_pc();
    void read_opcode();
    // imm_addressing
    void immediate();
    // imp_addressing
    void implied_or_accumulator();
    // r_addressing
    void r_immediate();
    void r_absolute();
    void r_zero_page();
    void r_zero_page_indexed(const uint32_t &indexRegister);
    void r_zero_page_indexed_x();
    void r_zero_page_indexed_y();
    void r_absolute_indexed(const uint32_t &indexRegister);
    void r_absolute_indexed_x();
    void r_absolute_indexed_y();
    void r_indexed_indirect();
    void r_indirect_indexed();
    // rel_addressing
    void rel();
    // rwm_addressing
    void rmw_absolute_indexed(const uint32_t &indexRegister);
    void rmw_absolute_indexed_x();
    void rmw_absolute_indexed_y();
    void rwm_absolute();
    void rwm_zero_page();
    void rwm_zero_page_indexed_x();
    void rwm_indexed_indirect();
    void rwm_indirect_indexed();
    // w_addressing
    void w_absolute();
    void w_zero_page();
    void w_zero_page_indexed(const uint32_t &indexRegister);
    void w_zero_page_indexed_x();
    void w_zero_page_indexed_y();
    void w_absolute_indexed(const uint32_t &indexRegister);
    void w_absolute_indexed_x();
    void w_absolute_indexed_y();
    void w_indexed_indirect();
    void w_indirect_indexed();
    // jump_addressing
    void jmp_absolute();
    void jmp_absolute_indirect();

    template<typename T> uint32_t TO_ZERO_PAGE(T address);
  }
}

using namespace mnes;
using namespace mnes::cpu;
using namespace mnes::memory;
using namespace mnes_::cpu;
using namespace mnes_::memory;

template<typename T> uint32_t mnes_::memory::TO_ZERO_PAGE(T address) { return address & 0xFF; }

// clang-format off
// cpu registers
#include "cpu_registers.cpp"
#include "memory_registers.cpp"
// opcodes
#include "imp_opcodes.cpp"
#include "rwm_opcodes.cpp"
#include "r_opcodes.cpp"
#include "w_opcodes.cpp"
#include "stack_opcodes.cpp"
#include "rel_opcodes.cpp"

#include "imp_addressing.cpp"
#include "imm_addressing.cpp"
#include "r_addressing.cpp"
#include "w_addressing.cpp"
#include "jump_addressing.cpp"
#include "rel_addressing.cpp"
#include "rwm_addressing.cpp"
// clang-format on

namespace {
uint32_t mnes_::memory::pc_peek(uint32_t address) { return mappers::current()->read_memory(address); }


struct joystick_shift_reg_t
{
  uint32_t reg = 0;
  uint32_t shifted = 0;

  void set(uint32_t value)
  {
    reg = value;
    shifted = 8;
  }

  uint32_t fetch()
  {
    if (shifted == 0) { return 1; }
    return reg & 1;
  }

  void shift()
  {
    reg &= 0xFE;
    reg >>= 1;
    if (shifted > 0) { shifted--; }
  }
};


struct memory_data_t
{
  joystick_shift_reg_t joy1;
  joystick_shift_reg_t joy2;
  std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(MEMORY_SIZE);

  void initialize()
  {
    memset(data.get(), 0, MEMORY_SIZE);
  }

  memory_t Memory() { return memory_t(data.get(), MEMORY_SIZE); }
};

memory_data_t &singleton()
{
  static memory_data_t instance;
  return instance;
}

}// namespace

main_memory_t mnes::memory::Main() { return main_memory_t(singleton().Memory().subspan(MAIN_OFFSET, MAIN_SIZE)); }

sram_memory_t mnes::memory::SRam() { return sram_memory_t(singleton().Memory().subspan(SRAM_OFFSET, SRAM_SIZE)); }

spr_ram_t mnes::memory::SPRRam() { return spr_ram_t(singleton().Memory().subspan(SPR_RAM_OFFSET, SPR_RAM_SIZE)); }

tmp_spr_ram_t mnes::memory::TmpSPRRam()
{
  return tmp_spr_ram_t(singleton().Memory().subspan(TMP_SPR_RAM_OFFSET, TMP_SPR_RAM_SIZE));
}

void mnes::memory::intialize()
{
  memreg = MemoryRegisters{};
  cpureg.tick_count = 0;
  singleton().initialize();
  log::main()->info("Init memory finished.");
}

uint32_t mnes::memory::read(uint32_t address)
{
  uint32_t nMapperAnswer = mappers::current()->read_memory(address);

  if (address < 0x2000) { return Main()[address & 0x7FF]; }

  uint32_t nRetval = 0;

  if (address < 0x4000) {
    address &= 7;
    switch (address) {
    case 0:// W
      // nRetval = memreg.r2000;
      nRetval = memreg.ppu_latch_byte;
      break;
    case 1:// W
      // nRetval =  memreg.r2001;
      nRetval = memreg.ppu_latch_byte;
      break;
    case 2:// R
      nRetval = memreg.r2002;
      /*
              Reading $2002 within a few PPU clocks of when VBL is set results in special-case behavior.
              Reading one PPU clock before reads it as clear and never sets the flag or generates NMI for that frame.
              Reading on the same PPU clock or one later reads it as set, clears it, and suppresses the NMI for that
         frame. Reading two or more PPU clocks before/after it's set behaves normally (reads flag's value, clears it,
         and doesn't affect NMI operation). This suppression behavior is due to the $2002 read pulling the NMI line back
         up too quickly after it drops (NMI is active low) for the CPU to see it. (CPU inputs like NMI are sampled each
         clock.)
      */
      ppu::set_last_r2002_read(ppu::get_cycle());
      clear_vblank();
      memreg.r2006Latch = false;
      break;
    case 3:// W
      nRetval = memreg.ppu_latch_byte;
      break;
    case 4:// RW
      if (memreg.oam_clear_reads) {
        nRetval = 0xFF;
      } else {
        nRetval = SPRRam()[memreg.r2003];
      }
      break;
    case 5:// W
      nRetval = memreg.ppu_latch_byte;
      break;
    case 6:// W
      nRetval = memreg.ppu_latch_byte;
      break;
    case 7:// RW
      // read from ppu
      MLOG("PPU Cycle {}", ppu::get_current_scanline_cycle());
      uint32_t ppuaddress = memreg.r2006;
      memreg.r2006 = (memreg.r2006 + ((memreg.r2000 & 0x4) ? 32 : 1)) & 0x7FFF;
      memreg.ppu_addr_bus = memreg.r2006;
      if (ppuaddress >= 0x3F00) {
        // returns value right away, but also sets the latch too
        memreg.r2006ByteLatch = ppu::memory::read(ppuaddress & 0x2FFF);
        nRetval = ppu::memory::read(ppuaddress);
      } else {
        nRetval = memreg.r2006ByteLatch;
        memreg.r2006ByteLatch = ppu::memory::read(ppuaddress);
      }
      break;
    }

    return nRetval;
  }

  if (address < 0x6000) {
    const uint32_t original = address;
    uint32_t open_bus = original >> 8;
    address &= 0x1F;

    if (original == 0x4015) { return apu::read(original); }
    // joypad
    switch (address) {
    case 0x14:// w
    case 0x15:// w
      // non readable
      nRetval = open_bus;// open bus !!
      break;
    case 0x16:// rw - joystick #0 - hardcode to say its connected but no button pressed.
      nRetval = (open_bus & 0xE0) | singleton().joy1.fetch();
      singleton().joy1.shift();
      break;
    case 0x17:// rw - joystick #1 - hardcode to say its not connected
      nRetval = (open_bus & 0xE0) | singleton().joy2.fetch();
      singleton().joy2.shift();
      break;
    default:
      nRetval = open_bus;
      break;
    }

    return nRetval;
  }

  // Sram is handled in mapper i guess...GL!

  return nMapperAnswer;
}

void mnes::memory::write(uint32_t address, uint32_t value)
{
  mappers::current()->write_memory(address, value);
  // g_pCurrentMapper->WriteMemory(address, value);

  if (address < 0x2000) {
    Main()[address & 0x7FF] = TO_BYTE(value);
    return;
  }
  if (address < 0x4000) {

    address &= 0x7;
    switch (address) {
    case 0:// W
    {
      bool bNmiEnabled = (memreg.r2000 & 0x80) == 0x80;
      memreg.r2000 = value;
      memreg.r2006Temp &= 0xF3FF;
      memreg.r2006Temp |= (value & 0x3) << 10;
      if (bNmiEnabled == false && (memreg.r2000 & 0x80) == 0x80 && is_vblank()) {
        log::main()->info("nmi set to true");
        cpureg.nmi = true;
      }
      memreg.ppu_latch_byte = value;
    } break;
    case 1:// W
      memreg.r2001 = value;
      memreg.ppu_latch_byte = value;
      break;
    case 2:// R
      memreg.ppu_latch_byte = value;
      break;
    case 3:// W
      memreg.ppu_latch_byte = memreg.r2003 = value;
      break;
    case 4:// RW
      SPRRam()[memreg.r2003++] = TO_BYTE(value);
      memreg.r2003 &= 0xFF;
      break;
    case 5:// W
    {
      if (memreg.r2006Latch) {// second write
        memreg.r2006Temp &= 0x8C1F;
        memreg.r2006Temp |= (value << 2) & 0x3E0;
        memreg.r2006Temp |= (value << 12) & 0x7000;
      } else {// first write
        memreg.TileXOffset = value & 0x7;
        memreg.r2006Temp &= 0xFFE0;// erase bits D0-D4
        memreg.r2006Temp |= (value >> 3) & 0x1F;// copy bit D3-D7 to D0-D4
      }
      memreg.r2006Latch = !memreg.r2006Latch;
      memreg.ppu_latch_byte = value;
      break;
    }
    case 6:// W
    {
      if (memreg.r2006Latch) {// second write
        memreg.r2006Temp &= 0x3F00;// clear bottom, according to docs it says top 2 bits are cleared.
        // in essense its masking 2006 to 0x3FFF, which is good because if you look inside write in 2007 it masks
        // anyways to 0x3FFF; side effect is Y-scroll gets 1 bit chopped off. which is prolly why they use 2005 to
        // scroll, since it is the scrolling register.
        memreg.r2006Temp |= value;// set bottom
        memreg.r2006 = memreg.r2006Temp;
        if (memreg.r2006 < 0x3F00) {
          MLOG("PPU cycle {} old ${:04X}", ppu::get_current_scanline_cycle(), memreg.ppu_addr_bus);
          memreg.ppu_addr_bus = memreg.r2006;
        }
      } else {// first write
        memreg.r2006Temp &= 0xFF;// clear top
        memreg.r2006Temp |= (value & 0x3F) << 8;// set top
      }
      memreg.r2006Latch = !memreg.r2006Latch;
      memreg.ppu_latch_byte = value;
      break;
    }
    case 7:// RW
    {
      uint32_t ppuAddress = memreg.r2006;
      memreg.r2006 = (memreg.r2006 + ((memreg.r2000 & 0x4) ? 32 : 1)) & 0x7FFF;
      if (memreg.r2006 < 0x3F00) {
        MLOG("PPU cycle {} old ${:04X}", ppu::get_current_scanline_cycle(), memreg.ppu_addr_bus);
        memreg.ppu_addr_bus = memreg.r2006;
      }
      ppu::memory::write(ppuAddress, value);
      break;
    }
    }
    return;
  }

  if (address < 0x6000) {
    address &= 0x1F;

    switch (address) {
    // Pulse 1
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    // Pulse 2
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    // Triangle
    case 0x08:
    case 0x0A:
    case 0x0B:
    // Noise
    case 0x0C:
    case 0x0E:
    case 0x0F:
    // DMC Channel
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    // Control
    case 0x15:
    // Frame Counter
    case 0x17:
      apu::write(address, value);
      break;
    case 0x14:// w
    {
      uint32_t nStartAddress = 0x100 * value;
      // keep the cpu and ppu in sync while dma is happening
      if (ppu::is_odd_cycle()) {
        do_cycle();
        do_cycle();
      } else {
        do_cycle();
      }
      for (uint32_t i = 0; i < 256; i++) { ext::write(0x2004, ext::read(nStartAddress + i)); }

      return;
    }
    case 0x16:// rw
    {
      static int strobe = -1;
      if ((value & 1) == 1) { strobe = 1; }
      if ((value & 1) == 0 && strobe == 1) {
        singleton().joy1.set(get_joy1());
        singleton().joy2.set(get_joy2());
        strobe = -1;
      }
    } break;
    default:
      // unknown...?
      break;
    }

    return;
  }

  if (address < 0x8000 && mappers::current()->m_bSaveRam) {// SRAM man..
    SRam()[address & 0x1FFF] = TO_BYTE(value);
    return;
  }
}

uint32_t mnes::memory::ext::read(uint32_t address)
{
  do_cycle();
  return memory::read(address);
}

void mnes::memory::ext::write(uint32_t address, uint32_t value)
{
  do_cycle();
  memory::write(address, value);
}

namespace {

void mnes_::memory::inc_stack()
{
  cpureg.stack++;
  cpureg.stack &= 0xFF;
}

void mnes_::memory::dec_stack()
{
  cpureg.stack--;
  cpureg.stack &= 0xFF;
}

void mnes_::memory::push_byte(uint32_t nValue)
{
  ext::write(cpureg.stack + 0x100, nValue);
  dec_stack();
}

uint32_t mnes_::memory::pop_byte()
{
  inc_stack();
  uint32_t retval = ext::read(cpureg.stack + 0x100);
  return retval;
}

void mnes_::memory::push_pc()
{
  ext::write(cpureg.stack + 0x100, (cpureg.pc >> 8));
  dec_stack();
  ext::write(cpureg.stack + 0x100, cpureg.pc & 0xFF);
  dec_stack();
}

void mnes_::memory::pop_pc()
{
  inc_stack();
  cpureg.pc = ext::read(cpureg.stack + 0x100);
  inc_stack();
  cpureg.pc |= ext::read(cpureg.stack + 0x100) << 8;
}
/// <summary>
/// Only is done right after opcode is officially read
/// </summary>
void mnes_::cpu::check_for_hardware_irq()
{
  if (cpureg.prev_nmi) {
    cpureg.opCode = 0x00;
    cpureg.actual_irq = doing_irq::nmi;
    MLOG("**NMI**");
  } else if (IF_INTERRUPT() == false) {
    if (cpureg.irq) {
      cpureg.opCode = 0x00;
      cpureg.actual_irq = doing_irq::irq;
      MLOG("**IRQ**");
    } else {
      cpureg.actual_irq = doing_irq::none;
      cpureg.pc++;
    }
  } else {
    cpureg.actual_irq = doing_irq::none;
    cpureg.pc++;
  }
}

void mnes_::cpu::check_for_software_irq()
{
  if (cpureg.actual_irq == doing_irq::none) {
    if (cpureg.opCode == mnes::opcodes::OPCODE_BRK) { cpureg.actual_irq = doing_irq::brk; }
  }
}

void mnes_::cpu::check_for_delayed_i_flag()
{
  if (cpureg.delayed != delayed_i::empty) {
    SET_INTERRUPT(cpureg.delayed == delayed_i::yes);
    cpureg.delayed = delayed_i::empty;
  }
}

/// <summary>
/// Does a read, using 1 cycle
/// </summary>
void mnes_::memory::read_opcode()
{
  cpureg.opCode = ext::read(cpureg.pc);

  check_for_hardware_irq();
  check_for_software_irq();
  check_for_delayed_i_flag();

  MLOG("${:04X} {:02X} {}", cpureg.pc, cpureg.opCode, opcodes::opcode_to_string(cpureg.opCode));
}
}

void mnes::memory::pc_process()
{
  read_opcode();
  switch (cpureg.opCode) {
    using namespace mnes::opcodes;
    // Read Instructions - Immediate
  case OPCODE_ADC_OP:
  case OPCODE_AND_OP:
  case OPCODE_CMP_OP:
  case OPCODE_CPX_OP:
  case OPCODE_CPY_OP:
  case OPCODE_EOR_OP:
  case OPCODE_LDA_OP:
  case OPCODE_LDX_OP:
  case OPCODE_LDY_OP:
  case OPCODE_ORA_OP:
  case OPCODE_SBC_OP:
  case OPCODE_SBC_IMM_EB:
  case OPCODE_ARR_IMM:
  case OPCODE_NOP_IMM:
  case OPCODE_NOP_IMM_82:
  case OPCODE_NOP_IMM_89:
  case OPCODE_NOP_IMM_C2:
  case OPCODE_NOP_IMM_E2:
  case OPCODE_ANC_IMM_0B:
  case OPCODE_ANC_IMM_2B:
  case OPCODE_ALR_IMM:
  case OPCODE_LAX_IMM:
  case OPCODE_AXS_IMM:
  case OPCODE_XAA_IMM:
    r_immediate();
    break;
    // Read Instructions - Absolute
  case OPCODE_LDA_AB:
  case OPCODE_LDX_AB:
  case OPCODE_LDY_AB:
  case OPCODE_EOR_AB:
  case OPCODE_AND_AB:
  case OPCODE_ORA_AB:
  case OPCODE_ADC_AB:
  case OPCODE_SBC_AB:
  case OPCODE_CMP_AB:
  case OPCODE_BIT_AB:
  case OPCODE_CPX_AB:
  case OPCODE_CPY_AB:
  case OPCODE_LAX_AB:
    r_absolute();
    break;
    // Read Instructions - Zero Page
  case OPCODE_LDA_ZP:
  case OPCODE_LDX_ZP:
  case OPCODE_LDY_ZP:
  case OPCODE_EOR_ZP:
  case OPCODE_AND_ZP:
  case OPCODE_ORA_ZP:
  case OPCODE_ADC_ZP:
  case OPCODE_SBC_ZP:
  case OPCODE_CMP_ZP:
  case OPCODE_BIT_ZP:
  case OPCODE_LAX_ZP:
  case OPCODE_CPX_ZP:
  case OPCODE_CPY_ZP:
  case OPCODE_NOP_04_ZP:
  case OPCODE_NOP_44_ZP:
  case OPCODE_NOP_64_ZP:
    r_zero_page();
    break;
    // Read Instructions - Zero Page Indexed
  case OPCODE_LDA_ZP_X:
  case OPCODE_LDY_ZP_X:
  case OPCODE_EOR_ZP_X:
  case OPCODE_AND_ZP_X:
  case OPCODE_ORA_ZP_X:
  case OPCODE_ADC_ZP_X:
  case OPCODE_SBC_ZP_X:
  case OPCODE_CMP_ZP_X:
  case OPCODE_NOP_14_ZP_X:
  case OPCODE_NOP_34_ZP_X:
  case OPCODE_NOP_54_ZP_X:
  case OPCODE_NOP_74_ZP_X:
  case OPCODE_NOP_D4_ZP_X:
  case OPCODE_NOP_F4_ZP_X:
    r_zero_page_indexed_x();
    break;
  case OPCODE_LDX_ZP_Y:
  case OPCODE_LAX_ZP_Y:
    r_zero_page_indexed_y();
    break;
    // Read Instructions - Absolute Indexed
  case OPCODE_LDA_AB_X:
  case OPCODE_LDY_AB_X:
  case OPCODE_EOR_AB_X:
  case OPCODE_AND_AB_X:
  case OPCODE_ORA_AB_X:
  case OPCODE_ADC_AB_X:
  case OPCODE_SBC_AB_X:
  case OPCODE_CMP_AB_X:
  case OPCODE_NOP_1C_AB_X:
  case OPCODE_NOP_3C_AB_X:
  case OPCODE_NOP_5C_AB_X:
  case OPCODE_NOP_7C_AB_X:
  case OPCODE_NOP_DC_AB_X:
  case OPCODE_NOP_FC_AB_X:
    r_absolute_indexed_x();
    break;
  case OPCODE_LDA_AB_Y:
  case OPCODE_LDX_AB_Y:
  case OPCODE_EOR_AB_Y:
  case OPCODE_AND_AB_Y:
  case OPCODE_ORA_AB_Y:
  case OPCODE_ADC_AB_Y:
  case OPCODE_SBC_AB_Y:
  case OPCODE_CMP_AB_Y:
  case OPCODE_LAX_AB_Y:
    r_absolute_indexed_y();
    break;
    // Read Instructions - Indexed Indirect
  case OPCODE_LDA_IN_X:
  case OPCODE_ORA_IN_X:
  case OPCODE_EOR_IN_X:
  case OPCODE_AND_IN_X:
  case OPCODE_ADC_IN_X:
  case OPCODE_CMP_IN_X:
  case OPCODE_SBC_IN_X:
  case OPCODE_LAX_IN_X:
    r_indexed_indirect();
    break;
    // Read Instructions - Indirect Indexed
  case OPCODE_LDA_IN_Y:
  case OPCODE_EOR_IN_Y:
  case OPCODE_AND_IN_Y:
  case OPCODE_ORA_IN_Y:
  case OPCODE_ADC_IN_Y:
  case OPCODE_SBC_IN_Y:
  case OPCODE_CMP_IN_Y:
  case OPCODE_LAX_IN_Y:
    r_indirect_indexed();
    break;
    // Read-Modify-Write instructions - Absolute
  case OPCODE_ASL_AB:
  case OPCODE_LSR_AB:
  case OPCODE_ROL_AB:
  case OPCODE_ROR_AB:
  case OPCODE_INC_AB:
  case OPCODE_DEC_AB:
  case OPCODE_SLO_AB:
  case OPCODE_LSE_AB:
  case OPCODE_RLA_AB:
  case OPCODE_RRA_AB:
  case OPCODE_ISC_AB:
  case OPCODE_DCP_AB:
    rwm_absolute();
    break;
    // Read-Modify-Write instructions - Zero Page
  case OPCODE_ASL_ZP:
  case OPCODE_LSR_ZP:
  case OPCODE_ROL_ZP:
  case OPCODE_ROR_ZP:
  case OPCODE_INC_ZP:
  case OPCODE_DEC_ZP:
  case OPCODE_SLO_ZP:
  case OPCODE_LSE_ZP:
  case OPCODE_RLA_ZP:
  case OPCODE_RRA_ZP:
  case OPCODE_ISC_ZP:
  case OPCODE_DCP_ZP:
    rwm_zero_page();
    break;
    // Read-Modify-Write instructions - Zero Page Indexed
  case OPCODE_ASL_ZP_X:
  case OPCODE_LSR_ZP_X:
  case OPCODE_ROL_ZP_X:
  case OPCODE_ROR_ZP_X:
  case OPCODE_INC_ZP_X:
  case OPCODE_DEC_ZP_X:
  case OPCODE_SLO_ZP_X:
  case OPCODE_LSE_ZP_X:
  case OPCODE_RLA_ZP_X:
  case OPCODE_RRA_ZP_X:
  case OPCODE_ISC_ZP_X:
  case OPCODE_DCP_ZP_X:
    rwm_zero_page_indexed_x();
    break;
    // Read-Modify-Write instructions - Absolute Indexed
  case OPCODE_ASL_AB_X:
  case OPCODE_LSR_AB_X:
  case OPCODE_ROL_AB_X:
  case OPCODE_ROR_AB_X:
  case OPCODE_INC_AB_X:
  case OPCODE_DEC_AB_X:
  case OPCODE_SLO_AB_X:
  case OPCODE_LSE_AB_X:
  case OPCODE_RLA_AB_X:
  case OPCODE_RRA_AB_X:
  case OPCODE_ISC_AB_X:
  case OPCODE_DCP_AB_X:
    rmw_absolute_indexed_x();
    break;
  case OPCODE_SLO_AB_Y:
  case OPCODE_LSE_AB_Y:
  case OPCODE_RLA_AB_Y:
  case OPCODE_RRA_AB_Y:
  case OPCODE_ISC_AB_Y:
  case OPCODE_DCP_AB_Y:
  case OPCODE_TAS_AB_Y:
  case OPCODE_AHX_AB_Y:
  case OPCODE_LAS_AB_Y:
    rmw_absolute_indexed_y();
    break;
    // Read-Modify-Write instructions - Indexed indirect
  case OPCODE_SLO_IN_X:
  case OPCODE_LSE_IN_X:
  case OPCODE_RLA_IN_X:
  case OPCODE_RRA_IN_X:
  case OPCODE_ISC_IN_X:
  case OPCODE_DCP_IN_X:
    rwm_indexed_indirect();
    break;
    // Read-Modify-Write instructions - Indirect Indexed
  case OPCODE_SLO_IN_Y:
  case OPCODE_LSE_IN_Y:
  case OPCODE_RLA_IN_Y:
  case OPCODE_RRA_IN_Y:
  case OPCODE_ISC_IN_Y:
  case OPCODE_DCP_IN_Y:
  case OPCODE_AHX_IN_Y:
    rwm_indirect_indexed();
    break;
    // Write instructions - Absolute
  case OPCODE_STA_AB:
  case OPCODE_STX_AB:
  case OPCODE_STY_AB:
    // case OPCODE_SAX_AB:
    w_absolute();
    break;
    // Write instructions - Zero Page
  case OPCODE_STA_ZP:
  case OPCODE_STX_ZP:
  case OPCODE_STY_ZP:
  case OPCODE_SAX_ZP:
    w_zero_page();
    break;
    // Write instructions - Zero Page Indexed
  case OPCODE_STA_ZP_X:
  case OPCODE_STY_ZP_X:
    w_zero_page_indexed_x();
    break;
  case OPCODE_STX_ZP_Y:
  case OPCODE_SAX_ZP_Y:
    w_zero_page_indexed_y();
    break;
    // Write instructions - Absolute Indexed
  case OPCODE_STA_AB_X:
  // case OPCODE_STX_AB_X:
  // case OPCODE_STY_AB_X:
  // case OPCODE_SHA_AB_X:
  // case OPCODE_SHX_AB_X:
  case OPCODE_SHY_AB_X:
    w_absolute_indexed_x();
    break;
  case OPCODE_STA_AB_Y:
  // case OPCODE_STX_AB_Y:
  // case OPCODE_STY_AB_Y:
  // case OPCODE_SHA_AB_Y:
  case OPCODE_SHX_AB_Y:
    // case OPCODE_SHY_AB_Y:
    w_absolute_indexed_y();
    break;
    // Write instructions - Indexed indirect
  case OPCODE_STA_IN_X:
    // case OPCODE_SAX_IN_X:
    w_indexed_indirect();
    break;
    // Write instructions - Indirect Indexed
  case OPCODE_STA_IN_Y:
    // case OPCODE_SHA_IN_Y:
    w_indirect_indexed();
    break;
    // Stack instructions
  case OPCODE_BRK:
    brk();
    break;
  case OPCODE_RTI:
    rti();
    break;
  case OPCODE_RTS:
    rts();
    break;
  case OPCODE_PHA:
    MLOG("A:${:02X} ->", cpureg.a);
    pha();
    break;
  case OPCODE_PHP:
    php();
    break;
  case OPCODE_PLA:
    pla();
    MLOG("A:${:02X} ->", cpureg.a);
    break;
  case OPCODE_PLP:
    plp();
    break;
  case OPCODE_JSR_AB:
    jsr();
    break;
    // Branch Instructions
  case OPCODE_BCC:
  case OPCODE_BCS:
  case OPCODE_BNE:
  case OPCODE_BEQ:
  case OPCODE_BPL:
  case OPCODE_BMI:
  case OPCODE_BVC:
  case OPCODE_BVS:
    rel();
    break;
    // Jumps
  case OPCODE_JMP_AB:
    jmp_absolute();
    break;
  case OPCODE_JMP_IN:
    jmp_absolute_indirect();
    break;
    // Implied or Accumulator
  case OPCODE_CLC:
  case OPCODE_CLD:
  case OPCODE_CLI:
  case OPCODE_CLV:
  case OPCODE_DEX:
  case OPCODE_DEY:
  case OPCODE_INX:
  case OPCODE_INY:
  case OPCODE_NOP:
  case OPCODE_SEC:
  case OPCODE_SED:
  case OPCODE_SEI:
  case OPCODE_TAX:
  case OPCODE_TAY:
  case OPCODE_TSX:
  case OPCODE_TXA:
  case OPCODE_TXS:
  case OPCODE_TYA:
  case OPCODE_ASL_A:
  case OPCODE_LSR_A:
  case OPCODE_ROR_A:
  case OPCODE_ROL_A:
  case OPCODE_NOP_1A:
  case OPCODE_NOP_3A:
  case OPCODE_NOP_5A:
  case OPCODE_NOP_7A:
  case OPCODE_NOP_DA:
  case OPCODE_NOP_FA:
    implied_or_accumulator();
    break;
  default:
    MLOG("Unknown opcode found ${:02X}", cpureg.opCode);
    break;
  }

  MLOG("T:[${:016X}]", cpureg.tick_count);
  //TODO: replace with single line that logs everything about the current opcode.

  cpureg.tick_count++;
}
