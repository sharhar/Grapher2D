#include <swin/SWin.h>
#include <glad/glad.h>
#include <malloc.h>

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