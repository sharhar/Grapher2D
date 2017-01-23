#include "GraphShader.h"
#include <vector>
#include <iostream>

GraphCalcShader::GraphCalcShader(std::string eq) {
	
	std::string vertSource = "";

	vertSource += "#version 420 core\n";
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

	fragSource += "#version 420 core\n";
	fragSource += "uniform float t;\n";
	fragSource += "uniform float at;\n";

	fragSource += "layout (rgba32f) uniform image2D data;\n";

	fragSource += "const float e = 2.718281828459045;\n";
	fragSource += "const float pi = 3.141592653589793;\n";
	fragSource += "const float tau = 3.141592653589793 * 2;\n";

	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";

	fragSource += "float powi_c(float b, int p) {";
	fragSource += "float result = 1;";
	fragSource += "for(int i = 0; i < p; i++) {";
	fragSource += "result = result * b;";
	fragSource += "}\n";
	fragSource += "return result;";
	fragSource += "}\n";

	fragSource += "float pow_c(float b, float p) {\n";

	fragSource += "int pi = int(p);";

	fragSource += "if(p < 0) {";
	fragSource += "if(pi == p) {";
	fragSource += "return 1/(powi_c(b, abs(pi)));";
	fragSource += "} else {";
	fragSource += "return 1/(pow(b, abs(p)));";
	fragSource += "}\n";
	fragSource += "}\n";

	fragSource += "if(pi == p) {";
	fragSource += "return powi_c(b, pi);";
	fragSource += "} else {";
	fragSource += "return pow(b, p);";
	fragSource += "}\n";
	
	fragSource += "}\n";

	fragSource += "void main(void) {\n";
	fragSource += "float x = coord.x;\n";
	fragSource += "float y = coord.y;\n";
	fragSource += "float total = " + eq + ";\n";
	fragSource += "imageStore(data, ivec2(gl_FragCoord.xy), vec4(total, sign(total), x, y));\n";
	fragSource += "discard;\n";
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

void GraphCalcShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

GraphEdgeShader::GraphEdgeShader() {
	std::string vertSource = "";

	vertSource += "#version 420 core\n";
	vertSource += "out vec2 coord;\n";
	vertSource += "in vec2 position;\n";
	vertSource += "void main(void) {\n";
	vertSource += "gl_Position = vec4(position.xy, 0, 1);\n";
	vertSource += "coord = position/2.0 + 0.5;\n";
	vertSource += "}\n";

	std::string fragSource = "";

	fragSource += "#version 420 core\n";

	fragSource += "layout (rgba32f) uniform image2D data;\n";
	fragSource += "layout (rg32f) uniform image2D edge;\n";

	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";

	fragSource += "bool isColored(ivec2 coord) {";

	fragSource += "vec4 c = imageLoad(data, coord);\n";
	
	fragSource += "vec4 r = imageLoad(data, ivec2(coord.x + 1, coord.y));\n";
	fragSource += "vec4 u = imageLoad(data, ivec2(coord.x, coord.y + 1));\n";
	fragSource += "vec4 l = imageLoad(data, ivec2(coord.x - 1, coord.y));\n";
	fragSource += "vec4 d = imageLoad(data, ivec2(coord.x, coord.y - 1));\n";

	fragSource += "vec4 r2 = imageLoad(data, ivec2(coord.x + 2, coord.y));\n";
	fragSource += "vec4 u2 = imageLoad(data, ivec2(coord.x, coord.y + 2));\n";
	fragSource += "vec4 l2 = imageLoad(data, ivec2(coord.x - 2, coord.y));\n";
	fragSource += "vec4 d2 = imageLoad(data, ivec2(coord.x, coord.y - 2));\n";

	fragSource += "float mm = 10;";
	fragSource += "float mm2 = 50000;";

	//Up Pixel
	fragSource += "if(coord.y + 2 <= 1200 && coord.y - 1 >= 0) {";

	fragSource += "float m1 = (c.x - d.x)/(c.z - d.z);";
	fragSource += "float m2 = (u2.x - u.x)/(u2.z - u.z);";
	fragSource += "float m = (u.x - c.x)/(u.z - c.z);";

	fragSource += "bool p1 = m1 > 0;";
	fragSource += "bool p2 = m2 > 0;";
	fragSource += "bool p = m > 0;";

	fragSource += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	fragSource += "if(u.y != c.y && !sk) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	fragSource += "if(u.y == c.y && sk2) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "}\n";

	//Down Pixel
	fragSource += "if(coord.y + 1 <= 1200 && coord.y - 2 >= 0) {";

	fragSource += "float m1 = (d.x - d2.x)/(d.z - d2.z);";
	fragSource += "float m2 = (u.x - c.x)/(u.z - c.z);";
	fragSource += "float m = (c.x - d.x)/(c.z - d.z);";

	fragSource += "bool p1 = m1 > 0;";
	fragSource += "bool p2 = m2 > 0;";
	fragSource += "bool p = m > 0;";

	fragSource += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	fragSource += "if(c.y != d.y && !sk) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	fragSource += "if(c.y == d.y && sk2) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "}\n";

	//Right Pixel
	fragSource += "if(coord.x + 2 <= 1200 && coord.x - 1 >= 0) {";

	fragSource += "float m1 = (c.x - l.x)/(c.z - l.z);";
	fragSource += "float m2 = (r2.x - r.x)/(r2.z - r.z);";
	fragSource += "float m = (r.x - c.x)/(r.z - c.z);";

	fragSource += "bool p1 = m1 > 0;";
	fragSource += "bool p2 = m2 > 0;";
	fragSource += "bool p = m > 0;";

	fragSource += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	fragSource += "if(r.y != c.y && !sk) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	fragSource += "if(r.y == c.y && sk2) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "}\n";

	//Left Pixel
	fragSource += "if(coord.x + 1 <= 1200 && coord.x - 2 >= 0) {";

	fragSource += "float m1 = (l.x - l2.x)/(l.z - l2.z);";
	fragSource += "float m2 = (r.x - c.x)/(r.z - c.z);";
	fragSource += "float m = (c.x - l.x)/(c.z - l.z);";

	fragSource += "bool p1 = m1 > 0;";
	fragSource += "bool p2 = m2 > 0;";
	fragSource += "bool p = m > 0;";

	fragSource += "bool sk = p1 == p2 && p1 != p && abs(m) > mm;";

	fragSource += "if(c.y != l.y && !sk) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "bool sk2 = p1 == p2 && p1 != p && abs(m) < mm2 && abs(m) > mm*5;";

	fragSource += "if(c.y == l.y && sk2) {";
	fragSource += "return true;\n";
	fragSource += "}\n";

	fragSource += "}\n";

	fragSource += "return false;\n";

	fragSource += "}";

	fragSource += "void main(void) {\n";

	fragSource += "if(isColored(ivec2(gl_FragCoord.xy))) {imageStore(edge, ivec2(gl_FragCoord.xy), vec4(1.0, 0.0, 0.0, 0.0)); }";
	fragSource += "else {imageStore(edge, ivec2(gl_FragCoord.xy), vec4(0.0, 0.0, 0.0, 0.0));}";
	fragSource += "discard;";
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

GraphRenderShader::GraphRenderShader() {
	std::string vertSource = "";

	vertSource += "#version 420 core\n";
	vertSource += "out vec2 coord;\n";
	vertSource += "in vec2 position;\n";
	vertSource += "void main(void) {\n";
	vertSource += "gl_Position = vec4(position.xy, 0, 1);\n";
	vertSource += "coord = position/2.0 + 0.5;\n";
	vertSource += "}\n";

	std::string fragSource = "";

	fragSource += "#version 420 core\n";

	fragSource += "layout (rg32f) uniform image2D edge;\n";
	fragSource += "uniform vec3 g_color;\n";

	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";

	fragSource += "void main(void) {\n";

	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x + 1, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x - 1, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y + 1)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y - 1)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x + 2, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x - 2, gl_FragCoord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y + 2)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";
	fragSource += "if(imageLoad(edge, ivec2(gl_FragCoord.x, gl_FragCoord.y - 2)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}";

	fragSource += "discard;";
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

	edgeLoc = glGetUniformLocation(shaderProgram, "edge");
	colorLoc = glGetUniformLocation(shaderProgram, "g_color");

	bind();
	glUniform1i(edgeLoc, 1);
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

