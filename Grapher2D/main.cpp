//#define PRINT_FPS
#define VAL_NUM 800
#define ZOOM_PERCENT 0.05

#include "Window.h"
#include "math/Equation.h"
#include "UI.h"

double g_left = -8;
double g_right = 8;
double g_down = -6;
double g_up = 6;

int g_windowWidth = 800;
int g_windowHeight = 600;

typedef struct Color {
	float r, g, b;
} Color;

typedef struct Graph {
	Equation* e;
	double* vals;
	Variable* xVar;
	Variable* tVar;
	bool used = false;
} Graph;

Graph** graphs;

double abs_c(double n1) {
	return n1 > 0 ? n1 : -(n1);
}

void drawAxes(int width, int height) {
	double l = abs_c(g_left);
	double r = abs_c(g_right);

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

	double d = abs_c(g_down);
	double u = abs_c(g_up);

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

void genVals(double* arr, double xl, double xr, Equation* e, Variable* xVar) {
	double xint = (xr - xl) / (VAL_NUM);
	for (int i = 0; i < VAL_NUM;i++) {
		xVar->value = xint*i + xl;
		try {
			arr[i] = e->eval();
		} catch (const std::exception& e) {
			std::cout << "Equation:  is invalid!\n";
		}
	}
}

void renderVals(double* arr, double xl, double xr, double yd, double yu, int width, int height, Color color) {
	double xint = (width * 1.0f)/(VAL_NUM-1);
	double axint = (xr - xl) / (VAL_NUM);
	double yh = (yu - yd);

	float x1;
	float y1;
	float x2;
	float y2;

	float m;

	float ax1;
	float ax2;
	float ay1;
	float ay2;
	float ax3;
	float ax4;
	float ay3;
	float ay4;

	glColor3f(color.r, color.g, color.b);
	glLineWidth(2.5f);
	glBegin(GL_LINES);
	for (int i = 1; i < VAL_NUM;i++) {
		ay1 = arr[i - 1];
		ay2 = arr[i];

		x1 = xint*(i - 1);
		x2 = xint*(i);

		y1 = ((ay1 - yd) / yh)*height;
		y2 = ((ay2 - yd) / yh)*height;

		if (i-1 > 0 && i+1 < VAL_NUM) {
			ax1 = axint*(i - 1) + xl;
			ax2 = axint*i + xl;

			ay3 = arr[i - 2];
			ay4 = arr[i + 1];

			ax3 = axint*(i - 2) + xl;
			ax4 = axint*(i + 1) + xl;
			
			float m1 = (ay1 - ay3) / (ax1 - ax3);
			float m2 = (ay2 - ay4) / (ax2 - ax4);
			float m =  (ay1 - ay2) / (ax1 - ax2);

			bool p1 = m1 > 0;
			bool p2 = m2 > 0;
			bool p = m > 0;

			if (p1 == p2 && p1 != p && abs_c(m) > 2) {
				if (p) {
					glVertex2f(x1, y1);
					glVertex2f(x1, 0);

					glVertex2f(x2, y2);
					glVertex2f(x2, height);
				} else {
					glVertex2f(x1, y1);
					glVertex2f(x1, height);

					glVertex2f(x2, y2);
					glVertex2f(x2, 0);
				}

				continue;
			}
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

void UI::addGraph(std::string eq, int index) {
	if (eq.size() == 0) {
		if (graphs[index] == NULL) {
			return;
		}

		while (graphs[index]->used) {
			
		}

		graphs[index] = NULL;
		return;
	}

	graphs[index] = (Graph*)malloc(sizeof(Graph));
	graphs[index]->e = new Equation();
	graphs[index]->e->setString(String((char*) eq.c_str()));

	graphs[index]->xVar = graphs[index]->e->createVariable("x");
	graphs[index]->tVar = graphs[index]->e->createVariable("time");
	graphs[index]->vals = new double[VAL_NUM];

	try {
		graphs[index]->e->parse();
	} catch (const std::exception& e) {
		std::cout << "Equation: '" << eq << "' is invalid!\n";
	}
	
}

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);

	input::mouse::init();

	Window window(g_windowWidth, g_windowHeight, "Grapher");
	window.setVSync(true);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_windowWidth, 0, g_windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_MULTISAMPLE);

	UI::start();
	
	graphs = new Graph*[5];
	for (int i = 0; i < 5; i++) {
		graphs[i] = NULL;
	}

#if defined(PRINT_FPS) && defined(_DEBUG)
	int frame = 0;
	int fps = 0;
	Uint32 start = SDL_GetTicks();
#endif

	int colorNum = 5;
	Color* colors = new Color[colorNum];
	colors[0] = { 0.8f, 0.2f, 0.2f };
	colors[1] = { 0.1f, 0.6f, 0.1f };
	colors[2] = { 0.2f, 0.2f, 0.9f };
	colors[3] = { 0.9f, 0.65f, 0.2f };
	colors[4] = { 0.8f, 0.2f, 0.8f };

	while (window.isOpen() && UI::open) {
		window.poll();

#if defined(PRINT_FPS) && defined(_DEBUG)
		Uint32 time = SDL_GetTicks();
		
		if (time - start > 1000) {
			fps = frame;
			std::cout << "FPS: " << fps << "\n";
			frame = 0;
			start = time;
		}
		
		frame++;
#endif
		glClear(GL_COLOR_BUFFER_BIT);

		g_windowWidth = window.getWidth();
		g_windowHeight = window.getHeight();

		glViewport(0, 0, g_windowWidth, g_windowHeight);

		drawAxes(800, 600);

		for (int i = 0; i < 5; i++) {
			if (graphs[i] == NULL) {
				continue;
			}
			graphs[i]->used = true;

			graphs[i]->tVar->value = SDL_GetTicks() / 1000.0;
			genVals(graphs[i]->vals, g_left, g_right, graphs[i]->e, graphs[i]->xVar);
			renderVals(graphs[i]->vals, g_left, g_right, g_down, g_up, 800, 600, colors[i%colorNum]);

			graphs[i]->used = false;
		}
		
		window.swapBuffers();
	}
	
	window.destroy();

	UI::stop();

	SDL_Quit();

	return 0;
}
