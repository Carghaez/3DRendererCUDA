#ifndef MATRIX_H
#define MATRIX_H

class Point4;

struct Matrix
{
	public:
		Matrix() {}

		static Matrix& identity();
		static Matrix perspective(float f);
		static Matrix translate(float x, float y, float z);
		static Matrix scale(float x, float y, float z);
		static Matrix rotate(float angle, float x, float y, float z);

		typedef float _row[4];
		inline _row& operator[](int row) { return d[row]; }
		inline const _row& operator[](int row) const { return d[row]; }

	private:
		Matrix(float m00, float m01, float m02, float m03,
		       float m10, float m11, float m12, float m13,
		       float m20, float m21, float m22, float m23,
		       float m30, float m31, float m32, float m33);

		float d[4][4];
};

Matrix operator*(const Matrix &a, const Matrix &b);

#endif // MATRIX_H
