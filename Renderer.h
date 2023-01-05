#ifndef RENDERER_H
#define RENDERER_H

#include <list>

class Point4;
class Triangle;

#include "Matrix.h"

class Renderer
{
	public:
		Renderer();
		~Renderer();
		void resize(int w, int h);

		enum DrawMode
		{
			Wireframe,
			WireframeHiddenLineRemoval,
			Solid
		};

		void copyToScreen();

		void resetMatrixStack();
		void pushMatrix();
		void popMatrix();
		void multMatrix(const Matrix &mtx);

		void setStrokeColor(char r, char g, char b);
		void drawLine(const Point3 &p1, const Point3 &p2);

		void setDrawMode(DrawMode newMode);
		DrawMode getDrawMode() const;

		void setFillColor(char r, char g, char b);
		void clearScreen();
		void drawTriangle(const Triangle &triangle);

	private:
		void drawLineNoTransform(const Point4 &p1, const Point4 &p2);
		void drawTriangleNoTransform(const Point4 &vA, const Point4 &vB,
			const Point4 &vC, char r, char g, char b);

		int buffer_w, buffer_h;
		char *colorbuffer; // buffer_w * buffer_h * 3 canali (RGB)
		float *depthbuffer; // buffer_w * buffer_h

		DrawMode drawMode;

		char stroke_r, stroke_g, stroke_b;
		char fill_r, fill_g, fill_b;

		std::list<Matrix> mtxStack;
		Matrix currMatrix;
};

#endif // RENDERER_H
