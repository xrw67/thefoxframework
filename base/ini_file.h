#ifndef _THEFOX_BASE_INI_FILE_H_
#define _THEFOX_BASE_INI_FILE_H_

#include <Winbase.h>
#include <base/common.h>

namespace thefox
{

class IniFile
{
public:
	IniFile(const char *fileName)
		: _fileName(fileName) {}
	~IniFile() {}

	int getInt(const char *section, const char *key, int defaultValue)
	{ return ::GetPrivateProfileInt(section, key, defaultValue, _fileName.c_str()); }

	std::string getString(const char *section, const char *key, const char *defaultValue)
	{ 
		char buf[8192] = {0};
		::GetPrivateProfileString(section, key, defaultValue, buf, sizeof(buf), _fileName.c_str());
		return buf;
	}

	void setInt(const char *section, const char *key, int value)
	{
		char buf[32] = {0};
		snprintf(buf, sizeof(buf), "%d", value);
		::WritePrivateProfileStringA(section, key, buf, _fileName.c_str());
	}

	void setString(const char *section, const char *key, const std::string &value)
	{ ::WritePrivateProfileStringA(section, key, value.c_str(), _fileName.c_str()); }

private:
	std::string _fileName;
};

} // namespace thefox

#endif // _THEFOX_BASE_INI_FILE_H_