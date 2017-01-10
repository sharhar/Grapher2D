#pragma once

#include <string>
#include <glcorearb.h>
#include <GLFW/glfw3.h>

/*
struct GraphQuadFuncs {
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
};
*/
class GraphQuad {
public:
	static GLuint vao;
	static GLuint vbuffer;
	//static GraphQuadFuncs* funcs;

	static void init();

	static void bind();
	static void unbind();
};