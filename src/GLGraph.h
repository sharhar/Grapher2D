#pragma once

#include "GraphShader.h"
#include "GraphQuad.h"
#include "math/Equation.h"
#include "utils/String.h"

struct GLGraphFuncs {
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
	PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
	PFNGLACTIVETEXTUREPROC glActiveTexture;
};

class GLGraph {
private:
	GraphCalcShader* calcShader;
	GraphRenderShader* renderShader;
	GLGraphFuncs* funcs;
	GLuint fbo;
	GLuint dtex;
public:
	GLGraph(Equation* e);

	void render(GLuint pfbo, glui::Color graphColor, float up, float down, float left, float right, float time, float atime);
	void cleanUp();
};