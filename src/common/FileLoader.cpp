#include "FileLoader.h"

#include <cstdio>

#include "ines_format.h"
#include "File.h"

#include <mappers/mapper.h>

//mappers
vuchar		g_arRawData;
ines_format	g_ines_format;
mapper_t*	g_mapper;

CFileLoader::CFileLoader()
{
}

CFileLoader::~CFileLoader()
{
}

bool CFileLoader::LoadRom(const string& sFileName)
{
	CFile file(sFileName, "rb");

	if (!file.IsOpen()) {
		printf("could not open rom %s\n", sFileName.c_str());
		return false;
	}

	ines_format& format = g_ines_format;

	//count be aligned funny.
	file.Read(&format.reserved, 16, 1);

	if (!(format.reserved[0] == 'N' && format.reserved[1] == 'E' && format.reserved[2] == 'S' && format.file_version == 0x1A)) {
		return false;//wrong format i guess
	}

	uint nMapperNo = ((format.rom_control_1 & 0xF0) >> 4) | (format.rom_control_2 & 0xF0);

	g_mapper = get_mapper(nMapperNo);

	if (g_mapper == nullptr) {
		printf("mapper %d is not supported\n", nMapperNo);
		return false;
	}

	//bool ines20 = (format.rom_control_1 & 0xC) == 0x8;

	//prg + chr + trainer
	size_t nFileSize = (format.prg_rom_count * 16384 ) + ( format.chr_rom_count * 8192 ) + (( (format.rom_control_1 & 4) == 4) ? 512 : 0);

	printf("detected rom size of %ld\n", nFileSize);

	g_arRawData.resize(nFileSize, 0);
	file.Read(&g_arRawData[0], 1, nFileSize);

	g_mapper->reset();

	return true;
}