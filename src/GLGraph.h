#pragma once

#include <GL/glew.h>
#include "GraphShader.h"
#include "GraphQuad.h"
#include "math/Equation.h"
#include "utils/String.h"

class GLGraph {
private:
	GraphCalcShader* calcShader;
	GraphEdgeShader* edgeShader;
	GraphRenderShader* renderShader;
	GLuint dtex, etex;
public:
	GLGraph(Equation* e);

	void calc(float up, float down, float left, float right, float time, float atime);
	void renderEdge();
	void render(glui::Color graphColor);
	void cleanUp();
};
