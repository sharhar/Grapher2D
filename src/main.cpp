#include <GLUI/GLUI.h>
#include "math/Equation.h"
#include <iostream>
#include <math.h>
#include <string>
#include <thread>
#include <chrono>
#include "gl/GLGraph.h"

#if defined(_WIN32) && !defined(_DEBUG)
#include <windows.h>
#endif

using namespace glui;

#define ZOOM_PERCENT 0.025

double g_left  = -6;
double g_right =  6;
double g_down  = -6;
double g_up    =  6;

bool g_gl42;

int g_windowWidth = 600;
int g_windowHeight = 600;

int g_colorNum = 5;
Color* g_colors = new Color[g_colorNum];

GLuint g_gtex;
GLuint g_fbo;

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

void drawAxes(float* modelMat, GLuint modelLoc, int width, int height) {
	double l = abs_c(g_left);
	double r = abs_c(g_right);

	double xoff = (l / (r + l))* width;

	if (g_left < 0 && g_right > 0) {
		Utils::getModelviewMatrix(modelMat, xoff, 300, 1, 600);
		glUniformMatrix4fv(modelLoc, 1, false, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	double d = abs_c(g_down);
	double u = abs_c(g_up);

	double yoff = (d / (u + d))* height;

	if (g_up > 0 && g_down < 0) {
		Utils::getModelviewMatrix(modelMat, 300, yoff, 600, 1);
		glUniformMatrix4fv(modelLoc, 1, false, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawGrid(float* modelMat, GLuint modelLoc, double xl, double xr, double yd, double yu, int width, int height) {
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
	for (int i = 0; i < xNum+1;i++) {
		double tx = (xlr - xl + xmag*i)*xratio;

		if (tx  < 0 || tx > width) {
			continue;
		}

		Utils::getModelviewMatrix(modelMat, tx, 300, 0.25f, 600);
		glUniformMatrix4fv(modelLoc, 1, false, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
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
		double ty = (ylr - yd + ymag*i)*yratio;

		if (ty < 0 || ty > height) {
			continue;
		}
		Utils::getModelviewMatrix(modelMat, 300, ty, 600, 0.25f);
		glUniformMatrix4fv(modelLoc, 1, false, modelMat);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void drawNums(double xl, double xr, double yd, double yu, int width, int height, Font* font, Color color, GLuint modelLoc) {
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
        
		Renderer::drawStringCustom(str, modelLoc, tx + 2, 10, 15, font, color);
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

		Renderer::drawStringCustom(str, modelLoc, 10, ty + 2, 15, font, color);
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

bool hasGL42() {
    std::string vertSource = "";
    
    vertSource += "#version 420 core\n";
    vertSource += "void main(void) {\n";
    vertSource += "gl_Position = vec4(1, 1, 1, 1);\n";
    vertSource += "}\n";
    
    std::string fragSource = "";
    
    fragSource += "#version 420 core\n";
    fragSource += "void main(void) {\n";
    fragSource += "discard;\n";
    fragSource += "}\n";
    
    GLuint vertexShader;
    GLuint fragmentShader;
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* shadersource = (GLchar*)vertSource.c_str();
    glShaderSource(vertexShader, 1, &shadersource, 0);
    shadersource = (GLchar*)fragSource.c_str();
    glShaderSource(fragmentShader, 1, &shadersource, 0);
    
    glCompileShader(vertexShader);
    
    GLint compiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        glDeleteShader(fragmentShader);
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

GLuint getQuadShader() {
    std::string vertSource = "";
    
    vertSource += "#version 330 core\n";
    
    vertSource += "in vec2 position;\n";
    vertSource += "in vec2 texcoord;\n";
    vertSource += "out vec2 texcoord_out;\n";
    
    vertSource += "void main(void) {\n";
    vertSource += "gl_Position = vec4(position.x*2, position.y*2, 0, 1);\n";
    vertSource += "texcoord_out = texcoord;\n";
    vertSource += "}\n";
    
    std::string fragSource = "";
    
    fragSource += "#version 330 core\n";
    fragSource += "out vec4 out_color;\n";
    
    fragSource += "uniform sampler2D tex;\n";
    fragSource += "in vec2 texcoord_out;\n";
    
    fragSource += "void main(void) {\n";
    fragSource += "out_color = texture(tex, texcoord_out);\n";
    fragSource += "}\n";
    
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* shadersource = (GLchar*)vertSource.c_str();
    glShaderSource(vertShader, 1, &shadersource, 0);
    shadersource = (GLchar*)fragSource.c_str();
    glShaderSource(fragShader, 1, &shadersource, 0);
    
    glCompileShader(vertShader);
    
    GLint compiled = 0;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, message);
        
        std::cout << "Vertex Shader failed to compile:\n";
        std::cout << message << "\n";
        
        glDeleteShader(vertShader);
        return -1;
    }
    
    glCompileShader(fragShader);
    
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, message);
        
        std::cout << "Fragment Shader failed to compile:\n";
        std::cout << message << "\n";
        
        glDeleteShader(fragShader);
        return -1;
    }
    
    GLuint shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    
    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "texcoord");
    
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    
    return shaderProgram;
}

GLuint getNumberShader() {
    std::string vertSource = "";
    
    vertSource += "#version 330 core\n";
    
    vertSource += "in vec2 position;\n";
    vertSource += "in vec2 texcoord;\n";
    vertSource += "out vec2 texcoord_out;\n";
    
    vertSource += "uniform mat4 projection;\n";
    vertSource += "uniform mat4 modelview;\n";
    
    vertSource += "void main(void) {\n";
    vertSource += "gl_Position = projection * modelview * vec4(position.x, position.y, 0, 1);\n";
    vertSource += "texcoord_out = texcoord;\n";
    vertSource += "}\n";
    
    std::string fragSource = "";
    
    fragSource += "#version 330 core\n";
    fragSource += "out vec4 out_color;\n";
    
    fragSource += "uniform sampler2D tex;\n";
    fragSource += "in vec2 texcoord_out;\n";
    
    fragSource += "void main(void) {\n";
    fragSource += "out_color = vec4(0.0, 0.0, 0.0, texture(tex, texcoord_out).r);\n";
    fragSource += "}\n";
    
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* shadersource = (GLchar*)vertSource.c_str();
    glShaderSource(vertShader, 1, &shadersource, 0);
    shadersource = (GLchar*)fragSource.c_str();
    glShaderSource(fragShader, 1, &shadersource, 0);
    
    glCompileShader(vertShader);
    
    GLint compiled = 0;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, message);
        
        std::cout << "Vertex Shader failed to compile:\n";
        std::cout << message << "\n";
        
        glDeleteShader(vertShader);
        return -1;
    }
    
    glCompileShader(fragShader);
    
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);
        
        GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, message);
        
        std::cout << "Fragment Shader failed to compile:\n";
        std::cout << message << "\n";
        
        glDeleteShader(fragShader);
        return -1;
    }
    
    GLuint shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    
    glBindAttribLocation(shaderProgram, 0, "position");
    glBindAttribLocation(shaderProgram, 1, "texcoord");
    
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    
    return shaderProgram;
}

GLuint getLineShader() {
	std::string vertSource = "";

	vertSource += "#version 330 core\n";

	vertSource += "in vec2 position;\n";

	vertSource += "uniform mat4 projection;\n";
	vertSource += "uniform mat4 modelview;\n";

	vertSource += "void main(void) {\n";
	vertSource += "gl_Position = projection * modelview * vec4(position.x, position.y, 0, 1);\n";
	vertSource += "}\n";

	std::string fragSource = "";

	fragSource += "#version 330 core\n";
	fragSource += "out vec4 out_color;\n";

	fragSource += "uniform vec3 color;\n";

	fragSource += "void main(void) {\n";
	fragSource += "out_color = vec4(color.xyz, 1.0);\n";
	fragSource += "}\n";

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLchar* shadersource = (GLchar*)vertSource.c_str();
	glShaderSource(vertShader, 1, &shadersource, 0);
	shadersource = (GLchar*)fragSource.c_str();
	glShaderSource(fragShader, 1, &shadersource, 0);

	glCompileShader(vertShader);

	GLint compiled = 0;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(vertShader, maxLength, &maxLength, message);

		std::cout << "Vertex Shader failed to compile:\n";
		std::cout << message << "\n";

		glDeleteShader(vertShader);
		return -1;
	}

	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(fragShader, maxLength, &maxLength, message);

		std::cout << "Fragment Shader failed to compile:\n";
		std::cout << message << "\n";

		glDeleteShader(fragShader);
		return -1;
	}

	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	glBindAttribLocation(shaderProgram, 0, "position");

	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	return shaderProgram;
}

int main() {
#if defined(_WIN32) && !defined(_DEBUG)
	FreeConsole();
#endif

	GLUI::init();

	Window win("Grapher2D", 1000, 620, false, 1, genIcon());
    
	g_gl42 = hasGL42();
    
	Renderer::init(&win);
	
    Layout* layout = new AbsoluteLayout(&win, 1000, 620);

	Font* font24 = new Font("arial.ttf", 24);

	if (!font24->inited()) {
		win.destroy();
		std::cout << "Could not load arial.tff!\n";
		return -1;
	}

	Font* font20 = new Font("arial.ttf", 20);

	if (!font20->inited()) {
		win.destroy();
		std::cout << "Could not load arial.tff!\n";
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

	glGenFramebuffers(1, &g_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &g_gtex);
	glBindTexture(GL_TEXTURE_2D, g_gtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1200, 1200,
		0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_gtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GraphQuad::init();
    
    GraphEdgeShader* edgeShader = new GraphEdgeShader(g_gl42);
	GraphRenderShader* renderShader = new GraphRenderShader(g_gl42);

    GLuint quadShader = getQuadShader();
    
    GLuint texLoc = glGetUniformLocation(quadShader, "tex");
    
    glUseProgram(quadShader);
    glUniform1i(texLoc, 0);
    glUseProgram(0);
    
    GLuint numberShader = getNumberShader();
    
    texLoc = glGetUniformLocation(numberShader, "tex");
    GLuint numProjLoc = glGetUniformLocation(numberShader, "projection");
    GLuint numModelLoc = glGetUniformLocation(numberShader, "modelview");
    
    float* numProj = new float[16];
    Utils::getOrthoMatrix(numProj, 0, 600, 0, 600, -1, 1);
    
    glUseProgram(numberShader);
    glUniform1i(texLoc, 0);
    glUniformMatrix4fv(numProjLoc, 1, false, numProj);
    glUseProgram(0);

	GLuint lineShader = getLineShader();

	GLuint lineColorLoc = glGetUniformLocation(lineShader, "color");
	GLuint lineProjLoc = glGetUniformLocation(lineShader, "projection");
	GLuint lineModelLoc = glGetUniformLocation(lineShader, "modelview");

	float* lineProj = new float[16];
	Utils::getOrthoMatrix(lineProj, 0, 600, 0, 600, -1, 1);

	float* lineModel = new float[16];
	Utils::getModelviewMatrix(lineModel, 0, 0, 1, 1);

	glUseProgram(lineShader);
	glUniform3f(lineColorLoc, 0, 0, 0);
	glUniformMatrix4fv(lineProjLoc, 1, false, lineProj);
	glUniformMatrix4fv(lineModelLoc, 1, false, lineModel);
	glUseProgram(0);

	GLPanel* panel;

	panel = new GLPanel({ 390, 10, 600, 600 }, { 600, 600 }, layout,
	[&]()->void {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        
		glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
		glViewport(0, 0, 1200, 1200);
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		GraphQuad::bind();
		glEnableVertexAttribArray(0);
		glUseProgram(lineShader);
		glUniform3f(lineColorLoc, 0.5f, 0.5f, 0.5f);
		drawGrid(lineModel, lineModelLoc, g_left, g_right, g_down, g_up, 600, 600);
		glUniform3f(lineColorLoc, 0, 0, 0);
		drawAxes(lineModel, lineModelLoc, 600, 600);		
		
		float time = glfwGetTime();
		
		if (g_gl42) {
			for (int i = 0; i < graphs.size(); i++) {
				if (graphs[i] == NULL) {
					continue;
				}

				graphs[i]->glg->calc(g_up, g_down, g_left, g_right, time - graphs[i]->startTime, time);
			}

			GLsync syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			glClientWaitSync(syncObj, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
			glDeleteSync(syncObj);
       
            edgeShader->bind();
            for (int i = 0; i < graphs.size(); i++) {
                if (graphs[i] == NULL) {
                    continue;
                }
                
                glBindImageTexture(0, graphs[i]->glg->dtex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                glBindImageTexture(1, graphs[i]->glg->etex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            edgeShader->unbind();

			syncObj = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			glClientWaitSync(syncObj, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
			glDeleteSync(syncObj);
            
            renderShader->bind();
            for (int i = 0; i < graphs.size(); i++) {
                if (graphs[i] == NULL) {
                    continue;
                }
                
                renderShader->setUniforms(g_colors[i % 5]);
                glBindImageTexture(0, graphs[i]->glg->etex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            renderShader->unbind();
		} else {
			for (int i = 0; i < graphs.size(); i++) {
				if (graphs[i] == NULL) {
					continue;
				}

				graphs[i]->glg->calc33(g_up, g_down, g_left, g_right, time - graphs[i]->startTime, time);
			}
            
			edgeShader->bind();
			for (int i = 0; i < graphs.size(); i++) {
				if (graphs[i] == NULL) {
					continue;
				}

				glBindFramebuffer(GL_FRAMEBUFFER, graphs[i]->glg->efbo);
                glClear(GL_COLOR_BUFFER_BIT);
				glBindTexture(GL_TEXTURE_2D, graphs[i]->glg->dtex);
				glActiveTexture(GL_TEXTURE0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			edgeShader->unbind();
            
			glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);

			renderShader->bind();
			for (int i = 0; i < graphs.size(); i++) {
				if (graphs[i] == NULL) {
					continue;
				}

				renderShader->setUniforms(g_colors[i % 5]);
				glBindTexture(GL_TEXTURE_2D, graphs[i]->glg->etex);
				glActiveTexture(GL_TEXTURE0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			renderShader->unbind();
		}
        
        glDisableVertexAttribArray(0);
        GraphQuad::unbind();

        
        Renderer::beginDraw();
        glUseProgram(numberShader);
        drawNums(g_left, g_right, g_down, g_up, 600, 600, font20, color::black, numModelLoc);
        
		glBindFramebuffer(GL_FRAMEBUFFER, panel->getFBO());
		glViewport(0, 0, 600, 600);
		glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(quadShader);
		glBindTexture(GL_TEXTURE_2D, g_gtex);
		glActiveTexture(GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

        Renderer::endDraw();
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
			double x1 = 0;
			double y1 = 0;
			double x2 = g_right - g_left;
			double y2 = g_up - g_down;
			
			double w1 = (data->pos.x / 600.0)*x2;
			double w2 = x2 - w1;

			if (data->scroll > 0) {
				x1 = 2 * ZOOM_PERCENT*w1;
				x2 = x2 - 2 * ZOOM_PERCENT*w2;
			} else if (data->scroll < 0){
				x1 = - 2 * ZOOM_PERCENT*w1;
				x2 = x2 + 2 * ZOOM_PERCENT*w2;
			}

			double h1 = ((600.0 - data->pos.y) / 600.0)*(y2);
			double h2 = y2 - ((600.0 - data->pos.y) / 600.0)*(y2);

			if (data->scroll > 0) {
				y1 = 2 * ZOOM_PERCENT*h1;
				y2 = y2 - 2 * ZOOM_PERCENT*h2;
			}
			else if (data->scroll < 0) {
				y1 = -2 * ZOOM_PERCENT*h1;
				y2 = y2 + 2 * ZOOM_PERCENT*h2;
			}


			g_right = g_left + x2;
			g_left = g_left + x1;
			g_up = g_down + y2;
			g_down = g_down + y1;
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

			graphs[index]->glg = new GLGraph(equation, g_gl42);
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

		glui::Rectangle boundsa = addGraphButton->getBounds();
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
				glui::Rectangle bounds = addGraphButton->getBounds();
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
        
        Renderer::beginDraw();

		Renderer::clear({0.6f, 0.75f, 1});

		panel->render();
		addGraphButton->render();

		for (Button* b : buttons) {
			b->render();
		}

		for (TextBox* t : textBoxes) {
			t->render();
		}
        
        Renderer::endDraw();

		win.swap();
	}

	for (int i = 0; i < graphs.size(); i++) {
		deleteGraph(i);
	}

	if (g_gl42) {
		edgeShader->cleanUp();
		renderShader->cleanUp();
	}
    
	win.destroy();

	GLUI::destroy();
	return 0;
}
