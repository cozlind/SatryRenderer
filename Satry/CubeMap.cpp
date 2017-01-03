#include "stdafx.h"
#include "FileIO.h"
#include "CubeMap.h"
#define HALFSIZE 255 //原点作为0-511中的256位置
extern FileIO fileIO;

Vector4 CubeMap::GetCubeMapColor255(float rx,float ry,float rz)
{
	if (rx == 0 && ry == 0 && rz == 0) return Vector4(0,0,0);
	//判定方向贴图
	int index;
	if (abs(rz) >= abs(rx) && abs(rz) >= abs(ry)) 
	{
		index = rz >= 0 ? 0 : 3;
	}
	else if (abs(rx) >= abs(rz)&&abs(rx)>=abs(ry)) 
	{
		index = rx >= 0 ? 1 : 4;
	}
	else if (abs(ry) >= abs(rz)&&abs(ry)>=abs(rx))
	{
		index = ry >= 0 ? 2 : 5;
	}
	int x, y;
	switch (index) {
	case 0://pz
		x = (int)(rx*HALFSIZE /rz + 256);
		y = (int)(ry * HALFSIZE / rz + 256);
		break;
	case 1://px
		x = (int)(-rz*HALFSIZE / rx + 256);
		y = (int)(ry*HALFSIZE / rx + 256);
		break;
	case 2://py
		x = (int)(rx*HALFSIZE / ry + 256);
		y = (int)(-rz*HALFSIZE / ry + 256);
		break;
	case 3://nz
		x = (int)(-rx*HALFSIZE / -rz + 256);
		y = (int)(ry*HALFSIZE / -rz + 256);
		break;
	case 4://nx
		x = (int)(rz*HALFSIZE / -rx + 256);
		y = (int)(ry*HALFSIZE / -rx + 256);
		break;
	case 5://ny
		x = (int)(rx*HALFSIZE / -ry + 256);
		y = (int)(rz*HALFSIZE / -ry + 256);
		break;
	}
	 //注意是BGR的存储方式
	int b = *(textures[index].data + y*textures[index].lineBytes + x * 3);
	int g = *(textures[index].data + y*textures[index].lineBytes + x * 3 + 1);
	int r = *(textures[index].data + y*textures[index].lineBytes + x * 3 + 2);
	return Vector4(r,g, b, 1);
}

void CubeMap::ReadCubeMap() 
{
	fileIO.readBitmap(".\\skybox\\pz.bmp", textures[0]);
	fileIO.readBitmap(".\\skybox\\px.bmp", textures[1]);
	fileIO.readBitmap(".\\skybox\\py.bmp", textures[2]);
	fileIO.readBitmap(".\\skybox\\nz.bmp", textures[3]);
	fileIO.readBitmap(".\\skybox\\nx.bmp", textures[4]);
	fileIO.readBitmap(".\\skybox\\ny.bmp", textures[5]);
}

CubeMap::CubeMap()
{
}


CubeMap::~CubeMap()
{
}
