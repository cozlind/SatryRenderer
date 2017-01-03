#include "stdafx.h"
#include "Transform.h"
#include "Matrix.h"
extern LONG width;
extern LONG height;
Transform::Transform()
{
	model.SetIdentity();
	view.SetIdentity();
	projection.SetIdentity();
	general.SetIdentity();
}


Transform::~Transform()
{
}

void Transform::Update()
{
	general = model*view*projection;
}
void Transform::ViewportTransform(Vector4& v)
{
	float x = v.x, y = v.y, z = v.z, rhw = 1.0f / v.w;
	v.x = width*0.5f*(x*rhw + 1);
	v.y = height*0.5f*(1-y*rhw);
	v.z = z*rhw;
}
void Transform::ViewportInverseTransform(Vector4& v) 
{
	float x = v.x, y = v.y, z = v.z, rhw = 1.0f / v.w;
	v.x =( x / width / 0.5f - 1) / rhw;
	v.y = (1 - y / 0.5f / height) / rhw;
	v.z = z/rhw;
}