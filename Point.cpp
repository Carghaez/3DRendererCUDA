#include "Matrix.h"
#include "Point.h"

Point4 Point4::operator*(const Matrix &m) const
{
	Point4 p(x * m[0][0] + y * m[1][0] + z * m[2][0] + t * m[3][0],
		     x * m[0][1] + y * m[1][1] + z * m[2][1] + t * m[3][1],
		     x * m[0][2] + y * m[1][2] + z * m[2][2] + t * m[3][2],
		     x * m[0][3] + y * m[1][3] + z * m[2][3] + t * m[3][3]);
	return p;
}
