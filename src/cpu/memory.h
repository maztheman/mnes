#pragma once


namespace mnes::memory
{
    static constexpr auto MAIN_MEMORY_OFFSET = 0UL;
    static constexpr auto MAIN_MEMORY_SIZE = 0x800UL;
    using main_memory_t = fixed_span_bytes<MAIN_MEMORY_SIZE>;
    static constexpr auto SRAM_OFFSET = MAIN_MEMORY_OFFSET + MAIN_MEMORY_SIZE;
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
}


void memory_intialize();
uint32_t ext_memory_read(uint32_t address);
void ext_memory_write(uint32_t address, uint32_t value);

uint32_t memory_main_read(uint32_t address);
void memory_main_write(uint32_t address, uint32_t value);

void memory_pc_process();

mnes::memory::main_memory_t MainMemory();
mnes::memory::sram_memory_t SRam();
mnes::memory::spr_ram_t SPRRam();
mnes::memory::tmp_spr_ram_t TmpSPRRam();
