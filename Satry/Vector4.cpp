#include "stdafx.h"
#include "Vector4.h"
using namespace std;
// 计算插值：t 为 [0, 1] 之间的数值
#define INTERP(x1, x2,t) (x1 + (x2 - x1) * t)


Vector4::Vector4() :x(0), y(0), z(0), w(0) {}

Vector4::Vector4(float num[]) : x(num[0]), y(num[1]), z(num[2]), w(num[3]) {}

Vector4::Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
//求单位向量的夹角
float Vector4::Angle(const Vector4& v1, const Vector4& v2)
{
	float x = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	x = x < -1 ? -1 : x;
	x = x > 1 ? 1 : x;
	return acos(x);
}

float Vector4::Dot(const Vector4& v1, const Vector4& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector4 Vector4::Reflect(const Vector4& v1, const Vector4& v2)
{
	return v1 - v2*(2*(v1.x*v2.x+v1.y*v2.y+v1.z*v2.z));
}

Vector4 Vector4::Cross(const Vector4& v1, const Vector4& v2)
{
	return Vector4(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v2.z*v1.x, v1.x*v2.y - v2.x*v1.y);
}
void Vector4::Swap(Vector4& v1, Vector4& v2)
{
	float t;
	t = v1.x, v1.x = v2.x, v2.x = t;
	t = v1.y, v1.y = v2.y, v2.y = t;
	t = v1.z, v1.z = v2.z, v2.z = t;
	t = v1.w, v1.w = v2.w, v2.w = t;
}
// 矢量插值，t取值 [0, 1]
Vector4 Vector4::Interpolate(const Vector4& v1, const Vector4& v2, float param)
{
	return Vector4(INTERP(v1.x, v2.x, param), INTERP(v1.y, v2.y, param), INTERP(v1.z, v2.z, param), v1.w);
}
//向量绕指定轴向旋转
void Vector4::Rotate(const Vector4& axis, float theta)
{
	Matrix rotateMatrix;
	rotateMatrix.SetRotate(axis.x, axis.y, axis.z, theta);
	(*this)*=rotateMatrix;
}
//向量旋转，输入矢量必须为单位向量
Vector4 Vector4::Rotate(const Vector4& v1, const Vector4& v2, float theta)
{
	Vector4 axis = Cross(v1, v2);
	Matrix rotateMatrix;
	rotateMatrix.SetRotate(axis.x, axis.y, axis.z, theta);
	return v1*rotateMatrix;
}
//球面矢量插值，输入矢量必须为单位向量
Vector4 Vector4::RotateInterpolate(const Vector4& v1, const Vector4& v2, float param)
{
	float theta = acos(Dot(v1, v2));
	Vector4 axis = Cross(v1, v2);
	Matrix rotateMatrix;
	rotateMatrix.SetRotate(axis.x, axis.y, axis.z, INTERP(0, theta, param));
	return v1*rotateMatrix;
}
float Vector4::Magnitude()const
{
	return sqrt(x*x + y*y + z*z);
}

Vector4& Vector4::Normalize()
{
	float len = sqrt(x*x + y*y + z*z);
	if (len == 0) return *this;
	x /= len;
	y /= len;
	z /= len;
	return *this;
}

void Vector4:: GetColorV4(UINT32 c,Vector4& v4)
{
	v4.x = (float)((c >> 16)&255);
	v4.y = (float)((c >> 8) & 255);
	v4.z = (float)(c & 255);
}

UINT32 Vector4::GetColor255() const
{
	int r = (int)(x);
	int g = (int)(y);
	int b = (int)(z);
	return (r << 16) | (g << 8) | b;
}
UINT32 Vector4::GetColor01() const
{
	int r = (int)(x * 255.0f);
	int g = (int)(y * 255.0f);
	int b = (int)(z * 255.0f);
	return (r << 16) | (g << 8) | b;
}
void Vector4::Print()const
{
	cout << "(" << x << "," << y << "," << z << "," << w << ") "<<endl;
}

Vector4 Vector4::operator + (const Vector4& right) const
{
	return Vector4(x + right.x, y + right.y, z + right.z, 1);
}

Vector4 Vector4::operator - (const Vector4& right) const
{
	return Vector4(x - right.x, y - right.y, z - right.z, 0);
}
Vector4 Vector4::operator * (float right) const
{
	return Vector4(x*right, y *right, z*right, 1);
}
//颜色调制
Vector4 Vector4::operator*(const Vector4& right) const
{
	return Vector4(x*right.x, y*right.y, z*right.z, w);
}
Vector4& Vector4::operator *=(const Vector4& right) 
{
	x *= right.x;
	y *= right.y;
	z *= right.z;
	return *this;
}
Vector4 Vector4::operator*(const Matrix& right) const
{
	float vx = x*right.m[0][0] + y*right.m[1][0] + z*right.m[2][0] + w*right.m[3][0];
	float vy = x*right.m[0][1] + y*right.m[1][1] + z*right.m[2][1] + w*right.m[3][1];
	float vz = x*right.m[0][2] + y*right.m[1][2] + z*right.m[2][2] + w*right.m[3][2];
	float vw = x*right.m[0][3] + y*right.m[1][3] + z*right.m[2][3] + w*right.m[3][3];
	return Vector4(vx, vy, vz, vw);
}
Vector4& Vector4::operator = (const Vector4& right)
{
	x = right.x;
	y = right.y;
	z = right.z;
	w = right.w;
	return *this;
}
Vector4& Vector4::operator += (const Vector4& right)
{
	x += right.x;
	y += right.y;
	z += right.z;
	w = 1;
	return *this;
}
Vector4& Vector4::operator *= (const Matrix& right)
{
	x = x*right.m[0][0] + y*right.m[1][0] + z*right.m[2][0] + w*right.m[3][0];
	y = x*right.m[0][1] + y*right.m[1][1] + z*right.m[2][1] + w*right.m[3][1];
	z = x*right.m[0][2] + y*right.m[1][2] + z*right.m[2][2] + w*right.m[3][2];
	w = x*right.m[0][3] + y*right.m[1][3] + z*right.m[2][3] + w*right.m[3][3];
	return *this;
}
bool Vector4::operator == (const Vector4& right) const
{
	return x == right.x&&y == right.y&&z == right.z&&w == right.w;
}