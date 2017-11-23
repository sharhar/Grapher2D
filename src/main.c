#include "stdhdr.h"
#include "utils.h"
#include "equation.h"
#include "glUtils.h"
#include <stdio.h>

void* malloc(size_t);

#define COLOR_NUM 5
#define ZOOM_PERCENT 0.025
#define GRAPH_PORT_SIZE 1200

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

double g_left = -6;
double g_right = 6;
double g_down = -6;
double g_up = 6;

int g_windowWidth = 600;
int g_windowHeight = 600;

Color g_colors[COLOR_NUM];

GLuint g_gtex;
GLuint g_fbo;

inline double abs_c(double n1) {
	return n1 > 0 ? n1 : -(n1);
}

void submitCallback(Entry* entry) {
	printf("%d: %s\n", entry->ID, swGetTextFromTextField(entry->textField));
}

void createEntry(UIState* uiState, int ID) {
	Entry* entry = malloc(sizeof(Entry));
	entry->textField = swCreateTextField(uiState->rootView, swMakeRect(10, 580 - uiState->y * 40, 285, 30), "");
	entry->submitButton = swCreateButton(uiState->rootView, swMakeRect(305, 580 - uiState->y * 40, 75, 30), "Submit", &submitCallback, entry);
	entry->deleteButton = NULL;
	entry->ID = ID;
	entry->next = NULL;

	if (uiState->first) {
		uiState->root = entry;
		uiState->last = entry;
		uiState->first = 0;
	} else {
		uiState->last->next = entry;
		uiState->last = entry;
	}

	uiState->y++;
}

void drawAxes(float* modelMat, GLuint modelLoc, int width, int height) {
	double l = abs_c(g_left);
	double r = abs_c(g_right);

	double xoff = (l / (r + l))* width;

	if (g_left < 0 && g_right > 0) {
		getModelviewMatrix(modelMat, xoff, 300, 1, 600);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	double d = abs_c(g_down);
	double u = abs_c(g_up);

	double yoff = (d / (u + d))* height;

	if (g_up > 0 && g_down < 0) {
		getModelviewMatrix(modelMat, 300, yoff, 600, 1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawGrid(float* modelMat, GLuint modelLoc, double xl, double xr, double yd, double yu, int width, int height) {
	double xratio = width / (xr - xl);

	double xlen = xr - xl;
	double xlog = log10(xlen);
	double xfloor = floor(xlog);
	double xmag = pow(10, xfloor);

	if (floor(xlog - 0.25) < xfloor) {
		xmag /= 2;
	}

	if (floor(xlog - 0.5) < xfloor) {
		xmag /= 2;
	}

	double xlr = round(xl / xmag)*xmag;
	double xrr = round(xr / xmag)*xmag;

	double xNum = (xrr - xlr) / xmag;
	for (int i = 0; i < xNum + 1;i++) {
		double tx = (xlr - xl + xmag*i)*xratio;

		if (tx  < 0 || tx > width) {
			continue;
		}

		getModelviewMatrix(modelMat, tx, 300, 0.25f, 600);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	double yratio = height / (yu - yd);

	double ylen = yu - yd;
	double ylog = log10(ylen);
	double yfloor = floor(ylog);
	double ymag = pow(10, yfloor);

	if (floor(ylog - 0.25) < yfloor) {
		ymag /= 2;
	}

	if (floor(ylog - 0.5) < yfloor) {
		ymag /= 2;
	}

	double ylr = round(yd / ymag)*ymag;
	double yrr = round(yu / ymag)*ymag;

	double yNum = (yrr - ylr) / ymag;

	for (int i = 0; i < yNum * 2; i++) {
		double ty = (ylr - yd + ymag*i)*yratio;

		if (ty < 0 || ty > height) {
			continue;
		}
		getModelviewMatrix(modelMat, 300, ty, 600, 0.25f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

int main() {
	swInit();

	SWindow* window = swCreateWindow(1000, 620, "Grapher2D");
	SView* rootView = swGetRootView(window);

	SOpenGLContextAttribs attribs;
	attribs.major = 3;
	attribs.minor = 3;
	attribs.debug = 0;
	attribs.swapInterval = 1;

	SOpenGLView* glView = swCreateOpenGLView(rootView, swMakeRect(390, 10, 600, 600), &attribs);

	UIState* state = malloc(sizeof(UIState));
	state->y = 0;
	state->first = 1;
	state->rootView = rootView;

	for (int i = 0; i < 15; i++) {
		createEntry(state, i);
	}

	ParsingInfo* parseInfo = eqGetDefaultParseInfo();
	
	char* eq = "integral(n, 0, 1, 10, sigma(k, 0, 10, n*k))";
	char* error = NULL;

	char* ffeq = "";
	char* feq = "";
	
	eqConvert(parseInfo, eq, &feq, &ffeq, &error);

	//printf("eq    = %s\nfeq   = %s\nffeq  = %s\nerror = %s\n", eq, feq, ffeq, error);

	swMakeContextCurrent(glView);

	if (!gladLoadGLLoader((GLADloadproc)swGetProcAddress)) {
		printf("Could not load opengl!\n");
		return 0;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	GLMesh* quad = createQuadMesh();
	GLShader* quadShader = createQuadShader();
	GLShader* lineShader = createLineShader();

	float* lineModel = malloc(sizeof(float) * 16);
	getModelviewMatrix(lineModel, 0, 0, 1, 1);

	float* lineProj = malloc(sizeof(float) * 16);
	getOrthoMatrix(lineProj, 0, 600, 0, 600, -1, 1);

	glUseProgram(lineShader->program);

	glUniformMatrix4fv(lineShader->uniformLocs[0], 1, GL_FALSE, lineProj);
	glUniformMatrix4fv(lineShader->uniformLocs[1], 1, GL_FALSE, lineModel);

	glUseProgram(0);

	while (!swCloseRequested(window)) {
		swPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(quad->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glUseProgram(lineShader->program);

		glUniform3f(lineShader->uniformLocs[2], 0.45f, 0.45f, 0.45f);
		drawGrid(lineModel, lineShader->uniformLocs[1], g_left, g_right, g_down, g_up, 600, 600);
		glUniform3f(lineShader->uniformLocs[2], 0, 0, 0);
		drawAxes(lineModel, lineShader->uniformLocs[1], 600, 600);

		//glDrawArrays(GL_TRIANGLES, 0, 6);

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}
