#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <GL/gl.h>
#include <cuda_runtime.h>

#include "Point.h"
#include "Renderer.h"
#include "Triangle.h"

static __global__ void clearBuffersK(char *colorbuffer, float *depthbuffer,
	int npixels, char r, char g, char b)
{
	const int i = threadIdx.x + blockIdx.x * blockDim.x;

	if (i < npixels)
	{
		char *pixel = colorbuffer + i * 3;
		*pixel++ = r;
		*pixel++ = g;
		*pixel = b;

		depthbuffer[i] = INFINITY;
	}
}

static __global__ void drawLineK(char *colorbuffer, float *depthbuffer, float subdivisions,
	float dx, float dy, float dz, float x1, float y1, float z1,
	int w, char r, char g, char b)
{
	const float pos = (threadIdx.x + blockIdx.x * blockDim.x) / (subdivisions - 1);

	if (pos <= 1)
	{
		const int x = roundf(x1 + dx * pos);
		const int y = roundf(y1 + dy * pos);
		const float z = z1 + dz * pos;

		const int i = x + y * w;

		if (depthbuffer[i] > z)
		{
			char *pixel = colorbuffer + i * 3;
			*pixel++ = r;
			*pixel++ = g;
			*pixel = b;

			depthbuffer[i] = z;
		}
	}
}

static __global__ void drawTriangleK(char *colorbuffer, float *depthbuffer,
	int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2,
	int min_x, int min_y, int w, char r, char g, char b)
{
	const int x = min_x + threadIdx.x + blockIdx.x * blockDim.x;
	const int y = min_y + threadIdx.y + blockIdx.y * blockDim.y;

	// Controlla che il punto sia interno ai tre lati del triangolo
	const float prodscal01 = (x1-x0)*(y-y0) - (y1-y0)*(x-x0);
	const float prodscal12 = (x2-x1)*(y-y1) - (y2-y1)*(x-x1);
	const float prodscal20 = (x0-x2)*(y-y2) - (y0-y2)*(x-x2);
	if ( (prodscal01 <= 0 && prodscal12 <= 0 && prodscal20 <= 0) ||
		(prodscal01 >= 0 && prodscal12 >= 0 && prodscal20 >= 0) )
	{
		const int i = x + y * w;
		const float z = (((x0-x)*y1+(x-x1)*y0+(x1-x0)*y)*z2+((x-x0)*y2+(x2-x)*y0+(x0-x2)*y)*z1+((x1-x)*y2+(x-x2)*y1+(x2-x1)*y)*z0)/((x1-x0)*y2+(x0-x2)*y1+(x2-x1)*y0);

		if (depthbuffer[i] > z)
		{
			char *pixel = colorbuffer + i * 3;
			*pixel++ = r;
			*pixel++ = g;
			*pixel = b;

			depthbuffer[i] = z;
		}
	}
}

Renderer::Renderer()
: buffer_w(0), buffer_h(0), colorbuffer(0), drawMode(Solid),
  stroke_r(0), stroke_g(0), stroke_b(0),
  fill_r(0), fill_g(0), fill_b(0)
{
}

void Renderer::resize(int w, int h)
{
	fprintf(stderr, "Buffer ridimensionato a %dx%d\n", w, h);

	if (colorbuffer != 0)
	{
		cudaFree(colorbuffer);
		cudaFree(depthbuffer);
	}

	cudaError_t err = cudaMalloc(&colorbuffer, w * h * 3);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "cudaMalloc (colorbuffer) fallito: %d - %s\n", err, cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	err = cudaMalloc(&depthbuffer, w * h * sizeof(float));
	if (err != cudaSuccess)
	{
		fprintf(stderr, "cudaMalloc (depthbuffer) fallito: %d - %s\n", err, cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	buffer_w = w;
	buffer_h = h;
}

void Renderer::copyToScreen()
{
	const int buffsize = buffer_w * buffer_h * 3;
	char * temp = (char*)malloc(buffsize);
	if (temp == 0)
	{
		fprintf(stderr, "memoria esaurita in copyToScreen\n");
		exit(EXIT_FAILURE);
	}

	cudaError_t err = cudaMemcpy(temp, colorbuffer, buffsize, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		fprintf(stderr, "cudaMemcpy fallito: %d - %s\n", err, cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	glRasterPos2i(-1, -1);
	glDrawPixels(buffer_w, buffer_h, GL_RGB, GL_UNSIGNED_BYTE, temp);

	free(temp);
}

void Renderer::resetMatrixStack()
{
	mtxStack.clear();
	currMatrix = Matrix::scale(buffer_h / 4.f, buffer_h / 4.f, 1) * Matrix::translate(buffer_w / 2.f, buffer_h / 2.f, 0);
}

void Renderer::pushMatrix()
{
	mtxStack.push_back(currMatrix);
}

void Renderer::popMatrix()
{
	currMatrix = mtxStack.back();
	mtxStack.pop_back();
}

void Renderer::multMatrix(const Matrix &mtx)
{
	currMatrix = mtx * currMatrix;
}

void Renderer::drawTriangle(const Triangle &t)
{
	if (drawMode == Solid || drawMode == WireframeHiddenLineRemoval)
	{
		drawTriangleNoTransform(
			Point4(t.vA) * currMatrix,
			Point4(t.vB) * currMatrix,
			Point4(t.vC) * currMatrix,
			fill_r, fill_g, fill_b);
	}

	if (drawMode == Wireframe || drawMode == WireframeHiddenLineRemoval)
	{
		drawLine(t.vA, t.vB);
		drawLine(t.vB, t.vC);
		drawLine(t.vC, t.vA);
	}
}

void Renderer::setDrawMode(DrawMode newMode)
{
	drawMode = newMode;
}

Renderer::DrawMode Renderer::getDrawMode() const
{
	return drawMode;
}

void Renderer::setStrokeColor(char r, char g, char b)
{
	stroke_r = r;
	stroke_g = g;
	stroke_b = b;
}

void Renderer::setFillColor(char r, char g, char b)
{
	fill_r = r;
	fill_g = g;
	fill_b = b;
}

void Renderer::drawLine(const Point3 &p1, const Point3 &p2)
{
	drawLineNoTransform(Point4(p1) * currMatrix, Point4(p2) * currMatrix);
}

Renderer::~Renderer()
{
	if (colorbuffer != 0)
	{
		cudaFree(colorbuffer);
		cudaFree(depthbuffer);
	}
}

void Renderer::clearScreen()
{
	const int npixels = buffer_w * buffer_h;

	const int blockSize = 32;
	const int numBlocks = (npixels + blockSize - 1) / blockSize;

	clearBuffersK<<<numBlocks, blockSize>>>(colorbuffer, depthbuffer,
		npixels, fill_r, fill_g, fill_b);

	cudaError_t err = cudaThreadSynchronize();
	if (err != cudaSuccess)
	{
		fprintf(stderr, "clearBuffersK fallito: %d - %s\n", err, cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

void Renderer::drawLineNoTransform(const Point4 &p1, const Point4 &p2)
{
	const int x1 = roundf(p1.x / p1.t);
	const int y1 = roundf(p1.y / p1.t);
	const int x2 = roundf(p2.x / p2.t);
	const int y2 = roundf(p2.y / p2.t);

	const int subdivisions = 2 * ceil(sqrt(pow(x2-x1, 2) + pow(y2-y1, 2)));
	const float dx = (x2 - x1);
	const float dy = (y2 - y1);
	const float dz = (p2.z - p1.z);

	const int blockSize = 32;
	const int numBlocks = (subdivisions + blockSize - 1) / blockSize;

	drawLineK<<<numBlocks, blockSize>>>(colorbuffer, depthbuffer, subdivisions,
		dx, dy, dz, x1, y1, p1.z, buffer_w,
		stroke_r, stroke_g, stroke_b);

	cudaError_t err = cudaThreadSynchronize();
	if (err != cudaSuccess)
	{
		fprintf(stderr, "drawLineK fallito: %d - %s\n", err, cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

void Renderer::drawTriangleNoTransform(const Point4 &p0, const Point4 &p1,
	const Point4 &p2, char r, char g, char b)
{
	const int x0 = roundf(p0.x / p0.t);
	const int y0 = roundf(p0.y / p0.t);
	const float z0 = p0.z;
	const int x1 = roundf(p1.x / p1.t);
	const int y1 = roundf(p1.y / p1.t);
	const float z1 = p1.z;
	const int x2 = roundf(p2.x / p2.t);
	const int y2 = roundf(p2.y / p2.t);
	const float z2 = p2.z;

	// Calcola bounding box
	const float min_x = min(min(x0, x1), x2);
	const float max_x = max(max(x0, x1), x2);
	const float min_y = min(min(y0, y1), y2);
	const float max_y = max(max(y0, y1), y2);

	dim3 blockSize;
	blockSize.x = 16;
	blockSize.y = 16;

	dim3 numBlocks;
	numBlocks.x = ( (max_x - min_x + 1) + blockSize.x - 1 ) / blockSize.x;
	numBlocks.y = ( (max_y - min_y + 1) + blockSize.y - 1 ) / blockSize.y;

	drawTriangleK<<<numBlocks, blockSize>>>(colorbuffer, depthbuffer,
		x0, y0, z0, x1, y1, z1, x2, y2, z2,
		min_x, min_y, buffer_w, r, g, b);
}
