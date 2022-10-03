
#include "mapper.h"
#include "FileLoader.h"
#include "processor.h"
#include "platform.h"

//extern mapper_t* g_mapper;

int main(int argc, char* argv[])
{
	if (argc == 2) {
		CFileLoader::LoadRom(argv[1]);
	}

    InitializeProcessor();

    CGfxManager::mainLoop();

    return 0;
}