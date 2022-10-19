
#include <common/FileLoader.h>

#include "processor.h"

#include <gfx/application.h>

//extern mapper_t* g_mapper;

int main()
{
    bool loaded = false;
    loaded = CFileLoader::LoadRom("/run/media/ksucre/MIX TAPE/mnes/data/roms/C1.nes");

    if (!loaded)
    {
        fmt::print(stderr, "no rom loaded, exiting.\n");
        return 0;
    }
    //create the application
    Application app;
    //run it
    return app.run();
}
