#pragma once
#include "Obj.h"
#include "Material.h"

extern int numMat;

class FileIO
{
public:
	FILE *fp,*matFp;
	//用于记录单次读入一个文件多个模型的总顶点等数目,为了每个模型单独一个顶点列表而存在
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

