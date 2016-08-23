#include <GLUI/GLUI.h>
#include <GLFW/glfw3.h>
#include "math/Equation.h"
#include <glcorearb.h>

using namespace glui;

#define VAL_NUM 600
#define ZOOM_PERCENT 0.05
#define glGenFramebuffers glFuncs->glGenFramebuffersM
#define glGenRenderbuffers glFuncs->glGenRenderbuffersM
#define glBindFramebuffer glFuncs->glBindFramebufferM
#define glBindRenderbuffer glFuncs->glBindRenderbufferM
#define glRenderbufferStorageMultisample glFuncs->glRenderbufferStorageMultisampleM
#define glFramebufferRenderbuffer glFuncs->glFramebufferRenderbufferM
#define glBlitFramebuffer glFuncs->glBlitFramebufferM

double g_left = -6;
double g_right = 6;
double g_down = -6;
double g_up = 6;

int g_windowWidth = 600;
int g_windowHeight = 600;

int g_colorNum = 5;
Color* g_colors = new Color[g_colorNum];

typedef struct MGLFuncs {
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersM;
	PFNGLGENRENDERBUFFERSPROC glGenRenderbuffersM;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebufferM;
	PFNGLBINDRENDERBUFFERPROC glBindRenderbufferM;
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisampleM;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbufferM;
	PFNGLBLITFRAMEBUFFERPROC glBlitFramebufferM;
} MGLFuncs;

typedef struct Graph {
	Equation* e;
	double* vals;
	Variable* xVar;
	Variable* tVar;
	Variable* atVar;
	double startTime;
} Graph;

double abs_c(double n1) {
	return n1 > 0 ? n1 : -(n1);
}

void drawAxes(int width, int height) {
	double l = abs_c(g_left);
	double r = abs_c(g_right);

	double xoff = (l / (r + l))* width;

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
	for (int i = 0; i < VAL_NUM; i++) {
		xVar->value = xint*i + xl;
		try {
			arr[i] = e->eval();
		}
		catch (const std::exception& e) {
			std::cout << "Equation:  is invalid!\n";
		}
	}
}

void renderVals(double* arr, double xl, double xr, double yd, double yu, int width, int height, Color color) {
	double xint = (width * 1.0f) / (VAL_NUM - 1);
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
	for (int i = 1; i < VAL_NUM; i++) {
		ay1 = arr[i - 1];
		ay2 = arr[i];

		x1 = xint*(i - 1);
		x2 = xint*(i);

		y1 = ((ay1 - yd) / yh)*height;
		y2 = ((ay2 - yd) / yh)*height;

		if (i - 1 > 0 && i + 1 < VAL_NUM) {
			ax1 = axint*(i - 1) + xl;
			ax2 = axint*i + xl;

			ay3 = arr[i - 2];
			ay4 = arr[i + 1];

			ax3 = axint*(i - 2) + xl;
			ax4 = axint*(i + 1) + xl;

			float m1 = (ay1 - ay3) / (ax1 - ax3);
			float m2 = (ay2 - ay4) / (ax2 - ax4);
			float m = (ay1 - ay2) / (ax1 - ax2);

			bool p1 = m1 > 0;
			bool p2 = m2 > 0;
			bool p = m > 0;

			if (p1 == p2 && p1 != p && abs_c(m) > 2) {
				if (p) {
					glVertex2f(x1, y1);
					glVertex2f(x1, 0);

					glVertex2f(x2, y2);
					glVertex2f(x2, height);
				}
				else {
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

int main() {
	GLUI::init();

	Window win("Grapher2D", 1000, 620);
	Renderer::init(&win);
	Layout* layout = new AbsoluteLayout(&win, 1000, 620);

	Font* font24 = new Font("arial.ttf", 24);

	if (!font24->inited()) {
		win.destroy();
#if defined(_WIN32) || defined (_WIN64)
		system("PAUSE");
#endif
		return -1;
	}

	Font* font20 = new Font("arial.ttf", 20);

	if (!font20->inited()) {
		win.destroy();
#if defined(_WIN32) || defined (_WIN64)
		system("PAUSE");
#endif
		return -1;
	}

	Theme theme = {};
	theme.body = color::lightGrey;
	theme.check = color::black;
	theme.circle = color::black;
	theme.hover = color::grey;
	theme.outline = color::black;
	theme.press = color::darkGrey;
	theme.text = color::black;

	TextStyle textStyle = { 20, font20 };
	TextStyle buttonStyle = { 24, font24 };

	std::vector<Button*> buttons;
	std::vector<TextBox*> textBoxes;
	std::vector<Graph*> graphs;

	MGLFuncs* glFuncs =  (MGLFuncs*)malloc(sizeof(MGLFuncs));

	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbuffer");
	glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)glfwGetProcAddress("glRenderbufferStorageMultisample");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glfwGetProcAddress("glFramebufferRenderbuffer");
	glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)glfwGetProcAddress("glBlitFramebuffer");

	int frameBufferWidth, frameBufferHeight;
	glfwGetFramebufferSize((GLFWwindow*) win.getGLFWwindow(), &frameBufferWidth, &frameBufferHeight);

	int samples = 4;
	GLuint fbo = 0;
	GLuint colorRenderBuffer = 0;
	GLuint depthRenderBuffer = 0;

	glGenRenderbuffers(1, &colorRenderBuffer);
	glGenRenderbuffers(1, &depthRenderBuffer);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER ,fbo);

	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, frameBufferWidth, frameBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, frameBufferWidth, frameBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLPanel* panel;

	panel = new GLPanel({ 390, 10, 600, 600 }, { 600, 600 }, layout,
	[]()->void {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, g_windowWidth, 0, g_windowHeight, -1, 1);
		glMatrixMode(GL_MODELVIEW);

	},
	[&]()->void {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glClear(GL_COLOR_BUFFER_BIT);
		drawAxes(600, 600);

		for (int i = 0; i < graphs.size(); i++) {
			if (graphs[i] == NULL) {
				continue;
			}

			graphs[i]->atVar->value = glfwGetTime();
			graphs[i]->tVar->value = graphs[i]->atVar->value - graphs[i]->startTime;
			genVals(graphs[i]->vals, g_left, g_right, graphs[i]->e, graphs[i]->xVar);
			renderVals(graphs[i]->vals, g_left, g_right, g_down, g_up, 600, 600, g_colors[i%g_colorNum]);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, panel->getFBO());

		glClear(GL_COLOR_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, frameBufferWidth, frameBufferHeight, 0, 0, frameBufferWidth, frameBufferHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, panel->getFBO());
	},
	[](GLPanelMouseData* data)->void {
		if ((data->difference.x != 0 || data->difference.y != 0) && data->leftDown) {
			double width = g_right - g_left;
			double percentX = data->difference.x / g_windowWidth;

			double moveX = -width * percentX;

			g_left += moveX;
			g_right += moveX;

			double height = g_up - g_down;
			double percentY = data->difference.y / g_windowHeight;

			double moveY = height * percentY;

			g_down += moveY;
			g_up += moveY;
		}
		
		if (data->scroll != 0) {
			double width = g_right - g_left;
			double percentX = (data->pos.x - g_windowHeight / 2.0) / g_windowWidth;
			double moveX = width * percentX;

			double xpos = ((g_right + g_left) / 2.0);
			double ypos = ((g_up + g_down) / 2.0);

			double sizeX = g_right - g_left;
			double sizeY = g_up - g_down;

			if (data->scroll < 0) {
				sizeX = sizeX / ((1 - ZOOM_PERCENT));
				sizeY = sizeY / ((1 - ZOOM_PERCENT));
			}
			else if (data->scroll > 0) {
				sizeX = sizeX * ((1 - ZOOM_PERCENT));
				sizeY = sizeY * ((1 - ZOOM_PERCENT));
			}

			g_left = xpos - sizeX / 2;
			g_right = xpos + sizeX / 2;
			g_down = ypos - sizeY / 2;
			g_up = ypos + sizeY / 2;
		}
	}, theme);

	Button* addGraphButton = NULL;
	Button* removeGraphButton = NULL;

	auto deleteGraph = [&](int index)->void {
		if (graphs[index] != NULL) {
			graphs[index]->e->cleanUp();
			delete graphs[index]->e;
			delete[] graphs[index]->vals;
			free(graphs[index]);
			
			graphs[index] = NULL;
		}
	};

	auto setGraph = [&](std::string eq, int index)->void {
		if (graphs[index] != NULL) {
			deleteGraph(index);
		}

		if (eq.size() == 0) {
			return;
		}

		graphs[index] = (Graph*)malloc(sizeof(Graph));

		graphs[index]->e = new Equation();
		graphs[index]->e->setString(String((char*)eq.c_str()));

		graphs[index]->xVar = graphs[index]->e->createVariable("x");
		graphs[index]->tVar = graphs[index]->e->createVariable("t");
		graphs[index]->atVar = graphs[index]->e->createVariable("at");
		graphs[index]->startTime = glfwGetTime();
		graphs[index]->vals = new double[VAL_NUM];

		String* result = graphs[index]->e->parse();

		if (result != NULL) {
			std::cout << "Error parsing expression: '" << eq << "'\n";
			std::cout << "Error: " << *result << "\n";

			delete result;
			deleteGraph(index);
		}
	};
	
	std::function<void(void)> addGraphButtonCallback = [&]() -> void {
		if (buttons.size() >= 9) {
			return;
		}

		Rectangle boundsa = addGraphButton->getBounds();
		addGraphButton->setPos({ boundsa.x, boundsa.y - 60 });
		Rectangle boundsr = removeGraphButton->getBounds();
		removeGraphButton->setPos({ boundsr.x, boundsr.y - 60 });

		int bSize = buttons.size();

		TextBox* textBox = new TextBox({ 10, (float)(560 - 60.0f * buttons.size()) + 10, 290, 30 }, layout, { textStyle , 1, 2, theme });

		Button* button = new Button({ 310, (float)(560 - 60.0f * buttons.size()) + 10, 70, 30 }, layout, "Submit", { textStyle,
			[bSize, textBox, setGraph]()->void {
			setGraph(textBox->m_text, bSize);
		}, 2, theme
		});


		textBoxes.push_back(textBox);
		buttons.push_back(button);
		graphs.push_back(NULL);
	};

	addGraphButton = new Button({ 10 - 2, 560, 360 / 2, 50 }, layout, "Add Graph", { buttonStyle, addGraphButtonCallback, 3, theme });

	removeGraphButton = new Button({ 360/2 + 20, 560, 360 / 2, 50 }, layout, "Remove Graph", { buttonStyle,
		[&]() -> void {
			if (buttons.size() == 1) {
				return;
			}

			Rectangle boundsa = addGraphButton->getBounds();
			addGraphButton->setPos({ boundsa.x, boundsa.y + 60 });
			Rectangle boundsr = removeGraphButton->getBounds();
			removeGraphButton->setPos({ boundsr.x, boundsr.y + 60 });

			deleteGraph(buttons.size() - 1);
			graphs.pop_back();

			delete buttons[buttons.size()-1];
			buttons.pop_back();

			delete textBoxes[textBoxes.size() - 1];
			textBoxes.pop_back();
			
	}, 3, theme
	});

	addGraphButtonCallback();

	g_colors[0] = { 0.8f, 0.2f, 0.2f };
	g_colors[1] = { 0.1f, 0.6f, 0.1f };
	g_colors[2] = { 0.2f, 0.2f, 0.9f };
	g_colors[3] = { 0.9f, 0.65f, 0.2f };
	g_colors[4] = { 0.8f, 0.2f, 0.8f };

	while (win.isOpen()) {
		win.poll();
		
		panel->poll();
		addGraphButton->poll();
		removeGraphButton->poll();

		for (Button* b:buttons) {
			b->poll();
		}

		for (TextBox* t:textBoxes) {
			t->poll();
		}

		Renderer::clear({0.6f, 0.75f, 1});

		panel->render();
		addGraphButton->render();
		removeGraphButton->render();

		for (Button* b : buttons) {
			b->render();
		}

		for (TextBox* t : textBoxes) {
			t->render();
		}

		win.swap();
	}

	for (int i = 0; i < graphs.size(); i++) {
		deleteGraph(i);
	}

	win.destroy();

	GLUI::destroy();
	return 0;
}
