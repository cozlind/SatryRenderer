#pragma once
#include "Vertex.h"
///





//该类未使用 This class isn't used






///
class Triangle
{
public:
	int state;//裁剪剔除等状态信息
	int attribute;//多边形物理属性，双面，反射光，透明等
	UINT32 color;

	Vertex initVList[3];//初始三角形的顶点
	Vertex vList[3];//变换后的顶点

	Triangle *prev;  
	Triangle *next;
	Triangle();
	~Triangle();
};

