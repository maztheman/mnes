#pragma once

namespace mnes::memory {

static constexpr auto MAIN_OFFSET = 0UL;
static constexpr auto MAIN_SIZE = 0x800UL;
using main_memory_t = fixed_span_bytes<MAIN_SIZE>;
static constexpr auto SRAM_OFFSET = MAIN_OFFSET + MAIN_SIZE;
static constexpr auto SRAM_SIZE = 0x2000UL;
using sram_memory_t = fixed_span_bytes<SRAM_SIZE>;
static constexpr auto SPR_RAM_OFFSET = SRAM_OFFSET + SRAM_SIZE;
static constexpr auto SPR_RAM_SIZE = 0x100UL;
using spr_ram_t = fixed_span_bytes<SPR_RAM_SIZE>;
static constexpr auto TMP_SPR_RAM_OFFSET = SPR_RAM_OFFSET + SPR_RAM_SIZE;
static constexpr auto TMP_SPR_RAM_SIZE = 32UL;
using tmp_spr_ram_t = fixed_span_bytes<TMP_SPR_RAM_SIZE>;
static constexpr auto MEMORY_SIZE = TMP_SPR_RAM_OFFSET + TMP_SPR_RAM_SIZE;
using memory_t = fixed_span_bytes<MEMORY_SIZE>;

void intialize();

namespace ext {
  uint32_t read(uint32_t address);
  void write(uint32_t address, uint32_t value);
}// namespace ext

uint32_t read(uint32_t address);
void write(uint32_t address, uint32_t value);

void pc_process();

main_memory_t Main();
sram_memory_t SRam();
spr_ram_t SPRRam();
tmp_spr_ram_t TmpSPRRam();

}// namespace mnes::memory