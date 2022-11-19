#include "control_flags.h"


runtime_variables& RV()
{
    static runtime_variables instance;
    return instance;
}

bool g_bCpuRunning = false;
bool g_bDisplayReady = false;
bool g_bPatternTableReady = false;
bool g_bNameTableReady = false;