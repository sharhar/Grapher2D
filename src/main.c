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
ParsingInfo* g_parseInfo;

int g_windowWidth = 600;
int g_windowHeight = 600;

Color g_colors[COLOR_NUM];

GLuint g_gtex;
GLuint g_fbo;

void submitCallback(Entry* entry) {
	//printf("%d: %s\n", entry->ID, swGetTextFromTextField(entry->textField));
	char* text = swGetTextFromTextField(entry->textField);
	if (strlen(text) == 0) {
		if (entry->graph != NULL) {
			deleteGraph(entry->graph);
		}
		
		entry->graph = NULL;
		entry->active = 0;
	}
	else {
		if (entry->graph != NULL) {
			deleteGraph(entry->graph);
		}

		char* error = NULL;

		char* ffeq = "";
		char* feq = "";
		eqConvert(g_parseInfo, text, &feq, &ffeq, &error);
		printf("eq    = %s\nfeq   = %s\nffeq  = %s\nerror = %s\n", text, feq, ffeq, error);
		entry->graph = createGraph(feq, ffeq, GRAPH_PORT_SIZE, GRAPH_PORT_SIZE);
		free(error);
		free(feq);
		free(ffeq);
		entry->active = 1;
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

	SRect* glViewBounds = swMakeRect(390, 10, 600, 600);
	SOpenGLView* glView = swCreateOpenGLView(rootView, glViewBounds, &attribs);

	UIState* state = malloc(sizeof(UIState));
	state->y = 0;
	state->first = 1;
	state->rootView = rootView;

	for (int i = 0; i < 15; i++) {
		createEntry(state, i);
	}

	g_parseInfo = eqGetDefaultParseInfo();
	g_colors[0].r = 0.8f;
	g_colors[0].g = 0.2f;
	g_colors[0].b = 0.2f;

	g_colors[1].r = 0.1f;
	g_colors[1].g = 0.6f;
	g_colors[1].b = 0.1f;

	g_colors[2].r = 0.2f;
	g_colors[2].g = 0.2f;
	g_colors[2].b = 0.9f;

	g_colors[3].r = 0.9f;
	g_colors[3].g = 0.65f;
	g_colors[3].b = 0.2f;

	g_colors[4].r = 0.8f;
	g_colors[4].g = 0.2f;
	g_colors[4].b = 0.8f;

	swMakeContextCurrent(glView);

	if (!gladLoadGLLoader((GLADloadproc)swGetProcAddress)) {
		printf("Could not load opengl!\n");
		return 0;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	GLMesh* quad = createQuadMesh();
	GLShader* quadShader = createQuadShader();
	GLShader* lineShader = createLineShader();
	GLShader* edgeShader = createEdgeShader(GRAPH_PORT_SIZE);
	GLShader* renderShader = createRenderShader(GRAPH_PORT_SIZE);

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
	preMouseState.scroll = 0;
	SMouseState* mouseState = swGetMouseState(window);

	GLuint g_fbo, g_gtex;

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
    
    int fps = 0;

	float time = 0;

	uint8_t i_color = 0;

	while (!swCloseRequested(window)) {
		swPollEvents();

		//printf("%d\n", fps);

		//fps++;

		mouseUpdate(mouseState, &preMouseState, glViewBounds);
		time += 1.0f / 60.0f;

		glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);

		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, GRAPH_PORT_SIZE, GRAPH_PORT_SIZE);

		glBindVertexArray(quad->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glUseProgram(lineShader->program);

		glUniform3f(lineShader->uniformLocs[2], 0.45f, 0.45f, 0.45f);
		drawGrid(lineModel, lineShader->uniformLocs[1], g_left, g_right, g_down, g_up, 600, 600);
		glUniform3f(lineShader->uniformLocs[2], 0, 0, 0);
		drawAxes(lineModel, lineShader->uniformLocs[1], 600, 600);

		Entry* currentEntry = state->root;
		while (currentEntry != NULL) {
			if (currentEntry->active) {
				drawGraphData(currentEntry->graph, time, 0);
			}
			currentEntry = currentEntry->next;
		}

		glUseProgram(edgeShader->program);
		
		currentEntry = state->root;
		while (currentEntry != NULL) {
			if (currentEntry->active) {
				drawGraphEdges(currentEntry->graph);
			}
			currentEntry = currentEntry->next;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);

		glViewport(0, 0, GRAPH_PORT_SIZE, GRAPH_PORT_SIZE);

		glUseProgram(renderShader->program);

		i_color = 0;
		currentEntry = state->root;
		while (currentEntry != NULL) {
			if (currentEntry->active) {
				glUniform3f(renderShader->uniformLocs[1], g_colors[i_color % COLOR_NUM].r, g_colors[i_color % COLOR_NUM].g, g_colors[i_color % COLOR_NUM].b);
				glBindTexture(GL_TEXTURE_2D, currentEntry->graph->etex);
				glActiveTexture(GL_TEXTURE0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			i_color++;
			currentEntry = currentEntry->next;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 600, 600);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(quadShader->program);
		glBindTexture(GL_TEXTURE_2D, g_gtex);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}
