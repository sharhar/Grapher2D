#include "Window.h"
#include "math/Equation.h"

#define VAL_NUM 100

void drawAxes() {
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex2f(0, 300);
	glVertex2f(800, 300);
	glVertex2f(400, 0);
	glVertex2f(400, 600);
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

void renderVals(double* arr, double yd, double yu) {
	double xint = (800.0f)/(VAL_NUM-1);
	double yh = (yu - yd);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(2.5f);
	glBegin(GL_LINES);
	for (int i = 1; i < VAL_NUM;i++) {
		glVertex2f(xint*(i-1) + 0, ((arr[i-1] - yd)/yh)*600);
		glVertex2f(xint*(i) + 0, ((arr[i] - yd)/yh)*600);
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

	e.setString("sin(x - a) + cos(x + a)");
	e.parse();

	double left = -5;
	double right = 5;
	double down = -5;
	double up = 5;

	double* vals = new double[VAL_NUM];

	

	while (window.isOpen()) {
		window.poll();

		e.setVar("a", glfwGetTime());
		genVals(vals, left, right, &e);

		glClear(GL_COLOR_BUFFER_BIT);
		
		drawAxes();
		
		renderVals(vals, down ,up);

		window.swapBuffers();
	}
	
	window.destroy();

	glfwTerminate();

	return 0;
}