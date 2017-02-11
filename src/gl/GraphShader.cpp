#include "GraphShader.h"
#include <vector>
#include <iostream>

static inline std::string getCalcVertSource42() {
	std::string result = "";

	result += "#version 420 core\n";
	result += "in vec2 position;\n";
	result += "out vec2 coord;\n";
	result += "uniform float up;\n";
	result += "uniform float down;\n";
	result += "uniform float left;\n";
	result += "uniform float right;\n";
	result += "void main(void) {\n";
	result += "gl_Position = vec4(position.xy, 0, 1);\n";
	result += "coord = vec2(0.0, 0.0);\n";

	result += "if(position.x == 1) {coord.x = right;}";
	result += "else {coord.x = left;}";

	result += "if(position.y == 1) {coord.y = up;}";
	result += "else {coord.y = down;}";

	result += "}\n";

	return result;
}

static inline std::string getCalcFragSource42(std::string eq) {
	std::string result = "";

	result += "#version 420 core\n";
	result += "uniform float t;\n";
	result += "uniform float at;\n";

	result += "layout (rg32f) uniform image2D data;\n";

	result += "const float e = 2.718281828459045;\n";
	result += "const float pi = 3.141592653589793;\n";
	result += "const float tau = 3.141592653589793 * 2;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "float powi_c(float b, int p) {";

	result += "if(p%2 == 0) {";
	result += "return pow(abs(b), p);";
	result += "}";

	result += "return sign(b)*pow(abs(b), p);";
	result += "}\n";

	result += "float pow_c(float b, float p) {\n";

	result += "int pi = int(p);";

	result += "if(p < 0) {";
	result += "if(pi == p) {";
	result += "return 1/(powi_c(b, abs(pi)));";
	result += "} else {";
	result += "return 1/(pow(b, abs(p)));";
	result += "}\n";
	result += "}\n";

	result += "if(pi == p) {";
	result += "return powi_c(b, pi);";
	result += "} else {";
	result += "return pow(b, p);";
	result += "}\n";

	result += "}\n";

	result += "void main(void) {\n";
	result += "float x0 = coord.x;\n";
	result += "float y0 = coord.y;\n";

	/**
		This commented section is some code that I wrote to test polar coordinates. I'll add them later.
	*/

	/*
	result += "float x = atan(y1/x1);\n";
	result += "float y = sqrt(x1*x1 + y1*y1);\n";

	result += "if(x1 < 0 && y1 > 0) {";
	result += "x = x + pi;";
	result += "}";

	result += "else if(x1 < 0 && y1 < 0) {";
	result += "x = x + pi;";
	result += "}";

	result += "else if(x1 > 0 && y1 < 0) {";
	result += "x = x + pi*2;";
	result += "}";
	*/

	result += "float x = x0;\n";
	result += "float y = y0;\n";

	result += "float total = " + eq + ";\n";

	result += "y = 0;\n";

	result += "float total0 = " + eq + ";\n";

	result += "imageStore(data, ivec2(gl_FragCoord.xy), vec4(total, total0, 0.0, 0.0));\n";

	result += "discard;\n";
	result += "}\n";

	return result;
}

static inline std::string getCalcVertSource33() {
	std::string result = "";

	result += "#version 330 core\n";
	result += "in vec2 position;\n";
	result += "out vec2 coord;\n";
	result += "uniform float up;\n";
	result += "uniform float down;\n";
	result += "uniform float left;\n";
	result += "uniform float right;\n";
	result += "void main(void) {\n";
	result += "gl_Position = vec4(position.xy, 0, 1);\n";
	result += "coord = vec2(0.0, 0.0);\n";

	result += "if(position.x == 1) {coord.x = right;}";
	result += "else {coord.x = left;}";

	result += "if(position.y == 1) {coord.y = up;}";
	result += "else {coord.y = down;}";

	result += "}\n";

	return result;
}

static inline std::string getCalcFragSource33(std::string eq) {
	std::string result = "";

	result += "#version 330 core\n";
	result += "uniform float t;\n";
	result += "uniform float at;\n";

	result += "const float e = 2.718281828459045;\n";
	result += "const float pi = 3.141592653589793;\n";
	result += "const float tau = 3.141592653589793 * 2;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "float powi_c(float b, int p) {";

	result += "if(p%2 == 0) {";
	result += "return pow(abs(b), p);";
	result += "}";

	result += "return sign(b)*pow(abs(b), p);";
	result += "}\n";

	result += "float pow_c(float b, float p) {\n";

	result += "int pi = int(p);";

	result += "if(p < 0) {";
	result += "if(pi == p) {";
	result += "return 1/(powi_c(b, abs(pi)));";
	result += "} else {";
	result += "return 1/(pow(b, abs(p)));";
	result += "}\n";
	result += "}\n";

	result += "if(pi == p) {";
	result += "return powi_c(b, pi);";
	result += "} else {";
	result += "return pow(b, p);";
	result += "}\n";

	result += "}\n";

	result += "void main(void) {\n";
	result += "float x = coord.x;\n";
	result += "float y = coord.y;\n";
	result += "float total = " + eq + ";\n";
	result += "y = 0;\n";
	result += "float total0 = " + eq + ";\n";

	result += "out_color = vec4(total, total0, 0.0, 1.0);\n";
	result += "}\n";

	return result;
}

GraphCalcShader::GraphCalcShader(std::string eq, bool gl42) {
	
	std::string vertSource;
	std::string fragSource;

	if (gl42) {
		vertSource = getCalcVertSource42();
		fragSource = getCalcFragSource42(eq);
	} else {
		vertSource = getCalcVertSource33();
		fragSource = getCalcFragSource33(eq);
	}

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

	GLuint dataLoc = glGetUniformLocation(shaderProgram, "data");

	glUseProgram(shaderProgram);

	glUniform1i(dataLoc, 0);

	glUseProgram(0);
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

void GraphCalcShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

static inline std::string getEdgeVertSource42() {
	std::string result = "";

	result += "#version 420 core\n";
	result += "out vec2 coord;\n";
	result += "in vec2 position;\n";
	result += "void main(void) {\n";
	result += "gl_Position = vec4(position.xy, 0, 1);\n";
	result += "coord = position/2.0 + 0.5;\n";
	result += "}\n";

	return result;
}

static inline std::string getEdgeFragSource42(int portSize) {
	std::string result = "";

	result += "#version 420 core\n";

	result += "layout (rg32f) uniform image2D data;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "const int portWidth = ";
	result += std::to_string(portSize);
	result += ";\n";

	result += "bool isColored(ivec2 coord) {";

	result += "vec4 c = imageLoad(data, coord);\n";

	result += "vec4 u = imageLoad(data, ivec2(coord.x, coord.y + 1));\n";
	result += "vec4 d = imageLoad(data, ivec2(coord.x, coord.y - 1));\n";
	result += "vec4 u2 = imageLoad(data, ivec2(coord.x, coord.y + 2));\n";

	//Up Pixel
	result += "if(coord.y + 2 <= portWidth && coord.y - 1 >= 0) {";

	result += "float m1 = (c.x - d.x);";
	result += "float m2 = (u2.x - u.x);";
	result += "float m = (u.x - c.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "if(sign(u.x) != sign(c.x) && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "float m10 = (c.y - d.y);";
	result += "float m20 = (u2.y - u.y);";
	result += "float m0 = (u.y - c.y);";

	result += "bool p10 = m10 > 0;";
	result += "bool p20 = m20 > 0;";
	result += "bool p0 = m0 > 0;";

	result += "bool sk0 = p10 == p20 && p10 != p0;";

	result += "if(sign(u.x) == sign(c.x) && sign(u.y) != sign(c.y) && sk0) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";
	
	result += "vec4 d2 = imageLoad(data, ivec2(coord.x, coord.y - 2));\n";
	
	//Down Pixel
	result += "if(coord.y + 1 <= portWidth && coord.y - 2 >= 0) {";

	result += "float m1 = (d.x - d2.x);";
	result += "float m2 = (u.x - c.x);";
	result += "float m = (c.x - d.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "if(sign(c.x) != sign(d.x) && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "float m10 = (d.y - d2.y);";
	result += "float m20 = (u.y - c.y);";
	result += "float m0 = (c.y - d.y);";

	result += "bool p10 = m10 > 0;";
	result += "bool p20 = m20 > 0;";
	result += "bool p0 = m0 > 0;";

	result += "bool sk0 = p10 == p20 && p10 != p0;";

	result += "if(sign(c.x) == sign(d.x) && sign(c.y) != sign(d.y) && sk0) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";
	
	result += "vec4 r = imageLoad(data, ivec2(coord.x + 1, coord.y));\n";
	result += "vec4 l = imageLoad(data, ivec2(coord.x - 1, coord.y));\n";
	result += "vec4 r2 = imageLoad(data, ivec2(coord.x + 2, coord.y));\n";
	
	//Right Pixel
	result += "if(coord.x + 2 <= portWidth && coord.x - 1 >= 0) {";

	result += "float m1 = (c.x - l.x);";
	result += "float m2 = (r2.x - r.x);";
	result += "float m = (r.x - c.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "if(sign(r.x) != sign(c.x) && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "float m10 = (c.y - l.y);";
	result += "float m20 = (r2.y - r.y);";
	result += "float m0 = (r.y - c.y);";

	result += "bool p10 = m10 > 0;";
	result += "bool p20 = m20 > 0;";
	result += "bool p0 = m0 > 0;";

	result += "bool sk0 = p10 == p20 && p10 != p0;";

	result += "if(sign(r.x) == sign(c.x) && sign(r.y) != sign(c.y) && sk0) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";
	
	result += "vec4 l2 = imageLoad(data, ivec2(coord.x - 2, coord.y));\n";
	
	//Left Pixel
	result += "if(coord.x + 1 <= portWidth && coord.x - 2 >= 0) {";

	result += "float m1 = (l.x - l2.x);";
	result += "float m2 = (r.x - c.x);";
	result += "float m = (c.x - l.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "if(sign(c.x) != sign(l.x) && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "float m10 = (l.y - l2.y);";
	result += "float m20 = (r.y - c.y);";
	result += "float m0 = (c.y - l.y);";

	result += "bool p10 = m10 > 0;";
	result += "bool p20 = m20 > 0;";
	result += "bool p0 = m0 > 0;";

	result += "bool sk0 = p10 == p20 && p10 != p0;";

	result += "if(sign(c.x) == sign(l.x) && sign(c.y) != sign(l.y) && sk0) {";
	result += "return true;\n";
	result += "}\n";
	
	result += "}\n";

	result += "return false;\n";

	result += "}";

	result += "void main(void) {\n";

	result += "if(isColored(ivec2(gl_FragCoord.xy))) {out_color = vec4(1.0, 0.0, 0.0, 1.0);}";
	result += "else {out_color = vec4(0.0, 0.0, 0.0, 1.0);}";

	result += "}\n";

	return result;
}

static inline std::string getEdgeVertSource33() {
	std::string result = "";

	result += "#version 330 core\n";
	result += "out vec2 coord;\n";
	result += "in vec2 position;\n";
	result += "void main(void) {\n";
	result += "gl_Position = vec4(position.xy, 0, 1);\n";
	result += "coord = position/2.0 + 0.5;\n";
	result += "}\n";

	return result;
}

static inline std::string getEdgeFragSource33(int portSize) {
	std::string result = "";

	result += "#version 330 core\n";

	result += "uniform sampler2D data;\n";
	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "const float pxw = 1.0/";
	result += std::to_string(portSize);
	result += ".0;\n";

	result += "bool isColored(vec2 coord) {";

	result += "vec4 c = texture(data, coord);\n";

	result += "if(c.x == 0) {";
	result += "return true;\n";
	result += "}\n";

	result += "vec4 u = texture(data, vec2(coord.x, coord.y - pxw));\n";
	result += "vec4 d = texture(data, vec2(coord.x, coord.y + pxw));\n";
	result += "vec4 u2 = texture(data, vec2(coord.x, coord.y - pxw*2));\n";
	
	//Up Pixel
	result += "if(coord.y + pxw*2 <= 1.0 && coord.y - pxw >= 0.0) {";

	result += "float m1 = (c.x - d.x);";
	result += "float m2 = (u2.x - u.x);";
	result += "float m = (u.x - c.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "float m1n = -(c.x - d.x);";
	result += "float m2n = -(u2.x - u.x);";
	result += "float mn = -(u.x - c.x);";

	result += "bool p1n = m1n > 0;";
	result += "bool p2n = m2n > 0;";
	result += "bool pn = mn > 0;";

	result += "bool skn = p1n == p2n && p1n != pn;";

	result += "if(sign(u.x) != sign(c.x) && !sk && !skn) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";
	
	result += "vec4 d2 = texture(data, vec2(coord.x, coord.y + pxw*2));\n";

	//Down Pixel
	result += "if(coord.y + pxw*2 <= 1.0 && coord.y - pxw >= 0.0) {";

	result += "float m1 = (d.x - d2.x);";
	result += "float m2 = (u.x - c.x);";
	result += "float m = (c.x - d.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "float m1n = -(d.x - d2.x);";
	result += "float m2n = -(u.x - c.x);";
	result += "float mn = -(c.x - d.x);";

	result += "bool p1n = m1n > 0;";
	result += "bool p2n = m2n > 0;";
	result += "bool pn = mn > 0;";

	result += "bool skn = p1n == p2n && p1n != pn;";

	result += "if(sign(c.x) != sign(d.x) && !sk && !skn) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";
	
	result += "vec4 r = texture(data, vec2(coord.x + pxw, coord.y));\n";
	result += "vec4 l = texture(data, vec2(coord.x - pxw, coord.y));\n";
	result += "vec4 r2 = texture(data, vec2(coord.x + pxw*2, coord.y));\n";
	
	//Right Pixel
	result += "if(coord.x + pxw*2 <= 1.0 && coord.x - pxw >= 0.0) {";

	result += "float m1 = (c.x - l.x);";
	result += "float m2 = (r2.x - r.x);";
	result += "float m = (r.x - c.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "float m1n = -(c.x - l.x);";
	result += "float m2n = -(r2.x - r.x);";
	result += "float mn = -(r.x - c.x);";

	result += "bool p1n = m1n > 0;";
	result += "bool p2n = m2n > 0;";
	result += "bool pn = mn > 0;";

	result += "bool skn = p1n == p2n && p1n != pn;";

	result += "if(sign(r.x) != sign(c.x) && !sk && !skn) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";
	
	result += "vec4 l2 = texture(data, vec2(coord.x - pxw*2, coord.y));\n";

	//Left Pixel
	result += "if(coord.x + pxw*2 <= 1.0 && coord.x - pxw >= 0.0) {";

	result += "float m1 = (l.x - l2.x);";
	result += "float m2 = (r.x - c.x);";
	result += "float m = (c.x - l.x);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p;";

	result += "float m1n = -(l.x - l2.x);";
	result += "float m2n = -(r.x - c.x);";
	result += "float mn = -(c.x - l.x);";

	result += "bool p1n = m1n > 0;";
	result += "bool p2n = m2n > 0;";
	result += "bool pn = mn > 0;";

	result += "bool skn = p1n == p2n && p1n != pn;";

	result += "if(sign(c.x) != sign(l.x) && !sk && !skn) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";

	result += "return false;\n";

	result += "}";

	result += "void main(void) {\n";
	result += "if(isColored(coord)) {out_color = vec4(1.0, 0.0, 0.0, 1.0); return;}\n";
	result += "out_color = vec4(0.0, 0.0, 0.0, 1.0);";
	result += "}\n";

	return result;
}

GraphEdgeShader::GraphEdgeShader(bool gl42, int portSize) {
	std::string vertSource;
	std::string fragSource;

	if (gl42) {
		vertSource = getEdgeVertSource42();
		fragSource = getEdgeFragSource42(portSize);
	}
	else {
		vertSource = getEdgeVertSource33();
		fragSource = getEdgeFragSource33(portSize);
	}

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

	dataLoc = glGetUniformLocation(shaderProgram, "data");
	//edgeLoc = glGetUniformLocation(shaderProgram, "edge");

	bind();

	glUniform1i(dataLoc, 0);
	//glUniform1i(edgeLoc, 1);

	unbind();
}

void GraphEdgeShader::bind() {
	glUseProgram(shaderProgram);
}

void GraphEdgeShader::unbind() {
	glUseProgram(0);
}

void GraphEdgeShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

static inline std::string getRenderVertSource() {
	std::string result = "";

	result += "#version 330 core\n";
	result += "out vec2 coord;\n";
	result += "in vec2 position;\n";
	result += "void main(void) {\n";
	result += "gl_Position = vec4(position.xy, 0, 1);\n";
	result += "coord = position/2.0 + 0.5;\n";
	result += "}\n";

	return result;
}

static inline std::string getRenderFragSource(int portSize) {
	std::string result = "";

	result += "#version 330 core\n";

	result += "uniform sampler2D edge;\n";
	result += "uniform vec3 g_color;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "const float pw = 1.0/";
	result += std::to_string(portSize);
	result += ".0;\n";

	result += "void main(void) {\n";
	result += "if(texture(edge, vec2(coord.x, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "if(texture(edge, vec2(coord.x + pw, coord.y)).x == 1.0 && coord.x + pw <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x - pw, coord.y)).x == 1.0 && coord.x - pw >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y + pw)).x == 1.0 && coord.y + pw <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y - pw)).x == 1.0 && coord.y - pw >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "if(texture(edge, vec2(coord.x + pw*2, coord.y)).x == 1.0 && coord.x + pw*2 <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x - pw*2, coord.y)).x == 1.0 && coord.x - pw*2 >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y + pw*2)).x == 1.0 && coord.y + pw*2 <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y - pw*2)).x == 1.0 && coord.y - pw*2 >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "discard;";
	result += "}\n";

	return result;
}

GraphRenderShader::GraphRenderShader(int portSize) {
	std::string vertSource;
	std::string fragSource;

	vertSource = getRenderVertSource();
	fragSource = getRenderFragSource(portSize);

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

	edgeLoc = glGetUniformLocation(shaderProgram, "edge");
	colorLoc = glGetUniformLocation(shaderProgram, "g_color");

	bind();

	glUniform1i(edgeLoc, 0);
	
	unbind();
}

void GraphRenderShader::setUniforms(glui::Color graphColor) {
	glUniform3f(colorLoc, graphColor.r, graphColor.g, graphColor.b);
}

void GraphRenderShader::bind() {
	glUseProgram(shaderProgram);
}

void GraphRenderShader::unbind() {
	glUseProgram(0);
}

void GraphRenderShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

