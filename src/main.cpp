
#include <gfx/platform.h>

#include <common/FileLoader.h>
#include <common/global.h>

#include <sound/apu.h>

#include "processor.h"

//extern mapper_t* g_mapper;

int main(int argc, char* argv[])
{
    bool loaded = false;
	if (argc == 2) {
        printf("loading rom %s\n", argv[1]);
		loaded = CFileLoader::LoadRom(argv[1]);
	} else {
        //loaded = CFileLoader::LoadRom("/home/kevin/source/mnes/bin/roms/smb1.nes");
        loaded = CFileLoader::LoadRom("/home/kevin/source/mnes/bin/roms/C1.nes");
        //loaded = CFileLoader::LoadRom("/home/kevin/source/mnes/data/roms/Ark.nes");
    }

    InitializeProcessor();

    if (loaded)
    {
        Start();
    }

    CGfxManager::mainLoop();

    VBufferCollection().ResetContent();
	VLogCollection().ResetContent();
	
	apu_destroy();

    return 0;
}
