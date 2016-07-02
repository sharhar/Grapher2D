#include "Window.h"
#include "math/Equation.h"
#include <math.h>

double add(double n1, double n2) {
	return n1 + n2;
}

double mul(double n1, double n2) {
	return n1 * n2;
}

double tanF(double* input) {
	return tan(input[0]);
}

double maxF(double* input) {
	if (input[0] >= input[1]) {
		return input[0];
	} else {
		return input[1];
	}
}

double blank(double* args) {
	return args[0];
}

int main() {
	Opperator* addO = (Opperator*)malloc(sizeof(Opperator));
	addO->name = '+';
	addO->order = 10;
	addO->func = &add;

	Opperator* mulO = (Opperator*)malloc(sizeof(Opperator));
	mulO->name = '*';
	mulO->order = 8;
	mulO->func = &mul;

	Function* funcTan = (Function*)malloc(sizeof(Function));
	funcTan->name = new String("tan");
	funcTan->func = &tanF;

	Function* funcMax = (Function*)malloc(sizeof(Function));
	funcMax->name = new String("max");
	funcMax->func = &maxF;

	Function* funcBlank = (Function*)malloc(sizeof(Function));
	funcMax->name = new String("");
	funcMax->func = &blank;

	Equation e;
	e.setString("6 * (2 + 4)");
	e.addOpperator(addO);
	e.addOpperator(mulO);
	e.addFunction(funcTan);
	e.addFunction(funcMax);
	e.addFunction(funcBlank);
	e.parse();

	double result = e.eval();

	std::cout << "EVAL: " << result << "\n";

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