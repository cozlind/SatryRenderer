#include "stdafx.h"
#include "Vector2.h"
using namespace std;
#define INTERP(x1, x2,t) (x1 + (x2 - x1) * t)

Vector2::Vector2() :x(0), y(0) {}

Vector2::Vector2(float _x, float _y) : x(_x), y(_y) {}

float Vector2::Dot(const Vector2& v1, const Vector2& v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

float Vector2::Cross(const Vector2& v1, const Vector2& v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

Vector2 Vector2::Interpolate(const Vector2& v1, const Vector2& v2, float param)
{
	return Vector2(INTERP(v1.x, v2.x, param), INTERP(v1.y, v2.y, param));
}
float Vector2::Magnitude()const
{
	return sqrt(x*x + y*y);
}

Vector2& Vector2::Normalize()
{
	float len = Magnitude();
	if (len == 0) return *this;
	x /= len;
	y /= len;
	return *this;
}
void Vector2::Swap(Vector2& v1, Vector2& v2)
{
	float t;
	t = v1.x, v1.x = v2.x, v2.x = t;
	t = v1.y, v1.y = v2.y, v2.y = t;
}
Vector2 Vector2::operator + (const Vector2& right) const
{
	return Vector2(x + right.x, y + right.y);
}

Vector2 Vector2::operator - (const Vector2& right) const
{
	return Vector2(x - right.x, y - right.y);
}
Vector2 Vector2::operator * (float right) const
{
	return Vector2(x*right, y *right);
}
Vector2& Vector2::operator += (const Vector2& right)
{
	x += right.x;
	y += right.y;
	return *this;
}
Vector2& Vector2::operator = (const Vector2& right)
{
	x = right.x;
	y = right.y;
	return *this;
}
bool Vector2::operator == (const Vector2& right) const
{
	return x == right.x&&y == right.y;
}