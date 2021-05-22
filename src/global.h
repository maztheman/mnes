#pragma once

#include "BytePtrVector.h"
#include "log.h"

extern CBytePtrVector g_BufferCollection;
inline CBytePtrVector& VBufferCollection() {
	return g_BufferCollection;
}

//#define ID_FILE_OPEN				1
#define ID_FILE_CLOSE				2
//#define ID_EMULATION_START			1
//#define ID_EMULATION_STOP			2
//#define ID_EMULATION_RESET_HARD		3
//#define ID_EMULATION_RESET_SOFT		4
