#pragma once

#include <cstdarg>
#include <cstdio>

using std::string;
using std::vector;

class CLog
{
public:
  explicit CLog(string sName);
  ~CLog(void);

  void Start(const string &sLogfileName);
  void AddLine(const string &sLine);
  void AddLine(const char *pcszFormat, ...);
  void Close();

  const string &Name() const { return m_sName; }

private:
  FILE *m_fp;
  string m_sName;
};

#include <unordered_map>

using LogNameMap = std::unordered_map<std::string, std::unique_ptr<CLog>>;

inline LogNameMap &VLogCollection()
{
  static LogNameMap instance;
  return instance;
}

inline CLog &VLog(const string &sName)
{

  if (auto ins = VLogCollection().insert(std::make_pair(sName, std::make_unique<CLog>(sName))); ins.second) {
    ins.first->second->Start("logs/" + sName + ".log");
    return *(ins.first->second.get());
  } else {
    return *(ins.first->second.get());
  }
}

// The main log.
inline CLog &VLog() { return VLog("mnes"); }

#ifdef USE_LOG
#define MLOG(x, ...)                  \
  {                                   \
    VLog().AddLine(x, ##__VA_ARGS__); \
  }
#define MLOG_PPU(x, ...)                   \
  {                                        \
    VLog("ppu").AddLine(x, ##__VA_ARGS__); \
  }
#else
#define MLOG(x, ...) \
  {                  \
  }
#define MLOG_PPU(x, ...) \
  {                      \
  }
#endif
