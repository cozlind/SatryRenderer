#pragma once
#include "Matrix.h"
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4();
	Vector4(float[]);
	Vector4(float, float, float, float w=1);


	static float Angle(const Vector4&, const Vector4&);
	static float Dot(const Vector4&, const Vector4&);
	static Vector4 Reflect(const Vector4&, const Vector4&);
	static Vector4 Cross(const Vector4&, const Vector4&);
	static Vector4 Rotate(const Vector4&, const Vector4&, float);
	static Vector4 RotateInterpolate(const Vector4&, const Vector4&, float);
	static Vector4 Interpolate(const Vector4&, const Vector4&, float);
	static void Swap(Vector4&, Vector4&);
	static void GetColorV4(UINT32, Vector4&);

	void SetR(float r) { x = r; }
	void SetG(float g) { y = g; }
	void SetB(float b) { z = b; }
	void SetA(float a) { w = a; }
	float GetR() { return x; }
	float GetG() { return y; }
	float GetB() { return z; }
	float GetA() { return w; }
	void Print()const;
	Vector4& Normalize();
	float Magnitude()const;

	UINT32 GetColor255()const;
	UINT32 GetColor01()const;
	void Rotate(const Vector4&, float);
	Vector4 operator + (const Vector4&) const;
	Vector4 operator - (const Vector4&) const;
	Vector4 operator * (float) const;
	Vector4 operator * (const Vector4&) const;
	Vector4 operator * (const Matrix&) const;
	Vector4& operator = (const Vector4&);
	Vector4& operator += (const Vector4& right);
	Vector4& operator *= (const Vector4& right);
	Vector4& operator *= (const Matrix& right);
	bool operator == (const Vector4&) const;
};

