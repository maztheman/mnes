
#include <common/FileLoader.h>

#include "processor.h"

#include <gfx/application.h>

//extern mapper_t* g_mapper;

int main(int argc, char* argv[])
{
    bool loaded = false;
	if (argc == 2) {
        printf("loading rom %s\n", argv[1]);
		loaded = CFileLoader::LoadRom(argv[1]);
	} else {
        //loaded = CFileLoader::LoadRom("/home/kevin/source/mnes/bin/roms/smb1.nes");
        loaded = CFileLoader::LoadRom("/run/media/ksucre/MIX TAPE/mnes/data/roms/C1.nes");
        //loaded = CFileLoader::LoadRom("/home/kevin/source/mnes/data/roms/Ark.nes");
    }

    if (loaded)
    {
        Start();
    }

    //create the application
    Application app;
    //run it
    return app.run();
}
