#pragma once
#include "Vertex.h"
///





//����δʹ�� This class isn't used






///
class Triangle
{
public:
	int state;//�ü��޳���״̬��Ϣ
	int attribute;//������������ԣ�˫�棬����⣬͸����
	UINT32 color;

	Vertex initVList[3];//��ʼ�����εĶ���
	Vertex vList[3];//�任��Ķ���

	Triangle *prev;  
	Triangle *next;
	Triangle();
	~Triangle();
};

