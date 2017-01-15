#pragma once

#include <GL/glew.h>
#include "GraphShader.h"
#include "GraphQuad.h"
#include "math/Equation.h"
#include "utils/String.h"

class GLGraph {
private:
	GraphCalcShader* calcShader;
	GraphRenderShader* renderShader;
	GLuint dtex;
public:
	GLGraph(Equation* e);

	void calc(float up, float down, float left, float right, float time, float atime);
	void render(glui::Color graphColor);
	void cleanUp();
};
