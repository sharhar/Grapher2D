#pragma once

#include <swin/SWin.h>
#include <glad/glad.h>
#include <math.h>
#include <string.h>

extern double g_left;
extern double g_right;
extern double g_down;
extern double g_up;

extern int g_windowWidth;
extern int g_windowHeight;

typedef struct Entry {
	STextField* textField;
	SButton* submitButton;
	SButton* deleteButton;

	int ID;
	struct Entry* next;
} Entry;

typedef struct UIState {
	SView* rootView;
	int y;
	int first;

	Entry* root;
	Entry* last;
} UIState;

typedef struct Color {
	float r, g, b;
} Color;

void getModelviewMatrix(float* vals, float x, float y, float w, float h);
void getModelviewMatrixR(float* vals, float x, float y, float w, float h, float r);
void getOrthoMatrix(float* vals, float left, float right, float bottom, float top, float near, float far);
double abs_c(double n1);
void createEntry(UIState* uiState, int ID);
void drawAxes(float* modelMat, GLuint modelLoc, int width, int height);
void drawGrid(float* modelMat, GLuint modelLoc, double xl, double xr, double yd, double yu, int width, int height);
void mouseUpdate(SMouseState* state, SMouseState* preState, SRect* bounds);
void submitCallback(Entry* entry);