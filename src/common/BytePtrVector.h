#pragma once

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
