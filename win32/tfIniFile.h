
/*

[Key]
NAME=TEXT

*/

#ifndef _TF_STRINGMAP_H_
#define _TF_STRINGMAP_H_

#include <map>
#include <string>

class TFIniFile
{
public:
	TFIniFile(const char *filename)
	{
	
	}
	~TFIniFile()
	{
	
	}
	bool LoadFile(const char *filename)
	{
		ASSERT(filename);
		FILE *pFile = fopen(filename, "r");
		if (pFile)
		{
			char line[512] = {0};
			while (fgets(line, sizeof(line), pFile)
			{
				
				ParseNewLine(line, 
			}
		}
		return false;
	}
	
	bool UnescapeString(std::string &s)
	{
	
	}
private:
	std::string m_fileName;
	std::map<std::string, std::string> m_map;
};

#endif // _TF_STRINGMAP_H_