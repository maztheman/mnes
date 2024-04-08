#include "control_flags.h"

#define NS mnes::common::

namespace {
bool bCpuRunning = false;
bool bDisplayReady = false;
bool bPatternTableReady = false;
bool bNameTableReady = false;
}

bool NS is_cpu_running() { return bCpuRunning; }
bool NS is_display_ready() { return bDisplayReady; }
bool NS is_pattern_table_ready() { return bPatternTableReady; }
bool NS is_name_table_ready() { return bNameTableReady; }

void NS set_cpu_running(bool value) { bCpuRunning = value; }
void NS set_display_ready(bool value) { bDisplayReady = value; }
void NS set_pattern_table_ready(bool value) { bPatternTableReady = value; }
void NS set_name_table_ready(bool value) { bNameTableReady = true; }
void NS set_all_display_ready(bool value)
{
  set_display_ready(value);
  set_pattern_table_ready(value);
  set_name_table_ready(value);
}