#pragma once
#include "Vector4.h"
class Camera
{
public:
	float fovx;
	float fovy;//�ӽ�
	float viewDistanceX;//�Ӿ�X
	float viewDistanceY;//�Ӿ�Y
	float viewPlaneWidth;//�ӿڿ��
	float viewPlaneHeight;//�ӿڸ߶�
	float aspect;//��߱�
	float zNear;//���ü�ƽ��
	float zFar;//Զ�ü�ƽ��
	bool isPerspective;
	Vector4 pos;//�������������
	Vector4 at;//�������������λ��
	Vector4 up;//��������ϵ��y����
	Vector4 n;//��������ϵz���򣬵�λ����
	Vector4 v;//���Ϸ�����������ϵy���򣬵�λ����
	Vector4 u;//������ϵ��������������ϵx���򣬵�λ����
	Camera();
	Camera(Vector4 position, Vector4 lookAt=Vector4(0,0,0), Vector4 upward=Vector4(0,1,0));
	~Camera();
	void Init();
	void Rotate(const Vector4& _n, const Vector4& _u, const Vector4& _v, const Vector4& _pos, const Vector4& _at, float x, float y);
	void RotateFocus(const Vector4& _n, const Vector4& _u, const Vector4& _v, const Vector4& _pos, const Vector4& _at, float x, float y);
	void RotateFocusY(const Vector4& _n, const Vector4& _u, const Vector4& _v, const Vector4& _pos, const Vector4& _at, float y);
	void SetViewTransform();
	void SetProjectionTransform();
	void UVNUpdate();
};

