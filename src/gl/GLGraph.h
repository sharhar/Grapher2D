#pragma once

#include <GLUI/GLUI.h>
#include "GraphShader.h"
#include "GraphQuad.h"
#include "../utils/Equation.h"

class GLGraph {
private:
	GraphCalcShader* calcShader;
public:
	GLGraph(Equation* e, bool gl42);

	void calc(float up, float down, float left, float right, float time, float atime);

	void calc33(float up, float down, float left, float right, float time, float atime);
	void cleanUp();

	GLuint dtex, etex;
	GLuint dfbo, efbo;
};
