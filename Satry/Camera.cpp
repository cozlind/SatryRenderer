#include "stdafx.h"
#include "Camera.h"
#include "Transform.h"
#define PI 3.1415926f
extern Transform transform;
extern LONG width;
extern LONG height;
void Camera::UVNUpdate()
{
	n = (at-pos).Normalize();
	u = Vector4::Cross(n, up*-1).Normalize();
	v = Vector4::Cross(n, u).Normalize();
}
Camera::Camera()
{
	fovx = PI*0.5f;
	fovy = PI*0.5f;
	zNear = 1;
	zFar = 500;
	isPerspective = true;
	pos = Vector4(0, 0, -10.0f);
	at = Vector4(0, 0, 0);
	up = Vector4(0, 1, 0);
	UVNUpdate();
}
void Camera::Init()
{
	aspect = (float)width / (float)height;
	viewPlaneWidth = 2;
	viewPlaneHeight = viewPlaneWidth / aspect;
	viewDistanceX = 0.5f*viewPlaneWidth/tan(fovx / 2);
	viewDistanceY = 0.5f*viewPlaneHeight/tan(fovy / 2);
}
Camera::Camera(Vector4 p,Vector4 lookAt,Vector4 upward)
{
	fovx = PI*0.5f;
	fovy = PI*0.5f;
	zNear = 1;
	zFar = 500;
	isPerspective = true;
	pos = p;
	at = lookAt;
	up = upward;
	UVNUpdate();
}
void Camera::Rotate(const Vector4& _n, const Vector4& _u, const Vector4& _v, const Vector4& _pos, const Vector4& _at,float x, float y) 
{
	n = _n, u=_u;
	n.Rotate(v, x);
	u.Rotate(v, x);
	v = _v;
	v.Rotate(u, y);
	n.Rotate(u, y);
	Vector4 dir = pos - at;
	float l = dir.Magnitude();
	at = pos + n.Normalize()*l;
	up = v;
	SetViewTransform();
}
void Camera::RotateFocus(const Vector4& _n, const Vector4& _u, const Vector4& _v, const Vector4& _pos, const Vector4& _at, float x, float y)
{
	n = _n,up = _v,u=_u, at = _at, pos = _pos;
	Vector4 dir = pos - at;
	float l = dir.Magnitude();
	dir.Rotate(Vector4(0, 1, 0), x);
	dir=dir.Normalize()*l;
	up.Rotate(Vector4(0, 1, 0), -x);
	pos = at + dir;

	/*n = at - pos;
	u = Vector4::Cross(n, up*-1).Normalize();
	n.Rotate(u, y);
	n.Normalize();
	pos = at - n*(at - pos).Magnitude();*/

	SetViewTransform();
}
void Camera::RotateFocusY(const Vector4& _n, const Vector4& _u, const Vector4& _v, const Vector4& _pos, const Vector4& _at,float y)
{
	n = _n, up = _v, u = _u, at = _at, pos = _pos;
	u = Vector4::Cross(n, up*-1).Normalize();
	n.Rotate(u, y);
	n.Normalize();
	pos = at - n*((at - pos).Magnitude());
	SetViewTransform();
}
Camera::~Camera()
{
}

void Camera::SetViewTransform()
{
	UVNUpdate();
	float m[4][4] = {	{ u.x,					 v.x,					n.x,				   0 },
						{ u.y,					 v.y,					n.y,				   0 },
						{ u.z,					 v.z,					n.z,                   0 },
						{ -Vector4::Dot(u, pos), -Vector4::Dot(v, pos), -Vector4::Dot(n, pos), 1 } };
	transform.view = m;
}

void Camera::SetProjectionTransform()
{
	UVNUpdate();
	float cot = 1.0f / (float)tan(fovy * 0.5f);
	float m[4][4] = {	{ cot / aspect, 0,		0,								0 },
						{ 0,			cot,	0,								0 },
						{ 0,			0,		zFar / (zFar - zNear),			1 },
						{ 0,			0,		-zNear*zFar / (zFar - zNear),	0 } };
	transform.projection = m;
}