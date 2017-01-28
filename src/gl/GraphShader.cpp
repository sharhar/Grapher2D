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

	result += "layout (rgba32f) uniform image2D data;\n";

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
	result += "imageStore(data, ivec2(gl_FragCoord.xy), vec4(total, sign(total), x, y));\n";
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
	result += "out_color = vec4(sign(total)/2 + 0.5, 0.0, 0.0, 1.0);\n";
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

static inline std::string getEdgeFragSource42() {
	std::string result = "";

	result += "#version 420 core\n";

	result += "layout (rgba32f) uniform image2D data;\n";
	result += "layout (rg32f) uniform image2D edge;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "bool isColored(ivec2 coord) {";

	result += "vec4 c = imageLoad(data, coord);\n";

	result += "vec4 r = imageLoad(data, ivec2(coord.x + 1, coord.y));\n";
	result += "vec4 u = imageLoad(data, ivec2(coord.x, coord.y + 1));\n";
	result += "vec4 l = imageLoad(data, ivec2(coord.x - 1, coord.y));\n";
	result += "vec4 d = imageLoad(data, ivec2(coord.x, coord.y - 1));\n";

	result += "vec4 r2 = imageLoad(data, ivec2(coord.x + 2, coord.y));\n";
	result += "vec4 u2 = imageLoad(data, ivec2(coord.x, coord.y + 2));\n";
	result += "vec4 l2 = imageLoad(data, ivec2(coord.x - 2, coord.y));\n";
	result += "vec4 d2 = imageLoad(data, ivec2(coord.x, coord.y - 2));\n";

	result += "float mm = 10;";
	result += "float mm2 = 50000;";

	//Up Pixel
	result += "if(coord.y + 2 <= 1200 && coord.y - 1 >= 0) {";

	result += "float m1 = (c.x - d.x)/(c.z - d.z);";
	result += "float m2 = (u2.x - u.x)/(u2.z - u.z);";
	result += "float m = (u.x - c.x)/(u.z - c.z);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	result += "if(u.y != c.y && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	result += "if(u.y == c.y && sk2) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";

	//Down Pixel
	result += "if(coord.y + 1 <= 1200 && coord.y - 2 >= 0) {";

	result += "float m1 = (d.x - d2.x)/(d.z - d2.z);";
	result += "float m2 = (u.x - c.x)/(u.z - c.z);";
	result += "float m = (c.x - d.x)/(c.z - d.z);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	result += "if(c.y != d.y && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	result += "if(c.y == d.y && sk2) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";

	//Right Pixel
	result += "if(coord.x + 2 <= 1200 && coord.x - 1 >= 0) {";

	result += "float m1 = (c.x - l.x)/(c.z - l.z);";
	result += "float m2 = (r2.x - r.x)/(r2.z - r.z);";
	result += "float m = (r.x - c.x)/(r.z - c.z);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	result += "if(r.y != c.y && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	result += "if(r.y == c.y && sk2) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";

	//Left Pixel
	result += "if(coord.x + 1 <= 1200 && coord.x - 2 >= 0) {";

	result += "float m1 = (l.x - l2.x)/(l.z - l2.z);";
	result += "float m2 = (r.x - c.x)/(r.z - c.z);";
	result += "float m = (c.x - l.x)/(c.z - l.z);";

	result += "bool p1 = m1 > 0;";
	result += "bool p2 = m2 > 0;";
	result += "bool p = m > 0;";

	result += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	result += "if(c.y != l.y && !sk) {";
	result += "return true;\n";
	result += "}\n";

	result += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	result += "if(c.y == l.y && sk2) {";
	result += "return true;\n";
	result += "}\n";

	result += "}\n";

	result += "return false;\n";

	result += "}";

	result += "void main(void) {\n";

	result += "if(isColored(ivec2(gl_FragCoord.xy))) {imageStore(edge, ivec2(gl_FragCoord.xy), vec4(1.0, 0.0, 0.0, 0.0)); }";
	result += "else {imageStore(edge, ivec2(gl_FragCoord.xy), vec4(0.0, 0.0, 0.0, 0.0));}";
	result += "discard;";
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

static inline std::string getEdgeFragSource33() {
	std::string result = "";

	result += "#version 330 core\n";

	result += "uniform sampler2D data;\n";
	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "void main(void) {\n";

	result += "float pxw = 1.0/1200.0;\n";
	result += "vec4 ct = texture(data, coord);\n";
	result += "float c = (ct.x-0.5)*2;\n";
	result += "float r = (texture(data, vec2(coord.x + pxw, coord.y)).x-0.5)*2;\n";
	result += "float u = (texture(data, vec2(coord.x, coord.y + pxw)).x-0.5)*2;\n";
	result += "float l = (texture(data, vec2(coord.x - pxw, coord.y)).x-0.5)*2;\n";
	result += "float d = (texture(data, vec2(coord.x, coord.y - pxw)).x-0.5)*2;\n";
	result += "if(c == 0 || (coord.x + pxw <= 1.0 && c != r) || (coord.y + pxw <= 1.0 && c != u) ||";
	result += "	(coord.x - pxw >= 0.0 && c != l) || (coord.y - pxw >= 0.0 && c != d)) {out_color = vec4(1.0, 0.0, 0.0, 1.0); return;}\n";

	result += "out_color = vec4(0.0, 0.0, 0.0, 1.0);";

	result += "}\n";

	return result;
}

GraphEdgeShader::GraphEdgeShader(bool gl42) {
	std::string vertSource;
	std::string fragSource;

	if (gl42) {
		vertSource = getEdgeVertSource42();
		fragSource = getEdgeFragSource42();
	}
	else {
		vertSource = getEdgeVertSource33();
		fragSource = getEdgeFragSource33();
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
	edgeLoc = glGetUniformLocation(shaderProgram, "edge");

	bind();

	glUniform1i(dataLoc, 0);
	glUniform1i(edgeLoc, 1);

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

static inline std::string getRenderVertSource42() {
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

static inline std::string getRenderFragSource42() {
	std::string result = "";

	result += "#version 420 core\n";

	result += "layout (rg32f) uniform image2D edge;\n";
	result += "uniform vec3 g_color;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "void main(void) {\n";

	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x + 1, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x - 1, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y + 1)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y - 1)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x + 2, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x - 2, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y + 2)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y - 2)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "discard;";
	result += "}\n";

	return result;
}

static inline std::string getRenderVertSource33() {
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

static inline std::string getRenderFragSource33() {
	std::string result = "";

	result += "#version 330 core\n";

	result += "uniform sampler2D edge;\n";
	result += "uniform vec3 g_color;\n";

	result += "in vec2 coord;\n";
	result += "out vec4 out_color;\n";

	result += "void main(void) {\n";

	result += "float pw = 1.0/1200.0;";

	result += "if(texture(edge, vec2(coord.x, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "if(texture(edge, vec2(coord.x + pw, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x - pw, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y + pw)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y - pw)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "if(texture(edge, vec2(coord.x + pw*2, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x - pw*2, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y + pw*2)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	result += "if(texture(edge, vec2(coord.x, coord.y - pw*2)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	result += "discard;";
	result += "}\n";

	return result;
}

GraphRenderShader::GraphRenderShader(bool gl42) {
	std::string vertSource;
	std::string fragSource;

	if (gl42) {
		vertSource = getRenderVertSource42();
		fragSource = getRenderFragSource42();
	}
	else {
		vertSource = getRenderVertSource33();
		fragSource = getRenderFragSource33();
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

