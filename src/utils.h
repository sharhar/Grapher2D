#pragma once

void getModelviewMatrix(float* vals, float x, float y, float w, float h);
void getModelviewMatrixR(float* vals, float x, float y, float w, float h, float r);
void getOrthoMatrix(float* vals, float left, float right, float bottom, float top, float near, float far);