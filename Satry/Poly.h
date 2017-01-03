#pragma once
#include "Vertex.h"
#define POLY_STATE_ACTIVE 0x0001
#define POLY_STATE_CLIPPED 0x0002
#define POLY_STATE_BACKFACE  0x0004
class Poly
{
public:
	int state;//�ü��޳���״̬��Ϣ
	int attribute;//������������ԣ�˫�棬����⣬͸����
	UINT32 color;
	Vector4 lightColor[3];
	Vector4* vList;
	int vert[3];
	Vector4* normList;
	int norm[3];
	Vector4* reflectList;
	Vector2* uvList;
	int uv[3];
	float nLength;//�淨����������
	int matIndex;

	Poly();
	Poly(Vector4* vList, int va, int vb, int vc, Vector4* normList, int na, int nb, int nc, Vector2* uvList, int uva, int uvb, int uvc);
	~Poly();
};

