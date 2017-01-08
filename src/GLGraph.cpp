#include "GLGraph.h"

#include <iostream>

#define glEnableVertexAttribArray funcs->glEnableVertexAttribArray
#define glDisableVertexAttribArray funcs->glDisableVertexAttribArray
#define glGenFramebuffers funcs->glGenFramebuffers
#define glBindFramebuffer funcs->glBindFramebuffer
#define glFramebufferTexture funcs->glFramebufferTexture
#define glActiveTexture funcs->glActiveTexture

static GLGraphFuncs* getFuncs() {
	GLGraphFuncs* funcs = (GLGraphFuncs*)malloc(sizeof(GLGraphFuncs));

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glDisableVertexAttribArray");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)glfwGetProcAddress("glFramebufferTexture");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture");

	return funcs;
}

GLGraph::GLGraph(std::string eq) {
	calcShader = new GraphCalcShader(eq);
	renderShader = new GraphRenderShader();

	funcs = getFuncs();

	GraphQuad::init();

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &dtex);
	glBindTexture(GL_TEXTURE_2D, dtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600,
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLGraph::render(GLuint pfbo, float up, float down, float left, float right) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 600, 600);

	calcShader->bind();
	calcShader->setUniforms(up, down,left, right);
	GraphQuad::bind();
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	GraphQuad::unbind();
	calcShader->unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, pfbo);

	glBindTexture(GL_TEXTURE_2D, dtex);

	glActiveTexture(GL_TEXTURE0);

	renderShader->bind();
	renderShader->setUniforms(0);
	GraphQuad::bind();
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	GraphQuad::unbind();
	renderShader->unbind();
	
	glBindTexture(GL_TEXTURE_2D, 0);
}