#pragma once

#include <string>
using std::string;

class CFile
{
public:
	CFile(const string& sFileName, const string& mode)
		: m_fp(nullptr)
	{
		m_fp = fopen( sFileName.c_str(), mode.c_str() );
	}

	~CFile() {
		Close();
	}

	bool IsOpen() {
		return m_fp != nullptr;
	}

	void Close() {
		if (m_fp != nullptr) {
			fclose(m_fp);
			m_fp = nullptr;
		}
	}

	size_t Read(void* dstBuf, size_t elementSize, size_t count) {
		return fread( dstBuf, elementSize, count, m_fp );
	}

private:

	FILE* m_fp;

};
