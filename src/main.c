#include "utils.h"
#include "equation.h"
#include "glUtils.h"
#include <stdio.h>
#include <stdlib.h>

#include <swin/SWin.h>
#include <VKL/VKL.h>

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

double g_time = 0;
SWindow* g_window = NULL;

Color g_colors[COLOR_NUM];

GLuint g_gtex;
GLuint g_fbo;

float g_posx = 0;
float g_posy = 0;

uint8_t g_mouseDown = 0;

void scrollCallback(float xpos, float ypos, float scroll) {
	double x1 = 0;
	double y1 = 0;
	double x2 = g_right - g_left;
	double y2 = g_up - g_down;
	
	double w1 = (g_posx / 600.0)*x2;
	double w2 = x2 - w1;
	
	if (scroll > 0) {
		x1 = 2 * ZOOM_PERCENT*w1;
		x2 = x2 - 2 * ZOOM_PERCENT*w2;
	}
	else if (scroll < 0) {
		x1 = -2 * ZOOM_PERCENT*w1;
		x2 = x2 + 2 * ZOOM_PERCENT*w2;
	}
	
	double h1 = ((600.0 - g_posy) / 600.0)*(y2);
	double h2 = y2 - ((600.0 - g_posy) / 600.0)*(y2);
	
	if (scroll > 0) {
		y1 = 2 * ZOOM_PERCENT*h1;
		y2 = y2 - 2 * ZOOM_PERCENT*h2;
	}
	else if (scroll < 0) {
		y1 = -2 * ZOOM_PERCENT*h1;
		y2 = y2 + 2 * ZOOM_PERCENT*h2;
	}
	
	g_right = g_left + x2;
	g_left = g_left + x1;
	g_up = g_down + y2;
	g_down = g_down + y1;
}

void mouseDownCallback(float xpos, float ypos, int button) {
	if(button == SWIN_MOUSE_BUTTON_LEFT) {
		g_mouseDown = 1;
	}
	
	g_posx = xpos;
	g_posy = ypos;
}

void mouseUpCallback(float xpos, float ypos, int button) {
	if(button == SWIN_MOUSE_BUTTON_LEFT) {
		g_mouseDown = 0;
	}
	
	g_posx = xpos;
	g_posy = ypos;
}

void mouseMovedCallback(float xpos, float ypos) {
	float dx = xpos - g_posx;
	float dy = ypos - g_posy;
	
	if (g_mouseDown) {
		double width = g_right - g_left;
		double percentX = dx / g_windowWidth;
		
		double moveX = -width * percentX;
		
		g_left += moveX;
		g_right += moveX;
		
		double height = g_up - g_down;
		double percentY = dy / g_windowHeight;
		
		double moveY = height * percentY;
		
		g_down += moveY;
		g_up += moveY;
	}
	
	g_posx = xpos;
	g_posy = ypos;
}

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
		char* error = NULL;

		char* ffeq = "";
		char* feq = "";
		eqConvert(g_parseInfo, text, &feq, &ffeq, &error);

		if (error != NULL) {
			//fiswPopup(g_window, "Error in Parsing Function", error);
            
            printf("Error: %s\n", error);

			return;
		}
		
		if (entry->graph != NULL) {
			deleteGraph(entry->graph);
		}

		entry->graph = createGraph(feq, ffeq, GRAPH_PORT_SIZE, GRAPH_PORT_SIZE);

		free(error);
		free(feq);
		free(ffeq);
		entry->active = 1;
		entry->time = g_time;
	}
}

int main() {
	swInit();
	swInitVK();

	SWindow* window = swCreateWindow(1000, 620, "Grapher2D");
    g_window = window;
	SView* rootView = swGetRootView(window);

	SOpenGLContextAttribs attribs;
	attribs.major = 3;
	attribs.minor = 3;
	attribs.debug = 0;
	attribs.swapInterval = 0;

	SRect* glViewBounds = swMakeRect(390, 10, 600, 600);
	SView* view = swCreateView(rootView, glViewBounds);

	uint32_t count = 0;
	char** extensions = swGetRequiredExtensionsVK(&count);

	VKLInstance* instance;
	VkBool32 debug = 0;
	vklCreateInstance(&instance, NULL, debug, extensions[1], swGetProcAddressVK);

	VKLSurface* surface = malloc_c(sizeof(VKLSurface));
	surface->width = 600;
	surface->height = 600;

	swCreateWindowSurfaceVK(instance->instance, view, NULL, &surface->surface);

	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;
	vklCreateDevice(instance, &device, &surface, 1, &deviceContexts, 0, NULL);

	VKLDeviceGraphicsContext* devCon = deviceContexts[0];

	VKLSwapChain* swapChain;
	VKLFrameBuffer* backBuffer;

	vklCreateSwapChain(devCon, &swapChain, VK_TRUE);
	
	vklGetBackBuffer(swapChain, &backBuffer);

	//SOpenGLContext* conetxt = swCreateOpenGLContext(view, &attribs);
	
	swSetMouseScrollCallback(view, scrollCallback);
	swSetMouseDownCallback(view, mouseDownCallback);
	swSetMouseUpCallback(view, mouseUpCallback);
	swSetMouseMovedCallback(view, mouseMovedCallback);

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

	/*
	swMakeContextCurrent(conetxt);

	if (!gladLoadGLLoader((GLADloadproc)swGetProcAddressGL)) {
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
	*/

	double start_time = swGetTime();

	uint8_t i_color = 0;

	double lasttime = start_time;
	uint32_t frames = 0;

	//SLabel* label = swCreateLabel(rootView, swMakeRect(250, 200, 300, 100), "Hello World!");

	VkCommandBuffer cmdBuffer;
	vklAllocateCommandBuffer(devCon, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	vklSetClearColor(backBuffer, 0.25f, 0.45f, 1.0f, 1.0f);

	while (!swCloseRequested(window)) {
		swPollEvents();

		frames++;

		g_time = swGetTime() - start_time;

		if (swGetTime() - lasttime > 1.0) {
			printf("frames = %d\n", frames);
			lasttime = swGetTime();
			frames = 0;
		}

		vklBeginCommandBuffer(device, cmdBuffer);

		vklBeginRender(device, backBuffer, cmdBuffer);

		

		vklEndRender(device, backBuffer, cmdBuffer);

		vklEndCommandBuffer(device, cmdBuffer);

		vklExecuteCommandBuffer(devCon, cmdBuffer);

		vklPresent(swapChain);

		/*
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
				drawGraphData(currentEntry->graph, (float)(g_time-currentEntry->time), (float)(g_time));
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

		swSwapBufers(conetxt);
		*/

		swDraw(window);
	}

	swDestroyWindow(window);

	return 0;
}
