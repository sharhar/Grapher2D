#pragma once

#include <glad/glad.hpp>
#include "GraphShader.h"
#include "GraphQuad.h"
#include "../math/Equation.h"
#include "../utils/String.h"

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
