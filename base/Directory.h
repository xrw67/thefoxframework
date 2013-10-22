#ifndef _THEFOX_DIRECTORY_H_
#define _THEFOX_DIRECTORY_H_

// 得到完整的路径，并且创建目录,末尾包含分隔符
inline bool MakeFullPath(const char *dir, char *fullPath, size_t fullPathLen)
{
	if (fullPathLen < 2)
	{
		return false;
	}

	size_t pathLen = 0;
	char filePath[512] = {0};

	if (NULL == dir)
	{
		_getcwd(filePath, sizeof(filePath));
	}
	else
	{
		bool bAbsolutePath = true;

		// 判断dir是绝对路径还是相对路径
#ifdef WIN32
		if (NULL == strchr(dir, ':'))
		{
			bAbsolutePath = false;
		}
#else
		if (dir[0] != '/')
		{
			bAbsolutePath = false;
		}
#endif
		_getcwd(filePath, sizeof(filePath));

		if (!bAbsolutePath) // 如果是相对路径，生成绝对路径
		{
			pathLen = strlen(filePath);
			if ('/' != filePath[pathLen] || '\\' != filePath[pathLen])
			{
				strcat(filePath, "/");
				++pathLen;
			}
			strncat(filePath, dir, sizeof(filePath) - pathLen);
		}
		//! 创建目录
		char *curDir = filePath;
		while ('\0' != *curDir)
		{
			if ('\\' == *curDir || '/' == *curDir)
			{
				*curDir = '\0';
				_mkdir(filePath);
				*curDir = '/';
			}
			++curDir;
		}
		_mkdir(filePath);
	}

	pathLen = strlen(filePath);
	if ('/' != filePath[pathLen])
	{
		strcat(filePath, "/");
		++pathLen;
	}
	if (pathLen < fullPathLen)
	{
		strcpy(fullPath, filePath);
		return true;
	}
	return false;
}

#endif // _THEFOX_DIRECTORY_H_