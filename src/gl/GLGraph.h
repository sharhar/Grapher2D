#pragma once

#include <GLUI/GLUI.h>
#include "GraphShader.h"
#include "GraphQuad.h"
#include "../utils/Equation.h"

class GLGraph {
private:
	GraphCalcShader* calcShader;
	bool inited;
public:
	GLGraph(Equation* e, int portSize, String** error);

	void calc(float up, float down, float left, float right, float time, float atime);
	void cleanUp();

	GLuint dtex, etex;
	GLuint dfbo, efbo;
};
