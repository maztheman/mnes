#ifndef __BYTEPTRVECTOR_H__
#define __BYTEPTRVECTOR_H__

#include <vector>


class CBytePtrVector : public std::vector<unsigned char*>
{
public:
	CBytePtrVector() {}
	~CBytePtrVector()
	{
		ResetContent();
	}

	void ResetContent()
	{
		if (empty()) return;//stop recursive
		for (auto& p : *this) {
			delete p;
		}

		clear();

		CBytePtrVector().swap(*this);
	}
};



#endif