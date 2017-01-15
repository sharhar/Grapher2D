#pragma once

#include <GL/glew.h>
#include <string>
#include <GLFW/glfw3.h>
#include <GLUI/GLUI.h>

class GraphCalcShader {
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	GLint upLoc, downLoc, leftLoc, rightLoc, tLoc, atLoc;

public:
	bool compiled;

	GraphCalcShader(std::string eq);

	void bind();
	void unbind();
	void setUniforms(float up, float down, float left, float right, float time, float atime);
	void cleanUp();
};

class GraphEdgeShader {
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	GLint dataLoc, edgeLoc;
public:
	bool compiled;

	GraphEdgeShader();

	void bind();
	void unbind();
	void cleanUp();
};

class GraphRenderShader {
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	GLint colorLoc, edgeLoc;
public:
	bool compiled;

	GraphRenderShader();

	void bind();
	void setUniforms(glui::Color graphColor);
	void unbind();
	void cleanUp();
};
