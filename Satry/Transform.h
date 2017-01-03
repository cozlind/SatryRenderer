#pragma once
#include "Matrix.h"
#include "Vertex.h"
#include "Vector4.h"
class Transform
{
public:
	Matrix model;//�ֲ��ռ�->����ռ�
	Matrix view;//����ռ�->�Ӽ��ռ�
	Matrix projection;//�Ӽ��ռ�->���ÿռ�
	Matrix general;//ͨ�ñ任����
	Transform();
	~Transform();
	void Update();
	static void ViewportInverseTransform(Vector4&);
	static void ViewportTransform(Vector4&);
};

