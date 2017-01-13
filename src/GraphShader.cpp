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

	fragSource += "layout (rg32f) uniform image2D data;\n";

	fragSource += "const float e = 2.718281828459045;\n";
	fragSource += "const float pi = 3.141592653589793;\n";
	fragSource += "const float tau = 3.141592653589793 * 2;\n";

	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";
	fragSource += "void main(void) {\n";
	fragSource += "float x = coord.x;\n";
	fragSource += "float y = coord.y;\n";
	fragSource += "float total = " + eq + ";\n";
	fragSource += "imageStore(data, ivec2(gl_FragCoord.xy), vec4(total, sign(total), 0.0, 0.0));\n";
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
	fragSource += "uniform sampler2D tex;\n";
	fragSource += "uniform vec3 g_color;\n";

	fragSource += "layout (rg32f) uniform image2D data;\n";

	fragSource += "in vec2 coord;\n";
	fragSource += "out vec4 out_color;\n";
	fragSource += "void main(void) {\n";

	fragSource += "vec4 c = imageLoad(data, ivec2(gl_FragCoord.xy));\n";

	fragSource += "int width = 4;\n";

	fragSource += "int ci = 0;";

	fragSource += "for(int i = 1; i <= width; i++) {\n";

	fragSource += "float r = imageLoad(data, ivec2(gl_FragCoord.x + i, gl_FragCoord.y)).y;\n";
	fragSource += "float u = imageLoad(data, ivec2(gl_FragCoord.x, gl_FragCoord.y + i)).y;\n";
	fragSource += "float l = imageLoad(data, ivec2(gl_FragCoord.x - i, gl_FragCoord.y)).y;\n";
	fragSource += "float d = imageLoad(data, ivec2(gl_FragCoord.x, gl_FragCoord.y - i)).y;\n";

	fragSource += "if(c.y == 0 || (gl_FragCoord.x + i <= 1200 && c.y != r) || (gl_FragCoord.y + i <= 1200 && c.y != u) ||";
	fragSource += "	(gl_FragCoord.x - i >= 0 && c.y != l) || (gl_FragCoord.y + i >= 0 && c.y != d)) {out_color = vec4(g_color.xyz, 1.0); ci = 1; break;}\n";

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

	//free(funcs);
}

void GraphCalcShader::cleanUp() {
	glUseProgram(0);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);

	//free(funcs);
}