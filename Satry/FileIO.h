#pragma once
#include "Obj.h"
#include "Material.h"

extern int numMat;

class FileIO
{
public:
	FILE *fp,*matFp;
	//���ڼ�¼���ζ���һ���ļ����ģ�͵��ܶ������Ŀ,Ϊ��ÿ��ģ�͵���һ�������б������
	int numV = 1, numUV =1, numN = 1;
	char filePath[64];
	bool FileIO::OpenObjFile(const char path[], const char fileName[]);
	int LoadAllMat(Material[]);
	void readBitmap(char path[],Material&);
	void readBitmap(char path[], Bmp& );
	bool LoadObjModel(Obj& obj,Material[]);
	void CloseFile();
	void Reset();
	FileIO();
	~FileIO();

};

