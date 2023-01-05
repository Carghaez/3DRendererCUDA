#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Point.h"
#include "Renderer.h"
#include "Triangle.h"

Renderer renderer;

float faseA = 0;
float faseB = 0;

void drawCube(void (Renderer::*setColor)(char, char, char), int color_shift)
{
	// back
	{
		static Triangle tr1 = { Point3(-1, -1, 1), Point3(1, -1, 1), Point3(1, 1, 1) };
		static Triangle tr2 = { Point3(-1, -1, 1), Point3(1, 1, 1), Point3(-1, 1, 1) };
		(renderer.*setColor)(255 >> color_shift, 0, 255 >> color_shift);
		renderer.drawTriangle(tr1);
		renderer.drawTriangle(tr2);
	}

	// front
	{
		static Triangle tr1 = { Point3(-1, -1, -1), Point3(1, -1, -1), Point3(1, 1, -1) };
		static Triangle tr2 = { Point3(-1, -1, -1), Point3(1, 1, -1), Point3(-1, 1, -1) };
		(renderer.*setColor)(128 >> color_shift, 128 >> color_shift, 128 >> color_shift);
		renderer.drawTriangle(tr1);
		renderer.drawTriangle(tr2);
	}

	// left
	{
		static Triangle tr1 = { Point3(-1, -1, -1), Point3(-1, 1, -1), Point3(-1, 1, 1) };
		static Triangle tr2 = { Point3(-1, -1, -1), Point3(-1, 1, 1), Point3(-1, -1, 1) };
		(renderer.*setColor)(0, 0, 255 >> color_shift);
		renderer.drawTriangle(tr1);
		renderer.drawTriangle(tr2);
	}

	// right
	{
		static Triangle tr1 = { Point3(1, -1, -1), Point3(1, 1, -1), Point3(1, 1, 1) };
		static Triangle tr2 = { Point3(1, -1, -1), Point3(1, 1, 1), Point3(1, -1, 1) };
		(renderer.*setColor)(0, 255 >> color_shift, 255 >> color_shift);
		renderer.drawTriangle(tr1);
		renderer.drawTriangle(tr2);
	}

	// bottom
	{
		static Triangle tr1 = { Point3(-1, -1, -1), Point3(-1, -1, 1), Point3(1, -1, 1) };
		static Triangle tr2 = { Point3(-1, -1, -1), Point3(1, -1, 1), Point3(1, -1, -1) };
		(renderer.*setColor)(255 >> color_shift, 128 >> color_shift, 0);
		renderer.drawTriangle(tr1);
		renderer.drawTriangle(tr2);
	}

	// top
	{
		static Triangle tr1 = { Point3(-1, 1, -1), Point3(-1, 1, 1), Point3(1, 1, 1) };
		static Triangle tr2 = { Point3(-1, 1, -1), Point3(1, 1, 1), Point3(1, 1, -1) };
		(renderer.*setColor)(255 >> color_shift, 0, 0);
		renderer.drawTriangle(tr1);
		renderer.drawTriangle(tr2);
	}
}

void showGlutString(const char *c)
{
	while (*c)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c++);
}

void Display()
{
	renderer.resetMatrixStack();

	renderer.setFillColor(255, 255, 255);

	renderer.clearScreen();
	renderer.multMatrix(Matrix::translate(0, 0, 10) * Matrix::perspective(5));

	renderer.multMatrix(Matrix::rotate(faseA, 0, 1, 0));
	renderer.multMatrix(Matrix::rotate(faseB, 1, 0, 0));
	drawCube((renderer.getDrawMode() == Renderer::Solid) ? &Renderer::setFillColor : &Renderer::setStrokeColor, 0);

	renderer.multMatrix(Matrix::translate(3, 0, 0));
	renderer.multMatrix(Matrix::scale(.5, .5, .5));
	renderer.multMatrix(Matrix::rotate(faseA, 0, 1, 0));
	renderer.multMatrix(Matrix::rotate(faseB, 1, 0, 0));
	drawCube((renderer.getDrawMode() == Renderer::Solid) ? &Renderer::setFillColor : &Renderer::setStrokeColor, 1);

	renderer.copyToScreen();

	glRasterPos2f(-1, -.95);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	showGlutString("Tasti:   [W]ireframe     [S]olid     Wireframe con qualcosa che somiglia a [H]idden Line Removal");

	glutSwapBuffers();
}

void Reshape(GLint width, GLint height)
{
	renderer.resize(width, height);
}

void MouseButton(int button, int state, int x, int y)
{
}

void MouseMotion(int x, int y)
{
}

void AnimateScene(int)
{
  faseA += .02;
  faseB += .004;

  // Force redraw
  glutPostRedisplay();
  glutTimerFunc(1000/60, AnimateScene, 0);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'W':
		case 'w':
			renderer.setDrawMode(Renderer::Wireframe);
			break;
		case 'S':
		case 's':
			renderer.setDrawMode(Renderer::Solid);
			break;
		case 'H':
		case 'h':
			renderer.setDrawMode(Renderer::WireframeHiddenLineRemoval);
			break;
	}
}

int main(int argc, char** argv)
{
	// GLUT Window Initialization
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("GLUT example");

	renderer.resize(800, 600);

	// Register callbacks
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	//glutIdleFunc(AnimateScene);
	glutTimerFunc(100, AnimateScene, 0);

	// Turn the flow of control over to GLUT
	glutMainLoop();

	return 0;
}
