#pragma once
#include "Matrix.h"
#include "Vertex.h"
#include "Vector4.h"
class Transform
{
public:
	Matrix model;//局部空间->世界空间
	Matrix view;//世界空间->视见空间
	Matrix projection;//视见空间->剪裁空间
	Matrix general;//通用变换矩阵
	Transform();
	~Transform();
	void Update();
	static void ViewportInverseTransform(Vector4&);
	static void ViewportTransform(Vector4&);
};

