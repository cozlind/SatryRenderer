#include "stdafx.h"
#include "Matrix.h"
#include "Vector4.h"
using namespace std;

Matrix::Matrix()
{
	memset(m, 0, 16*sizeof(float));
}

Matrix::Matrix(float a[4][4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = a[i][j];
		}
	}
}

void Matrix::Print()const
{
	for (int i = 0; i < 4; i++)
	{
		cout << "[";
		for (int j = 0; j < 3; j++)
		{
			cout << m[i][j] << ",";
		}
		cout << m[i][3] << "]" << endl;
	}
}
void Matrix::SetIdentity()
{
	SetZero();
	for (int i = 0; i < 4; i++)
	{
		m[i][i] = 1;
	}
}

void Matrix::SetZero()
{
	memset(m, 0, 16 * sizeof(float));
}


void Matrix::SetTranslate(float x, float y, float z)
{
	SetIdentity();
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
}
//ÈÆÈÎÒâÖáË³Ê±ÕëÐý×ª
void Matrix::SetRotate(float x, float y, float z, float theta)
{
	float qsin = (float)sin(theta*0.5f);
	float qcos = (float)cos(theta*0.5f);
	Vector4 vec(x, y, z, 1);
	vec.Normalize();
	x = vec.x*qsin;
	y = vec.y*qsin;
	z = vec.z*qsin;
	float w = qcos;

	float mat[4][4] = { { 1 - 2 * (y*y + z*z), 2 * (x*y + w*z), 2 * (x*z - w*y), 0 },
						{ 2 * (x*y - w*z), 1 - 2 * (x*x + z*z), 2 * (y*z + w*x), 0 },
						{ 2 * (x*z + w*y), 2 * (y*z - w*x), 1 - 2 * (x*x + y*y), 0 },
						{ 0,					0,						0,		 1 }};
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = mat[i][j];
		}
	}
}
void Matrix::SetScale(float x, float y, float z)
{
	SetIdentity();
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
}
//Äæ¾ØÕóµÄ×ªÖÃ
Matrix Matrix::InverseTranspose()
{
	float d;
	Matrix invM=*this;

	for (int k =0; k <4; ++k)
	{
		d = 1.0 / invM.m[k][k];
		invM.m[k][k] = d;
		for (int i = 0; i < 4; ++i)
		{
			if (i != k)
				invM.m[k][i] *= -d;
		}
		for (int i = 0; i < 4; ++i)
		{
			if (i != k)
				invM.m[i][k] *= d;
		}
		for (int i = 0; i < 4; ++i)
		{
			if (i != k)
			{
				for (int j = 0; j <4; ++j)
				{
					if (j != k)
						invM.m[i][j] += invM.m[i][k] * invM.m[k][j] / d;
				}
			}
		}
	}
	Matrix result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = invM.m[j][i];
		}
	}

	return result;
}

//Äæ¾ØÕó
Matrix Matrix::Inverse()
{
	float d;
	Matrix invM = *this;

	for (int k = 0; k <4; ++k)
	{
		d = 1.0 / invM.m[k][k];
		invM.m[k][k] = d;
		for (int i = 0; i < 4; ++i)
		{
			if (i != k)
				invM.m[k][i] *= -d;
		}
		for (int i = 0; i < 4; ++i)
		{
			if (i != k)
				invM.m[i][k] *= d;
		}
		for (int i = 0; i < 4; ++i)
		{
			if (i != k)
			{
				for (int j = 0; j <4; ++j)
				{
					if (j != k)
						invM.m[i][j] += invM.m[i][k] * invM.m[k][j] / d;
				}
			}
		}
	}
	return invM;
}
Matrix Matrix ::operator+(const Matrix& right)const
{
	Matrix result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m[i][j] + right.m[i][j];
		}
	}
	return result;
}

Matrix Matrix ::operator-(const Matrix& right)const
{
	Matrix result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m[i][j] - right.m[i][j];
		}
	}
	return result;
}

Matrix Matrix ::operator*(const Matrix& right)const
{
	Matrix result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m[i][0] * right.m[0][j] + m[i][1] * right.m[1][j] + m[i][2] * right.m[2][j] + m[i][3] * right.m[3][j];
		}
	}
	return result;
}

Matrix Matrix::operator*(float right)const
{
	Matrix result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m[i][j] * right;
		}
	}
	return result;
}
Matrix& Matrix::operator = (const Matrix& right)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] = right.m[i][j];
		}
	}
	return *this;
}

bool Matrix::operator == (const Matrix& right) const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (m[i][j] != right.m[i][j]) return false;
		}
	}
	return true;
}