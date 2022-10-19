#pragma once

#include <filesystem>

//re-write this using std::filesystem and istream::read() no c!

class CFileLoader
{
	CFileLoader();
public:

	~CFileLoader();

	static bool LoadRom(const std::filesystem::path& fileName);
};

