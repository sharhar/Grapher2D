#include "Window.h"
#include "math/Equation.h"

#define VAL_NUM 150

void drawAxes(int width, int height) {
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(0, height/2.0f);
	glVertex2f(width, height/2.0f);
	glVertex2f(width/2.0f, 0);
	glVertex2f(width/2.0f, height);
	glEnd();
}

void genVals(double* arr, double xl, double xr, Equation* e) {
	double xint = (xr - xl) / (VAL_NUM);
	for (int i = 0; i < VAL_NUM;i++) {
		double x = xint*i + xl;
		e->setVar("x", x);
		arr[i] = e->eval();
	}
}

void renderVals(double* arr, double yd, double yu, int width, int height) {
	double xint = (width * 1.0f)/(VAL_NUM-1);
	double yh = (yu - yd);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(2.5f);
	glBegin(GL_LINES);
	for (int i = 1; i < VAL_NUM;i++) {
		glVertex2f(xint*(i-1) + 0, ((arr[i-1] - yd)/yh)*height);
		glVertex2f(xint*(i) + 0, ((arr[i] - yd)/yh)*height);
	}
	glEnd();
}

int main() {
	glfwInit();

	Window window(800, 600, "Hello!");

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	Equation e;

	e.setString("sin(x)*e^(x/(cos(a)*2 + 4))");
	e.parse();

	double left = -10;
	double right = 10;
	double down = -10;
	double up = 10;

	double* vals = new double[VAL_NUM];

	while (window.isOpen()) {
		window.poll();

		e.setVar("a", glfwGetTime());
		genVals(vals, left, right, &e);

		glClear(GL_COLOR_BUFFER_BIT);

		int width, height;
		
		window.getSize(&width, &height);

		glViewport(0, 0, width, height);

		drawAxes(800, 600);
		renderVals(vals, down ,up, 800, 600);

		window.swapBuffers();
	}
	
	window.destroy();

	glfwTerminate();

	return 0;
}