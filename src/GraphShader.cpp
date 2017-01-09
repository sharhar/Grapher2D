#include "GraphShader.h"
#include <vector>
#include <iostream>

#define glCreateProgram funcs->glCreateProgram
#define glCreateShader funcs->glCreateShader
#define glShaderSource funcs->glShaderSource
#define glCompileShader funcs->glCompileShader
#define glGetShaderiv funcs->glGetShaderiv
#define glGetShaderInfoLog funcs->glGetShaderInfoLog
#define glAttachShader funcs->glAttachShader
#define glLinkProgram funcs->glLinkProgram
#define glValidateProgram funcs->glValidateProgram
#define glBindAttribLocation funcs->glBindAttribLocation
#define glUseProgram funcs->glUseProgram
#define glGetUniformLocation funcs->glGetUniformLocation
#define glUniform1f funcs->glUniform1f
#define glUniform3f funcs->glUniform3f
#define glUniform1i funcs->glUniform1i
#define glDetachShader funcs->glDetachShader
#define glDeleteShader funcs->glDeleteShader
#define glDeleteProgram funcs->glDeleteProgram

static GraphShaderFuncs* getFuncs() {
	GraphShaderFuncs* funcs = (GraphShaderFuncs*)malloc(sizeof(GraphShaderFuncs));

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
	glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
	glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)glfwGetProcAddress("glValidateProgram");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)glfwGetProcAddress("glBindAttribLocation");
	glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
	glUniform1f = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f");
	glUniform3f = (PFNGLUNIFORM3FPROC)glfwGetProcAddress("glUniform3f");
	glUniform1i = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i");
	glDetachShader = (PFNGLDETACHSHADERPROC)glfwGetProcAddress("glDetachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
	
	return funcs;
}

GraphCalcShader::GraphCalcShader(std::string eq) {
	funcs = getFuncs();

	std::string vertSource = "";

	vertSource += "#version 330 core\n";
	vertSource += "in vec2 position;\n";
	vertSource += "out vec2 coord;\n";
	vertSource += "uniform float up;\n";
	vertSource += "uniform float down;\n";
	vertSource += "uniform float left;\n";
	vertSource += "uniform float right;\n";
	vertSource += "void main(void) {\n";
	vertSource += "gl_Position = vec4(position.xy, 0, 1);\n";
	vertSource += "coord = vec2(0.0, 0.0);\n";

	vertSource += "if(position.x == 1) {coord.x = right;}";
	vertSource += "else {coord.x = left;}";

	vertSource += "if(position.y == 1) {coord.y = up;}";
	vertSource += "else {coord.y = down;}";

	vertSource += "}\n";

	std::string fragSource = "";

	fragSource += "#version 330 core\n";
	fragSource += "uniform float t;\n";
	fragSource += "uniform float at;\n";

	fragSource += "const float e = 2.718281828459045;\n";
	fragSource += "const float pi = 3.141592653589793;\n";
	fragSource += "const float tau = 3.141592653589793 * 2;\n";

	//const float myfloat = 10.0;

	//fragSource += "float packColor(vec4 color) {return ((color.a*2 -1 ) / 256.0 + (color.r*2 - 1) + (color.g*2 - 1) + (color.b*2 - 1) * 256.0) * 256.0;}\n";
	//fragSource += "vec4 unpackColor(float f) {";
	//fragSource += "vec4 color;";
	//fragSource += "color.b = floor(f / 256.0); color.g = floor((f - color.b * 256.0)); color.r = floor((f - color.b * 256.0 - color.g)*256.0); color.a = floor((f - color.b * 256.0 - color.g - color.r / 256.0)*256.0*256.0);";
	//fragSource += "color = color / 256.0;\n";
	//fragSource += "return color / 2 + 0.5;}\n";

	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";
	fragSource += "void main(void) {\n";
	fragSource += "float x = coord.x;\n";
	fragSource += "float y = coord.y;\n";
	fragSource += "float total = " + eq + ";\n";
	fragSource += "out_color = vec4(sign(total)/2 + 0.5, 0.0, 0.0, 1.0);\n";
	fragSource += "}\n";

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
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, message);
		
		std::cout << "Vertex Shader failed to compile:\n";
		std::cout << message << "\n";

		glDeleteShader(vertexShader);
		return;
	}

	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, message);
		
		std::cout << "Fragment Shader failed to compile:\n";
		std::cout << message << "\n";

		glDeleteShader(fragmentShader);
		return;
	}

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glBindAttribLocation(shaderProgram, 0, "position");

	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	upLoc = glGetUniformLocation(shaderProgram, "up");
	downLoc = glGetUniformLocation(shaderProgram, "down");
	leftLoc = glGetUniformLocation(shaderProgram, "left");
	rightLoc = glGetUniformLocation(shaderProgram, "right");
	tLoc = glGetUniformLocation(shaderProgram, "t");
	atLoc = glGetUniformLocation(shaderProgram, "at");
}

void GraphCalcShader::setUniforms(float up, float down, float left, float right, float time, float atime) {
	glUniform1f(upLoc, up);
	glUniform1f(downLoc, down);
	glUniform1f(leftLoc, left);
	glUniform1f(rightLoc, right);
	glUniform1f(tLoc, time);
	glUniform1f(atLoc, atime);
}

void GraphCalcShader::bind() {
	glUseProgram(shaderProgram);
}

void GraphCalcShader::unbind() {
	glUseProgram(0);
}

GraphRenderShader::GraphRenderShader() {
	funcs = getFuncs();

	std::string vertSource = "";

	vertSource += "#version 330 core\n";
	vertSource += "out vec2 coord;\n";
	vertSource += "in vec2 position;\n";
	vertSource += "void main(void) {\n";
	vertSource += "gl_Position = vec4(position.xy, 0, 1);\n";
	vertSource += "coord = position/2.0 + 0.5;\n";
	vertSource += "}\n";

	std::string fragSource = "";

	fragSource += "#version 330 core\n";
	fragSource += "uniform sampler2D tex;\n";
	fragSource += "uniform vec3 g_color;\n";
	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";
	fragSource += "void main(void) {\n";

	fragSource += "int width = 2;\n";

	fragSource += "float pxwb = 1.0/600.0;\n";
	fragSource += "vec4 ct = texture(tex, coord);\n";
	fragSource += "float c = (ct.x-0.5)*2;\n";

	fragSource += "int ci = 0;";

	fragSource += "for(int i = 1; i <= width; i++) {\n";

	fragSource += "float pxw = pxwb*i;\n";

	fragSource += "float r = (texture(tex, vec2(coord.x + pxw, coord.y)).x-0.5)*2;\n";
	fragSource += "float u = (texture(tex, vec2(coord.x, coord.y + pxw)).x-0.5)*2;\n";
	fragSource += "float l = (texture(tex, vec2(coord.x - pxw, coord.y)).x-0.5)*2;\n";
	fragSource += "float d = (texture(tex, vec2(coord.x, coord.y - pxw)).x-0.5)*2;\n";

	fragSource += "if(c == 0 || (coord.x + pxw <= 1.0 && c != r) || (coord.y + pxw <= 1.0 && c != u) ||";
	fragSource += "	(coord.x - pxw >= 0.0 && c != l) || (coord.y - pxw >= 0.0 && c != d)) {out_color = vec4(g_color.xyz, 1.0); ci = 1; break;}\n";

	fragSource += "}\n";

	fragSource += "if(ci == 0) {out_color = vec4(0.0, 0.0, 0.0, 0.0);}";

	fragSource += "}\n";

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
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, message);

		std::cout << "Vertex Shader failed to compile:\n";
		std::cout << message << "\n";

		glDeleteShader(vertexShader);
		return;
	}

	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, message);

		std::cout << "Fragment Shader failed to compile:\n";
		std::cout << message << "\n";

		glDeleteShader(fragmentShader);
		return;
	}

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glBindAttribLocation(shaderProgram, 0, "position");

	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	texLoc = glGetUniformLocation(shaderProgram, "tex");
	colorLoc = glGetUniformLocation(shaderProgram, "g_color");
}

void GraphRenderShader::bind() {
	glUseProgram(shaderProgram);
}

void GraphRenderShader::unbind() {
	glUseProgram(0);
}

void GraphRenderShader::setUniforms(GLuint tex, glui::Color graphColor) {
	glUniform1i(texLoc, tex);
	glUniform3f(colorLoc, graphColor.r, graphColor.g, graphColor.b);
}

void GraphRenderShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);

	free(funcs);
}

void GraphCalcShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);

	free(funcs);
}