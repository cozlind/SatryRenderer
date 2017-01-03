#pragma once
#include "Vector4.h"
class CubeMap
{
public:
	Bmp textures[6];

	CubeMap();
	~CubeMap();
	Vector4 GetCubeMapColor255(float x, float y, float z);
	void ReadCubeMap();
};

