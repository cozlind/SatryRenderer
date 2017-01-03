#pragma once
#include "Vector4.h"
class Camera
{
public:
	float fovx;
	float fovy;//视角
	float viewDistanceX;//视距X
	float viewDistanceY;//视距Y
	float viewPlaneWidth;//视口宽度
	float viewPlaneHeight;//视口高度
	float aspect;//宽高比
	float zNear;//近裁剪平面
	float zFar;//远裁剪平面
	bool isPerspective;
	Vector4 pos;//相机的世界坐标
	Vector4 at;//朝向的世界坐标位置
	Vector4 up;//世界坐标系的y朝向
	Vector4 n;//世界坐标系z朝向，单位向量
	Vector4 v;//正上方，世界坐标系y朝向，单位向量
	Vector4 u;//（左手系）左方向，世界坐标系x朝向，单位向量
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

