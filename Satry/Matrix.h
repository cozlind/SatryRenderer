#pragma once
class Matrix
{
public:
	float m[4][4];

	Matrix();
	Matrix(float a[4][4]);

	void Print()const;
	void SetIdentity();
	void SetZero();
	void SetTranslate(float x, float y, float z);
	void SetRotate(float x, float y, float z, float theta);
	void SetScale(float x, float y, float z);
	Matrix InverseTranspose();
	Matrix Inverse();

	Matrix operator+(const Matrix&)const;
	Matrix operator-(const Matrix&)const;
	Matrix operator*(const Matrix&)const;
	Matrix operator*(float)const;
	Matrix& operator = (const Matrix&);
	bool operator == (const Matrix&) const;
};

