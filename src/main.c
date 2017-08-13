#include <swin/SWin.h>
#include <glad/glad.h>
#include "utils.h"
#include "equation.h"
#include <stdio.h>

void* malloc(size_t);

#define COLOR_NUM 5
#define ZOOM_PERCENT 0.025
#define GRAPH_PORT_SIZE 1200

typedef struct Entry {
	STextField* textField;
	SButton* submitButton;
	SButton* deleteButton;

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

void submitCallback(Entry* entry) {
	printf("%s\n", swGetTextFromTextField(entry->textField));
}

void createEntry(UIState* uiState) {
	Entry* entry = malloc(sizeof(Entry));
	entry->textField = swCreateTextField(uiState->rootView, swMakeRect(10, 580 - uiState->y * 40, 285, 30), "");
	entry->submitButton = swCreateButton(uiState->rootView, swMakeRect(305, 580 - uiState->y * 40, 75, 30), "Submit", &submitCallback, entry);
	entry->deleteButton = NULL;
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

	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);
	createEntry(state);

	ParsingInfo* parseInfo = eqGetDefaultParseInfo();
	
	char* eq = "sin(x*47.4823)^y = y + 34634.7562 - t + 77";
	char* error = NULL;

	char* ffeq;
	char* feq;
	
	eqConvert(parseInfo, eq, & feq, &ffeq, &error);

	printf("eq    = %s\nfeq   = %s\nffeq  = %s\nerror = %s\n", eq, feq, ffeq, error);

	swMakeContextCurrent(glView);

	if (!gladLoadGLLoader((GLADloadproc)swGetProcAddress)) {
		printf("Could not load opengl!\n");
		return 0;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	while (!swCloseRequested(window)) {
		swPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}
