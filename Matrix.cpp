#include <cmath>

#include "Matrix.h"

Matrix& Matrix::identity()
{
	static Matrix m(1, 0, 0, 0,
	                0, 1, 0, 0,
	                0, 0, 1, 0,
	                0, 0, 0, 1);
	return m;
}

Matrix Matrix::perspective(float f)
{
	Matrix m(1, 0, 0, 0,
	         0, 1, 0, 0,
	         0, 0, 1, 1/f,
	         0, 0, 0, 0);
	return m;
}

Matrix Matrix::translate(float x, float y, float z)
{
	Matrix m(1, 0, 0, 0,
	         0, 1, 0, 0,
	         0, 0, 1, 0,
	         x, y, z, 1);
	return m;
}

Matrix Matrix::scale(float x, float y, float z)
{
	Matrix m(x, 0, 0, 0,
	         0, y, 0, 0,
	         0, 0, z, 0,
	         0, 0, 0, 1);
	return m;
}

Matrix Matrix::rotate(float angle, float x, float y, float z)
{
	const float s = sinf(angle);
	const float c = cosf(angle);

	const float fatt = 1 / sqrtf(x*x + y*y + z*z);
	x *= fatt;
	y *= fatt;
	z *= fatt;

	Matrix m(x*x*(1-c)+c,   y*x*(1-c)+z*s, z*x*(1-c)-y*s, 0,
	         x*y*(1-c)-z*s, y*y*(1-c)+c,   z*y*(1-c)+x*s, 0,
	         x*z*(1-c)+y*s, y*z*(1-c)-x*s, z*z*(1-c)+c,   0,
	         0,             0,             0,             1);
	return m;
}

Matrix::Matrix(float m00, float m01, float m02, float m03,
		       float m10, float m11, float m12, float m13,
		       float m20, float m21, float m22, float m23,
		       float m30, float m31, float m32, float m33)
{
	d[0][0] = m00; d[0][1] = m01; d[0][2] = m02; d[0][3] = m03;
	d[1][0] = m10; d[1][1] = m11; d[1][2] = m12; d[1][3] = m13;
	d[2][0] = m20; d[2][1] = m21; d[2][2] = m22; d[2][3] = m23;
	d[3][0] = m30; d[3][1] = m31; d[3][2] = m32; d[3][3] = m33;
}

Matrix operator*(const Matrix &a, const Matrix &b)
{
	Matrix r;

	for (int i = 0; i < 4; ++i)
	{	
		for (int j = 0; j < 4; ++j)
		{
			r[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] +
				a[i][2] * b[2][j] + a[i][3] * b[3][j];
		}
	}

	return r;
}
