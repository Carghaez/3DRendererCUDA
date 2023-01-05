#ifndef POINT_H
#define POINT_H

class Matrix;

struct Point3
{
	float x, y, z;

	Point3(float x, float y, float z)
	: x(x), y(y), z(z) {}
};

struct Point4 : public Point3
{
	float t;

	Point4(float x, float y, float z, float t = 1)
	: Point3(x, y, z), t(t) {}

	explicit Point4(const Point3 &p, float t = 1)
	: Point3(p), t(t) {}

	Point4 operator*(const Matrix &m) const;
};

#endif // POINT_H
