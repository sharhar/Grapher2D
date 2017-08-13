#include "utils.h"
#include <math.h>

#define DEG_TO_RAD 0.0174532925199

void getModelviewMatrix(float* vals, float x, float y, float w, float h) {
	vals[0] = w;
	vals[1] = 0;
	vals[2] = 0;
	vals[3] = 0;

	vals[4] = 0;
	vals[5] = h;
	vals[6] = 0;
	vals[7] = 0;

	vals[8] = 0;
	vals[9] = 0;
	vals[10] = 1;
	vals[11] = 0;

	vals[12] = x;
	vals[13] = y;
	vals[14] = 0;
	vals[15] = 1;
}

void getModelviewMatrixR(float* vals, float x, float y, float w, float h, float r) {
	float c = cos(r*DEG_TO_RAD);
	float s = sin(r*DEG_TO_RAD);

	vals[0] = w*c;
	vals[1] = w*s;
	vals[2] = 0;
	vals[3] = 0;

	vals[4] = -h*s;
	vals[5] = h*c;
	vals[6] = 0;
	vals[7] = 0;

	vals[8] = 0;
	vals[9] = 0;
	vals[10] = 1;
	vals[11] = 0;

	vals[12] = x;
	vals[13] = y;
	vals[14] = 0;
	vals[15] = 1;
}

void getOrthoMatrix(float* vals, float left, float right, float bottom, float top, float near, float far) {
	vals[0] = 2.0f / (right - left);
	vals[1] = 0;
	vals[2] = 0;
	vals[3] = 0;

	vals[4] = 0;
	vals[5] = 2.0f / (top - bottom);
	vals[6] = 0;
	vals[7] = 0;

	vals[8] = 0;
	vals[9] = 0;
	vals[10] = -2.0f / (far - near);
	vals[11] = 0;

	vals[12] = -(right + left) / (right - left);
	vals[13] = -(top + bottom) / (top - bottom);
	vals[14] = -(far + near) / (far - near);
	vals[15] = 1;
}