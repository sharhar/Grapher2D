#include "GraphQuad.h"
#include <iostream>

//GraphQuadFuncs* GraphQuad::funcs = 0;
GLuint GraphQuad::vao = 0;
GLuint GraphQuad::vbuffer = 0;

#define glGenVertexArrays //funcs->glGenVertexArrays
#define glBindVertexArray //funcs->glBindVertexArray
#define glGenBuffers //funcs->glGenBuffers
#define glBindBuffer //funcs->glBindBuffer
#define glBufferData //funcs->glBufferData
#define glVertexAttribPointer //funcs->glVertexAttribPointer
#define glDeleteBuffers //funcs->glDeleteBuffers
#define glDeleteVertexArrays //funcs->glDeleteVertexArrays

/*
static GraphQuadFuncs* getFuncs() {
	GraphQuadFuncs* funcs = (GraphQuadFuncs*)malloc(sizeof(GraphQuadFuncs));

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");

	return funcs;
}
*/

void GraphQuad::init() {
	//funcs = getFuncs();

	GLfloat* verts = (GLfloat*)malloc(sizeof(GLfloat)*6*2);

	verts[0] = -1;
	verts[1] = -1;

	verts[2] =  1;
	verts[3] = -1;

	verts[4] = -1;
	verts[5] =  1;

	verts[6] = 1;
	verts[7] = -1;

	verts[8] = -1;
	verts[9] = 1;

	verts[10] = 1;
	verts[11] = 1;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void GraphQuad::bind() {
	glBindVertexArray(vao);
}

void GraphQuad::unbind() {
	glBindVertexArray(0);
}
