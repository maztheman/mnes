#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <cstdarg>

using std::string;
using std::vector;

class CLog
{
public:
	CLog(string sName);
	~CLog(void);

	void Start(const string& sLogfileName);
	void AddLine(const string& sLine);
	void AddLine(const char* pcszFormat, ...);
	void Close();

	string& Name() {
		return m_sName;
	}
private:

	FILE* m_fp;
	string m_sName;
};

#include <algorithm>

class CLogPtrVector : public vector<CLog*>
{
public:
	CLogPtrVector() {
	}
	~CLogPtrVector()
	{
		ResetContent();
	}

	void ResetContent() {
		if (empty()) { return; }

		for (auto& p : *this) {
			delete p;
		}

		clear();

		CLogPtrVector().swap(*this);
	}

	struct SName
	{
		bool operator()(CLog* pLeft, const string& right) {
			return pLeft->Name() < right;
		}

		bool operator()(CLog* pLeft, CLog* pRight) {
			return pLeft->Name() < pRight->Name();
		}
	};

	CLog* FindByName(const string& sName) {
		auto test = std::lower_bound(begin(), end(), sName, SName());
		bool bFound = (test != end() && !(sName < (*test)->Name()));
		return bFound ? *test : nullptr;
	}

	void Sort() {
		std::sort(begin(), end(), SName());
	}

};

extern CLogPtrVector g_arLogs;
inline CLogPtrVector& VLogCollection() {
	return g_arLogs;
}

inline CLog& VLog(const string& sName) {
	CLog* pLog = g_arLogs.FindByName(sName);
	if (pLog == nullptr) {
		pLog = new CLog(sName);
		pLog->Start("logs/" + sName + ".log");
		g_arLogs.push_back( pLog );
		g_arLogs.Sort();
	}
	return *pLog; 
}

//The main log.
inline CLog& VLog() { 
	CLog* pLog = g_arLogs.FindByName("main");
	if (pLog == nullptr) {
		pLog = new CLog("main");
		pLog->Start("logs/mnes.log");
		g_arLogs.push_back( pLog );
		g_arLogs.Sort();
	}
	return *pLog; 
}

#ifdef USE_LOG
#define MLOG(x, ...) { VLog().AddLine(x, ## __VA_ARGS__); }
#define MLOG_PPU(x, ...) { VLog("ppu").AddLine(x, ## __VA_ARGS__); }
#else
#define MLOG(x, ...) {}
#define MLOG_PPU(x, ...) {}
#endif
