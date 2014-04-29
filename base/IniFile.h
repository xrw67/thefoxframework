#ifndef _THEFOX_BASE_INI_FILE_H_
#define _THEFOX_BASE_INI_FILE_H_

#include <base/Types.h>

namespace thefox
{

class IniFile
{
public:
	IniFile(const char *fileName)
		: _fileName(fileName) 
	{}
	~IniFile() 
	{}

	int getInt(const char *section, const char *key)
	{ 
		return ::GetPrivateProfileInt(section, key, 0, _fileName.c_str()); 
	}
	
	std::string getString(const char *section, const char *key)
	{ 
		char buf[8192] = {0};
		::GetPrivateProfileString(section, key, "", buf, sizeof(buf), _fileName.c_str());
		return buf;
	}
	
private:
	std::string _fileName;
};

} // namespace thefox

#endif // _THEFOX_BASE_INI_FILE_H_