#include "utils.h"
#include "equation.h"
#include "glUtils.h"
#include <stdio.h>

void* malloc(size_t);

#define COLOR_NUM 5
#define ZOOM_PERCENT 0.025
#define GRAPH_PORT_SIZE 1200

double g_left = -6;
double g_right = 6;
double g_down = -6;
double g_up = 6;

int g_windowWidth = 600;
int g_windowHeight = 600;

Color g_colors[COLOR_NUM];

GLuint g_gtex;
GLuint g_fbo;

void submitCallback(Entry* entry) {
	printf("%d: %s\n", entry->ID, swGetTextFromTextField(entry->textField));
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

	SRect* glViewBounds = swMakeRect(390, 10, 600, 600);
	SOpenGLView* glView = swCreateOpenGLView(rootView, glViewBounds, &attribs);

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

	SMouseState preMouseState;
	preMouseState.x = 0;
	preMouseState.y = 0;
	preMouseState.ldown = 0;
	SMouseState* mouseState = swGetMouseState(window);

	while (!swCloseRequested(window)) {
		swPollEvents();

		mouseUpdate(mouseState, &preMouseState, glViewBounds);

		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(quad->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glUseProgram(lineShader->program);
		
		glUniform3f(lineShader->uniformLocs[2], 0.45f, 0.45f, 0.45f);
		drawGrid(lineModel, lineShader->uniformLocs[1], g_left, g_right, g_down, g_up, 600, 600);
		glUniform3f(lineShader->uniformLocs[2], 0, 0, 0);
		drawAxes(lineModel, lineShader->uniformLocs[1], 600, 600);

		//printf("x: %f\ny: %f\nl: %d\n", mouseState->x, mouseState->y, mouseState->ldown);

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}
