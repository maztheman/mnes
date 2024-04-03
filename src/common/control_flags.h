#pragma once


struct runtime_variables
{
  bool CpuRunning{ false };
  bool DisplayReady{ false };
  bool PatternTableReady{ false };
  bool NameTableReady{ false };

  void setReady(bool value) { DisplayReady = PatternTableReady = NameTableReady = value; }
};


runtime_variables &RV();

extern bool g_bCpuRunning;
extern bool g_bDisplayReady;
extern bool g_bPatternTableReady;
extern bool g_bNameTableReady;
