#include "Window.h"
#include "math/Equation.h"

double add(double n1, double n2) {
	return n1 + n2;
}

double mul(double n1, double n2) {
	return n1 * n2;
}

int main() {
	Opperator* addO = (Opperator*)malloc(sizeof(Opperator));
	addO->name = '+';
	addO->order = 4;
	addO->func = &add;

	Opperator* mulO = (Opperator*)malloc(sizeof(Opperator));
	mulO->name = '*';
	addO->order = 6;
	mulO->func = &add;

	Equation e;
	e.setString("5 * 6 + 2 + sin( hello + tan(ln)* 4) * 6 + (4 + 7)");
	e.addOpperator(addO);
	e.addOpperator(mulO);
	e.parse();

	system("PAUSE");

	/*
	glfwInit();

	Window window(800, 600, "Hello!");

	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	

	while (window.isOpen()) {
		window.poll();
		glClear(GL_COLOR_BUFFER_BIT);
		window.swapBuffers();
	}
	
	window.destroy();

	glfwTerminate();
	*/
	return 0;
}