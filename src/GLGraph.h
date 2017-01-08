#pragma once

#include "GraphShader.h"
#include "GraphQuad.h"

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
	GLGraph(std::string eq);

	void render(GLuint pfbo, float up, float down, float left, float right);
};