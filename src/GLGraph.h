#pragma once

#include <gl/glew.h>
#include "GraphShader.h"
#include "GraphQuad.h"
#include "math/Equation.h"
#include "utils/String.h"

class GLGraph {
private:
	GraphCalcShader* calcShader;
	GraphRenderShader* renderShader;
	GLuint fbo;
	GLuint dtex;
public:
	GLGraph(Equation* e);

	void render(GLuint pfbo, glui::Color graphColor, float up, float down, float left, float right, float time, float atime);
	void cleanUp();
};