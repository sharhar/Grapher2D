#include <GLUI/GLUI.h>
#include <GLFW/glfw3.h>
#include "math/Equation.h"
#include <glcorearb.h>
#include <math.h>
#include <string>
#include <thread>
#include <chrono>
#include "GLGraph.h"

using namespace glui;

#define VAL_NUM 600
#define ZOOM_PERCENT 0.05
#define IMPLICIT_SKIP 4
#define EQ_NUM 4

#define glGenFramebuffers__ glFuncs->glGenFramebuffersM
#define glGenRenderbuffers__ glFuncs->glGenRenderbuffersM
#define glBindFramebuffer__ glFuncs->glBindFramebufferM
#define glBindRenderbuffer__ glFuncs->glBindRenderbufferM
#define glRenderbufferStorageMultisample__ glFuncs->glRenderbufferStorageMultisampleM
#define glFramebufferRenderbuffer__ glFuncs->glFramebufferRenderbufferM
#define glBlitFramebuffer__ glFuncs->glBlitFramebufferM

double g_left  = -6;
double g_right =  6;
double g_down  = -6;
double g_up    =  6;

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
	double startTime;
	bool del;
	GLGraph* glg;
} Graph;

typedef struct GraphData {
	TextBox* textBox;
	Button* button;
	Graph* graph;
	bool del;
} GraphData;

inline double abs_c(double n1) {
	return n1 > 0 ? n1 : -(n1);
}

inline int sign_c(double n1) {
	return n1 >= 0 ? (n1 != 0 ? 1 : 0) : -1;
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

void drawGrid(double xl, double xr, double yd, double yu, int width, int height) {
	double xratio = width/(xr - xl);

	double xlen = xr - xl;
	double xlog = log10(xlen);
	double xfloor = floor(xlog);
	double xmag = pow(10, xfloor);

	if (floor(xlog - 0.25) < xfloor) {
		xmag /= 2;
	} 

	if (floor(xlog - 0.5) < xfloor) {
		xmag /= 2;
	}

	double xlr = round(xl/xmag)*xmag;
	double xrr = round(xr/xmag)*xmag;
	
	double xNum = (xrr - xlr)/xmag;

	glColor3f(0.5f, 0.5f, 0.5f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);
	for (int i = 0; i < xNum;i++) {
		double tx = (xlr - xl + xmag*i)*xratio;

		if (tx  < 0 || tx > width) {
			continue;
		}

		glVertex2f(tx, 0);
		glVertex2f(tx, height);
	}

	glEnd();

	double yratio = height / (yu - yd);

	double ylen = yu - yd;
	double ylog = log10(ylen);
	double yfloor = floor(ylog);
	double ymag = pow(10, yfloor);

	if (floor(ylog - 0.25) < yfloor) {
		ymag /= 2;
	}

	if (floor(ylog - 0.5) < yfloor) {
		ymag /= 2;
	}

	double ylr = round(yd / ymag)*ymag;
	double yrr = round(yu / ymag)*ymag;

	double yNum = (yrr - ylr) / ymag;

	glColor3f(0.5f, 0.5f, 0.5f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);
	for (int i = 0; i < yNum * 2; i++) {
		double ty = (ylr - yd + ymag*i)*yratio;

		if (ty < 0 || ty > height) {
			continue;
		}

		glVertex2f(0,     ty);
		glVertex2f(width, ty);
	}

	glEnd();
}

void drawNums(double xl, double xr, double yd, double yu, int width, int height, Font* font, Color* color) {
	double xratio = width / (xr - xl);

	double xlen = xr - xl;
	double xlog = log10(xlen);
	double xfloor = floor(xlog);
	double xmag = pow(10, xfloor);

	if (floor(xlog - 0.25) < xfloor) {
		xmag /= 2;
	}

	if (floor(xlog - 0.5) < xfloor) {
		xmag /= 2;
	}

	double xlr = round(xl / xmag)*xmag;
	double xrr = round(xr / xmag)*xmag;

	double xNum = (xrr - xlr) / xmag;

	for (int i = 0; i < xNum * 2; i++) {
		double xrv = xlr + xmag*i;

		double tx = (xlr - xl + xmag*i)*xratio;

		if (tx < -15 || tx > width) {
			continue;
		}

		std::string str = std::to_string(xrv);

		while (str.at(str.length() - 1) == '0') {
			str.pop_back();
		}

		if (str.at(str.length() - 1) == '.') {
			str.pop_back();
		}

		Renderer::drawString(str, tx + 2, 10, 15, font, color);
	}

	double yratio = height / (yu - yd);

	double ylen = yu - yd;
	double ylog = log10(ylen);
	double yfloor = floor(ylog);
	double ymag = pow(10, yfloor);

	if (floor(ylog - 0.25) < yfloor) {
		ymag /= 2;
	}

	if (floor(ylog - 0.5) < yfloor) {
		ymag /= 2;
	}

	double ylr = round(yd / ymag)*ymag;
	double yrr = round(yu / ymag)*ymag;

	double yNum = (yrr - ylr) / ymag;

	for (int i = 0; i < yNum * 2; i++) {
		double yrv = ylr + ymag*i;

		double ty = (ylr - yd + ymag*i)*yratio;

		if (ty < -15 || ty > height) {
			continue;
		}

		std::string str = std::to_string(yrv);

		while (str.at(str.length() - 1) == '0') {
			str.pop_back();
		}

		if (str.at(str.length() - 1) == '.') {
			str.pop_back();
		}

		Renderer::drawString(str, 10, ty + 2, 15, font, color);
	}
}

GLFWimage* genIcon() {
	unsigned char redr = (unsigned char)(0.8f * 255);
	unsigned char redg = (unsigned char)(0.2f * 255);
	unsigned char redb = (unsigned char)(0.2f * 255);

	float xs = 32;
	float ys = 32;

	int* siny = new int[255];

	for (int i = 0; i < 255; i++) {
		int x = i - 128;

		float y = -sin(x / xs) * ys;

		siny[i] = (int)y;
	}

	GLFWimage* img = (GLFWimage*)malloc(sizeof(GLFWimage));
	img->height = 255;
	img->width = 255;

	img->pixels = new unsigned char[img->width * img->height * 4];

	for (int y = 0; y < img->height; y++) {
		for (int x = 0; x < img->width; x++) {
			int index = (x + y * img->width) * 4;

			int xo = 128 - x;
			int yo = 128 - y;

			double dist = sqrt(xo*xo + yo*yo);

			img->pixels[index + 0] = 255;
			img->pixels[index + 1] = 255;
			img->pixels[index + 2] = 255;
			img->pixels[index + 3] = 255;

			int lineWidth = 9;

			if (y > 128 - lineWidth && y < 128 + lineWidth) {
				img->pixels[index + 0] = 0;
				img->pixels[index + 1] = 0;
				img->pixels[index + 2] = 0;
				img->pixels[index + 3] = 255;
			}
			else if (x > 128 - lineWidth && x < 128 + lineWidth) {
				img->pixels[index + 0] = 0;
				img->pixels[index + 1] = 0;
				img->pixels[index + 2] = 0;
				img->pixels[index + 3] = 255;
			}

			if (abs(siny[x] - (y - 128)) < 15) {
				img->pixels[index + 0] = redr;
				img->pixels[index + 1] = redg;
				img->pixels[index + 2] = redb;
				img->pixels[index + 3] = 255;
			}

			if (dist >= 110 && dist < 128) {
				img->pixels[index + 0] = 0;
				img->pixels[index + 1] = 0;
				img->pixels[index + 2] = 0;
				img->pixels[index + 3] = 255;
			}

			if (dist >= 128) {
				img->pixels[index + 3] = 0;
			}
		}
	}

	return img;
}

GLuint genFBO(MGLFuncs* glFuncs, int samples, int width, int height) {
	glGenFramebuffers__ = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	glGenRenderbuffers__ = (PFNGLGENRENDERBUFFERSPROC)glfwGetProcAddress("glGenRenderbuffers");
	glBindFramebuffer__ = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	glBindRenderbuffer__ = (PFNGLBINDRENDERBUFFERPROC)glfwGetProcAddress("glBindRenderbuffer");
	glRenderbufferStorageMultisample__ = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)glfwGetProcAddress("glRenderbufferStorageMultisample");
	glFramebufferRenderbuffer__ = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glfwGetProcAddress("glFramebufferRenderbuffer");
	glBlitFramebuffer__ = (PFNGLBLITFRAMEBUFFERPROC)glfwGetProcAddress("glBlitFramebuffer");

	GLuint fbo = 0;
	GLuint colorRenderBuffer = 0;
	GLuint depthRenderBuffer = 0;

	glGenRenderbuffers__(1, &colorRenderBuffer);
	glGenRenderbuffers__(1, &depthRenderBuffer);

	glGenFramebuffers__(1, &fbo);
	glBindFramebuffer__(GL_FRAMEBUFFER, fbo);

	glBindRenderbuffer__(GL_RENDERBUFFER, colorRenderBuffer);
	glRenderbufferStorageMultisample__(GL_RENDERBUFFER, samples, GL_RGBA8, width, height);
	glFramebufferRenderbuffer__(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);

	glBindRenderbuffer__(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorageMultisample__(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer__(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glBindRenderbuffer__(GL_RENDERBUFFER, 0);
	glBindFramebuffer__(GL_FRAMEBUFFER, 0);

	return fbo;
}

int main() {
	GLUI::init();

	Window win("Grapher2D", 1000, 620, false, 1, genIcon());
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
	theme.popupBackground = { 0.6f * 0.8f, 0.75f * 0.8f, 1};
	theme.popupText = color::black;

	TextStyle textStyle = { 20, font20 };
	TextStyle buttonStyle = { 24, font24 };

	std::vector<Button*> buttons;
	std::vector<TextBox*> textBoxes;
	std::vector<Graph*> graphs;
	std::vector<GraphData*> datas;

	int frameBufferWidth, frameBufferHeight;
	glfwGetFramebufferSize((GLFWwindow*) win.getGLFWwindow(), &frameBufferWidth, &frameBufferHeight);

	int samples = 4;
	MGLFuncs* glFuncs = (MGLFuncs*)malloc(sizeof(MGLFuncs));
	GLuint fbo = genFBO(glFuncs, samples, frameBufferWidth, frameBufferHeight);

	GLPanel* panel;

	panel = new GLPanel({ 390, 10, 600, 600 }, { 600, 600 }, layout,
	[]()->void {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, g_windowWidth, 0, g_windowHeight, -1, 1);
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_TEXTURE_2D);
	},
	[&]()->void {
		glBindFramebuffer__(GL_FRAMEBUFFER, fbo);

		glClear(GL_COLOR_BUFFER_BIT);
		drawGrid(g_left, g_right, g_down, g_up, 600, 600);
		drawAxes(600, 600);
		
		float time = glfwGetTime();

		for (int i = 0; i < graphs.size(); i++) {
			if (graphs[i] == NULL) {
				continue;
			}

			graphs[i]->glg->render(fbo, g_colors[i % 5], g_up, g_down, g_left, g_right, time - graphs[i]->startTime, time);
		}

		drawNums(g_left, g_right, g_down, g_up, 600, 600, font20, &color::black);

		glBindFramebuffer__(GL_FRAMEBUFFER, panel->getFBO());

		glClear(GL_COLOR_BUFFER_BIT);
		
		glBindFramebuffer__(GL_READ_FRAMEBUFFER, fbo);
		glBlitFramebuffer__(0, 0, frameBufferWidth, frameBufferHeight, 
			              0, 0, frameBufferWidth, frameBufferHeight, 
			              GL_COLOR_BUFFER_BIT, GL_NEAREST);
#ifdef __APPLE__
		glGetError();
#endif
		glBindFramebuffer__(GL_FRAMEBUFFER, panel->getFBO());

		glFinish();
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

	auto deleteGraph = [&](int index)->void {
		if (graphs[index] != NULL) {
			if (graphs[index]->glg != NULL) {
				graphs[index]->glg->cleanUp();
				delete graphs[index]->glg;
				graphs[index]->glg = NULL;
			}
			
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

		String equ = String((char*)eq.c_str());

		Equation* equation = new Equation();
		
		if (equ.find('=') != -1) {
			equation->setString(equ);
		}	else {
			equation->setString("(y)-(" + equ + ")");
		}
		
		equation->createVariable("y");
		equation->createVariable("x");
		equation->createVariable("t");
		equation->createVariable("at");
		
		String* result = NULL;

		result = equation->parse();

		if (result == NULL) {
			graphs[index] = (Graph*)malloc(sizeof(Graph));

			graphs[index]->del = false;

			graphs[index]->glg = new GLGraph(equation);
			graphs[index]->startTime = glfwGetTime();
		} else {
			char** chars = new char*[1];
			chars[0] = "Ok";

			std::string text = "Error parsing expression: \n" + 
				std::string(1, '"') + eq + std::string(1, '"') + 
				"\n\n" + 
				"Error: " + (*result).getstdstring();

			PopupDescriptor pDesc = {};
			pDesc.width = 300;
			pDesc.height = 200;
			pDesc.title = "Parsing Error!";
			pDesc.text = text.c_str();
			pDesc.btnNum = 1;
			pDesc.btnText = (const char**)chars;
			pDesc.window = &win;
			pDesc.bodyTextStyle = textStyle;
			pDesc.buttonTextStyle = buttonStyle;

			Window::popup(pDesc, theme);

			delete result;
			deleteGraph(index);
		}

		equation->cleanUp();
		delete equation;
	};

	std::function<void(void)> addGraphButtonCallback = [&]() -> void {
		if (buttons.size() >= 9) {
			return;
		}

		Rectangle boundsa = addGraphButton->getBounds();
		addGraphButton->setPos({ boundsa.x, boundsa.y - 60 });

		GraphData* data = (GraphData*)malloc(sizeof(GraphData));

		int bSize = buttons.size();

		TextBox* textBox = new TextBox({ 10, (float)(560 - 60.0f * buttons.size()) + 10, 335, 30 }, layout, { textStyle , 1, 2, theme });

		textBox->setEnterFunc([bSize, textBox, setGraph]() -> void { setGraph(textBox->m_text, bSize); });

		Button* button = new Button({ 355, (float)(560 - 60.0f * buttons.size()) + 10, 25, 30 }, layout, "X", { textStyle,
			[&addGraphButton, &buttons, &deleteGraph, &graphs, &textBoxes, data]()->void {
				data->del = true;
			}, 
		2, theme
		});

		data->button = button;
		data->textBox = textBox;
		data->graph = NULL;
		data->del = false;

		textBoxes.push_back(textBox);
		buttons.push_back(button);
		graphs.push_back(NULL);
		datas.push_back(data);
	};

	addGraphButton = new Button({ 10 - 2, 570, 360 / 2, 40 }, layout, "Add Graph", { buttonStyle, addGraphButtonCallback, 3, theme });

	addGraphButtonCallback();

	g_colors[0] = { 0.8f, 0.2f , 0.2f };
	g_colors[1] = { 0.1f, 0.6f , 0.1f };
	g_colors[2] = { 0.2f, 0.2f , 0.9f };
	g_colors[3] = { 0.9f, 0.65f, 0.2f };
	g_colors[4] = { 0.8f, 0.2f , 0.8f };

	while (win.isOpen()) {
		win.poll();
		
		panel->poll();
		addGraphButton->poll();
		
		for (int i = 0; i < buttons.size();i++) {
			buttons[i]->setPos({ buttons[i]->getBounds().x, (float)(560 - 60.0f * i) + 10 });
			textBoxes[i]->setPos({ textBoxes[i]->getBounds().x, (float)(560 - 60.0f * i) + 10 });
		}

		for (Button* b:buttons) {
			b->poll();
		}

		for (TextBox* t:textBoxes) {
			t->poll();
		}

		for (int i = 0; i < graphs.size();i++) {
			if (datas[i]->del) {
				Rectangle bounds = addGraphButton->getBounds();
				addGraphButton->setPos({ bounds.x, bounds.y + 60 });

				deleteGraph(i);
				graphs.erase(graphs.begin() + i);

				delete buttons[i];
				buttons.erase(buttons.begin() + i);

				delete textBoxes[i];
				textBoxes.erase(textBoxes.begin() + i);

				free(datas[i]);
				datas.erase(datas.begin() + i);

				break;
			}
		}

		if (graphs.size() < 1) {
			addGraphButtonCallback();
		}

		Renderer::clear({0.6f, 0.75f, 1});

		panel->render();
		addGraphButton->render();

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
