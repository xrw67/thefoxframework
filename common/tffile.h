#ifndef _TF_FILE_H_
#define _TF_FILE_H_

class TFFile
{
public:
	TFFile();
	~TFFile();
	bool Open();
	bool Close();
	std::string ReadLine();
	bool WriteLine();

private:
	FILE *m_pFile;

};

#endif // _TF_FILE_H_