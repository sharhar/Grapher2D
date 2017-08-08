#include <swin/SWin.h>

int main() {
	swInit();

	SWindow* window = swCreateWindow(1000, 620, "Grapher2D");
	SView* rootView = swGetRootView(window);

	SOpenGLView* glView = swCreateOpenGLView(rootView, swMakeRect(390, 10, 600, 600));

	swMakeContextCurrent(glView);

	while (!swCloseRequested(window)) {
		swPollEvents();

		swSwapBufers(glView);
		swDraw(window);
	}

	swCloseWindow(window);

	return 0;
}