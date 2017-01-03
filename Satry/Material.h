#pragma once
#include "Vector4.h"
#define MAT_ATTR_2SIDED 0x0001
#define MAT_ATTR_TRANSPARENT 0x0002
#define MAT_ATTR_8BITCOLOR 0x0004
#define MAT_ATTR_RGB16 0x0008
#define MAT_ATTR_RGB24 0x0010
#define MAT_ATTR_SHADE_MODE_CONSTANT 0x0020
#define MAT_ATTR_SHADE_MODE_EMMISIVE 0x0020
#define MAT_ATTR_SHADE_MODE_FLAT 0x0040
#define MAT_ATTR_SHADE_MODE_GOURAUD 0x0080
#define MAT_ATTR_SHADE_MODE_FASTPHONG 0x0100
#define MAT_ATTR_SHADE_MODE_TEXTURE 0x0200
#define MAT_STATE_ACTIVE 0x0001

class Material
{
public:
	int id;
	int state;
	int attribute;
	char name[64];
	UINT32 color;
	Vector4 ka, kd, ks;//环境光，散射光，镜面反射光，颜色和反射系数的积
	Vector4 tf;//透射光
	float ni;//折射光
	char filePath[80];//纹理文件位置
	Bmp texture;//纹理图
	Material();
	~Material();
	Vector4 GetTexColor255(float u, float v);
	Vector4 GetTexture(int x,int y);
};

