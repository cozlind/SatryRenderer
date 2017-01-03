#include "stdafx.h"
#include "Material.h"

Material::Material() :state(MAT_STATE_ACTIVE), id(0), attribute(0) {}


Material::~Material()
{
}

Vector4 Material::GetTexColor255(float u, float v)
{
	u = u < 0 ? 0 : (u>1 ? 1 : u);
	v = v < 0 ? 0 : (v>1 ? 1 : v);
	int x = (int)(u*(texture.width - 1)+0.5f);
	int y = (int)(v*(texture.height - 1) + 0.5f);
	x = texture.width - x - 1;//右手坐标系转左手坐标系
	//注意是BGR的存储方式
	int b = *(texture.data + y*texture.lineBytes + x * 3);
	int g = *(texture.data + y*texture.lineBytes + x * 3 + 1);
	int r = *(texture.data + y*texture.lineBytes + x * 3 + 2);
	return Vector4(r, g, b, 0);
}

Vector4 Material::GetTexture(int x, int y)
{
	//注意是BGR的存储方式
	int b = *(texture.data + y*texture.lineBytes + x * 3);
	int g = *(texture.data + y*texture.lineBytes + x * 3 + 1);
	int r = *(texture.data + y*texture.lineBytes + x * 3 + 2);
	return Vector4(r, g, b, 0);
}
