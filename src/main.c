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
	
	char* eq = "tan(x)^tan(x)=y^tan(x+t)";
	char* error = NULL;

	char* ffeq = "";
	char* feq = "";
	
	eqConvert(parseInfo, eq, &feq, &ffeq, &error);

	printf("eq    = %s\nfeq   = %s\nffeq  = %s\nerror = %s\n", eq, feq, ffeq, error);

	swMakeContextCurrent(glView);

	if (!gladLoadGLLoader((GLADloadproc)swGetProcAddress)) {
		printf("Could not load opengl!\n");
		return 0;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	GLMesh* quad = createQuadMesh();
	GLShader* quadShader = createQuadShader();
	GLShader* lineShader = createLineShader();
	GLShader* calcShader = createCalcShader(feq, ffeq);
	GLShader* edgeShader = createEdgeShader();
	GLShader* renderShader = createRenderShader();

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

	GLuint dfbo, dtex, efbo, etex, g_fbo, g_gtex;

	glGenFramebuffers(1, &dfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &dtex);
	glBindTexture(GL_TEXTURE_2D, dtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 1200, 1200,
		0, GL_RG, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &efbo);
	glBindFramebuffer(GL_FRAMEBUFFER, efbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &etex);
	glBindTexture(GL_TEXTURE_2D, etex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 1200, 1200,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, etex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &g_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &g_gtex);
	glBindTexture(GL_TEXTURE_2D, g_gtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		GRAPH_PORT_SIZE, GRAPH_PORT_SIZE,
		0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_gtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float time = 0;

	while (!swCloseRequested(window)) {
		swPollEvents();

		mouseUpdate(mouseState, &preMouseState, glViewBounds);

		glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);

		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, 1200, 1200);

		glBindVertexArray(quad->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glUseProgram(lineShader->program);
		
		glUniform3f(lineShader->uniformLocs[2], 0.45f, 0.45f, 0.45f);
		drawGrid(lineModel, lineShader->uniformLocs[1], g_left, g_right, g_down, g_up, 600, 600);
		glUniform3f(lineShader->uniformLocs[2], 0, 0, 0);
		drawAxes(lineModel, lineShader->uniformLocs[1], 600, 600);

		glBindFramebuffer(GL_FRAMEBUFFER, dfbo);

		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, 1200, 1200);

		glUseProgram(calcShader->program);

		glUniform1f(calcShader->uniformLocs[0], g_up);
		glUniform1f(calcShader->uniformLocs[1], g_down);
		glUniform1f(calcShader->uniformLocs[2], g_left);
		glUniform1f(calcShader->uniformLocs[3], g_right);
		glUniform1f(calcShader->uniformLocs[4], time);
		glUniform1f(calcShader->uniformLocs[5], 0);

		time += 1.0f / 60.0f;

		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		glBindFramebuffer(GL_FRAMEBUFFER, efbo);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, 1200, 1200);

		glUseProgram(edgeShader->program);
		glBindTexture(GL_TEXTURE_2D, dtex);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);

		glViewport(0, 0, GRAPH_PORT_SIZE, GRAPH_PORT_SIZE);

		glUseProgram(renderShader->program);
		glUniform3f(renderShader->uniformLocs[1], 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, etex);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 600, 600);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(quadShader->program);
		glBindTexture(GL_TEXTURE_2D, g_gtex);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//printf("x: %f\ny: %f\nl: %d\n", mouseState->x, mouseState->y, mouseState->ldown);

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}
