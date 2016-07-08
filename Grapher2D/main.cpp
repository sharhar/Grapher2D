#include "Window.h"
#include "math/Equation.h"
#include "gui/GUI.h"

#ifdef _WIN32
#ifndef _DEBUG
#include <windows.h>
#endif
#endif

#define VAL_NUM 600
#define ZOOM_PERCENT 0.05

double g_left = -8;
double g_right = 8;
double g_down = -6;
double g_up = 6;

int g_windowWidth = 800;
int g_windowHeight = 600;

void drawAxes(int width, int height) {
	double l = abs(g_left);
	double r = abs(g_right);

	double xoff = (l/(r + l))* width;

	if (g_left < 0 && g_right > 0) {
		glPushMatrix();

		glTranslatef(xoff, 0, 0);

		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(0, height);
		glEnd();

		glPopMatrix();
	}

	double d = abs(g_down);
	double u = abs(g_up);

	double yoff = (d / (u + d))* height;

	if (g_up > 0 && g_down < 0) {
		glPushMatrix();

		glTranslatef(0, yoff, 0);

		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(width, 0);
		glEnd();

		glPopMatrix();
	}
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

	float x1;
	float y1;
	float x2;
	float y2;

	bool y1d;
	bool y1u;
	bool y1o;
	
	bool y2d;
	bool y2u;
	bool y2o;
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(2.5f);
	glBegin(GL_LINES);
	for (int i = 1; i < VAL_NUM;i++) {
		x1 = xint*(i - 1);
		y1 = ((arr[i - 1] - yd) / yh)*height;
		x2 = xint*(i);
		y2 = ((arr[i] - yd) / yh)*height;

		y1d = y1 < 0;
		y1u = y1 > height;
		y1o = (y1d || y1u);

		y2d = y2 < 0;
		y2u = y2 > height;
		y2o = (y2d || y2u);

		if (y1o && y2o) {
			continue;
		}

		if (y1d) {
			y1 = 0;
		} else if (y1u) {
			y1 = height;
		}

		if (y2d) {
			y2 = 0;
		}
		else if (y2u) {
			y2 = height;
		}

		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	}
	glEnd();
}

void input::mouse::moved(double x, double y) {

}

void input::mouse::dragged(double x, double y) {
	double width = g_right - g_left;
	double percentX = x / g_windowWidth;

	double moveX = -width * percentX;

	g_left += moveX;
	g_right += moveX;

	double height = g_up - g_down;
	double percentY = y / g_windowHeight;

	double moveY = height * percentY;

	g_down += moveY;
	g_up += moveY;
}

void input::mouse::scrolled(double s) {
	input::MouseData* mouseData = input::mouse::getMouseData();
	
	double width = g_right - g_left;
	double percentX = (mouseData->posX - g_windowHeight/2.0) / g_windowWidth;
	double moveX = width * percentX;

	double xpos = ((g_right + g_left) / 2.0);
	double ypos = ((g_up + g_down) / 2.0);

	double sizeX = g_right - g_left;
	double sizeY = g_up - g_down;

	if (s < 0) {
		sizeX = sizeX / ((1 - ZOOM_PERCENT));
		sizeY = sizeY / ((1 - ZOOM_PERCENT));
	} else if (s > 0) {
		sizeX = sizeX * ((1 - ZOOM_PERCENT));
		sizeY = sizeY * ((1 - ZOOM_PERCENT));
	}

	g_left = xpos - sizeX / 2;
	g_right = xpos + sizeX / 2;
	g_down = ypos - sizeY / 2;
	g_up = ypos + sizeY / 2;
}

int main() {
#ifdef _WIN32
#ifndef _DEBUG
	FreeConsole();
#endif
#endif

	glfwInit();

	input::mouse::init();

	Window window(g_windowWidth, g_windowHeight, "Grapher");

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_windowWidth, 0, g_windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	GUI g;

	Equation e;

	e.setString("1/cos(x)");
	e.parse();

	double* vals = new double[VAL_NUM];

	while (window.isOpen() && g.running) {
		window.poll();

		//e.setVar("a", glfwGetTime());
		genVals(vals, g_left, g_right, &e);

		glClear(GL_COLOR_BUFFER_BIT);
		
		window.getSize(&g_windowWidth, &g_windowHeight);

		glViewport(0, 0, g_windowWidth, g_windowHeight);

		drawAxes(800, 600);
		renderVals(vals, g_down , g_up, 800, 600);

		window.swapBuffers();
	}
	
	window.destroy();

	g.stop();

	glfwTerminate();

	return 0;
}