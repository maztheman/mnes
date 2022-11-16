#include "ppu_memory.h"

#include <cpu/memory_registers.h>

#include <span>
#include <array>

static constexpr auto VRAM_SIZE = 64 + 0x800;

struct ppu_memory_t
{
	std::array<uint8_t*, 8> PPUTable = {
		nullptr, nullptr, nullptr, nullptr, 
		nullptr, nullptr, nullptr, nullptr
	};
	std::array<uint8_t*, 4> Tables = {
		nullptr, nullptr, nullptr, nullptr
	};
	std::unique_ptr<uint8_t[]> VRAM = std::make_unique<uint8_t[]>(VRAM_SIZE);

	std::span<uint8_t> VRAMSPAN()
	{
		return std::span<uint8_t>(VRAM.get(), VRAM_SIZE);
	}

	std::span<uint8_t> Palette()
	{
		return VRAMSPAN().subspan(0, 64);
	}

	std::span<uint8_t> NTRam()
	{
		return VRAMSPAN().subspan(64, 0x800);
	}
};

ppu_memory_t& ppu_memory()
{
	static ppu_memory_t instance;
	return instance;
}

std::array<uint8_t*, 8>& PPUTable()
{
	return ppu_memory().PPUTable;
}

std::array<uint8_t*, 4>& Tables()
{
	return ppu_memory().Tables;
}

std::span<uint8_t> Palette()
{
	return ppu_memory().Palette();
}

std::span<uint8_t> NTRam()
{
	return ppu_memory().NTRam();
}

static auto& ppuTable = PPUTable();
static auto& tables = Tables();

uint ppu_memory_main_read(uint address)
{
	//g_MemoryRegisters.ppu_addr_bus = address;

	address &= 0x3FFF;

	current_mapper()->read_ppu_memory(address);//allow mapper to read shit man

	//Pattern Table
	if (address < 0x2000) {
		return ppuTable[address >> 0xA][address & 0x3FF];
	}

	//Name Table
	if (address < 0x3F00) {
		uint index = (address & 0xC00) >> 10;
		return tables[index][address & 0x3FF];
	}

	address &= 0x1F;
	return ppu_memory().Palette()[address];
}

void ppu_memory_main_write(uint address, uint value)
{
	//g_MemoryRegisters.ppu_addr_bus = address;

	address &= 0x3FFF;

	//Pattern Table
	if (address < 0x2000) {
		ppu_memory().PPUTable[address >> 0xAU][address & 0x3FF] = value;
		return;
	}

	//Name Table
	if (address < 0x3F00) {
		size_t index = (address&0xC00)>>10;
		ppu_memory().Tables[index][address & 0x3FF] = value;
		return;
	}

	//Palette
	uint tmp = address & 0x1F;
	uint val = value & 0x3F;

	if (auto palette = ppu_memory().Palette(); tmp <= 0x10)
	{
		tmp &= 0xF;
		palette[tmp] = val;
		palette[0xC] = palette[0x8] = palette[0x4] = palette[0x0];
		palette[0x10] = palette[0x1C] = palette[0x18] = palette[0x14] = palette[0x0];
	} else {
		palette[tmp] = val;
	}
}