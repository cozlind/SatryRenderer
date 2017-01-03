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
	int id;//物体ID
	char name[64];//名称
	int state;//状态
	int attribute;//属性
	float avgRadius;//平均半径，碰撞检测用
	float maxRadius;//最大半径
	Vector4 worldPos;//世界坐标
	Vector4 ux, uy, uz;//局部坐标轴朝向

	int numVertices;//每帧包含物体顶点数
	Vector4 bakVList[100000];//顶点局部坐标数组
	Vector4 vList[100000];//变换后的坐标数组
	
	int numNorm;
	Vector4 bakNormList[100000];
	Vector4 normList[100000];
	Vector4 reflectList[100000];

	int numUV;
	Vector2 uvList[100000];

	int numPolys;//多边形数
	Poly pList[100000];//多边形数组
	
	int matIndex=-1;

	Obj(); 
	Obj(char name[], float avgRadius, float maxRaidus, Vector4 dir, int numVertices, Vertex verts[], int numPolys, int polyIndex[]);
	~Obj();
};

