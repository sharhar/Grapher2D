#include <swin/SWin.h>
#include <glad/glad.h>

int main() {
	swInit();

	SWindow* window = swCreateWindow(1000, 620, "Grapher2D");
	SView* rootView = swGetRootView(window);

	SOpenGLView* glView = swCreateOpenGLView(rootView, swMakeRect(390, 10, 600, 600));

	swMakeContextCurrent(glView);

	if (!gladLoadGLLoader((GLADloadproc)swGetProcAddress)) {
		printf("Could not load opengl!\n");
		return 0;
	}

	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	//GLuint vao;
	//glGenVertexArrays(1, &vao);

	while (!swCloseRequested(window)) {
		swPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}