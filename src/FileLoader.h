#ifndef __FILELOADER_HPP__
#define __FILELOADER_HPP__

#include <string>
using std::string;

class CFileLoader
{
	CFileLoader();
public:

	~CFileLoader();

	static bool LoadRom(const string& sFileName);
};

#endif