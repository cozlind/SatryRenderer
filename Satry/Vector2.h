#pragma once
class Vector2
{
public:
	float x;
	float y;

	Vector2();
	Vector2(float, float);

	static float Dot(const Vector2&, const Vector2&);
	static float Cross(const Vector2&, const Vector2&);
	static Vector2 Interpolate(const Vector2&, const Vector2&, float);

	float Magnitude()const;
	Vector2& Normalize(); 
	static void Swap(Vector2& v1, Vector2& v2);

	Vector2 operator + (const Vector2&) const;
	Vector2 operator - (const Vector2&) const;
	Vector2 operator * (float) const;
	Vector2& operator += (const Vector2& right);
	Vector2& operator = (const Vector2& right);
	bool operator == (const Vector2&) const;
};

