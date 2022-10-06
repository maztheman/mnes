#pragma once

using std::string;

class CFileLoader
{
	CFileLoader();
public:

	~CFileLoader();

	static bool LoadRom(const string& sFileName);
};

