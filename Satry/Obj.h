#pragma once
#include "Vector4.h"
#include "Vertex.h"
#include "Poly.h"

#define OBJ_STATE_CULLED 0x0001
#define POLY_ATTR_2SIDED 0x0001
#define POLY_ATTR_TRANSPARENT 0x0002
#define POLY_ATTR_8BITCOLOR 0x0004
#define POLY_ATTR_RGB16 0x0008
#define POLY_ATTR_RGB24 0x0010
#define SHADE_MODE_PURE 0x0020
#define SHADE_MODE_FLAT 0x0040
#define SHADE_MODE_GOURAUD 0x0080
#define SHADE_MODE_CUBEMAP 0x0100

class Obj
{
public:
	int id;//����ID
	char name[64];//����
	int state;//״̬
	int attribute;//����
	float avgRadius;//ƽ���뾶����ײ�����
	float maxRadius;//���뾶
	Vector4 worldPos;//��������
	Vector4 ux, uy, uz;//�ֲ������ᳯ��

	int numVertices;//ÿ֡�������嶥����
	Vector4 bakVList[100000];//����ֲ���������
	Vector4 vList[100000];//�任�����������
	
	int numNorm;
	Vector4 bakNormList[100000];
	Vector4 normList[100000];
	Vector4 reflectList[100000];

	int numUV;
	Vector2 uvList[100000];

	int numPolys;//�������
	Poly pList[100000];//���������
	
	int matIndex=-1;

	Obj(); 
	Obj(char name[], float avgRadius, float maxRaidus, Vector4 dir, int numVertices, Vertex verts[], int numPolys, int polyIndex[]);
	~Obj();
};

