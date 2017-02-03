#pragma once

#include <GLUI/GLUI.h>

class GraphQuad {
public:
	static GLuint vao;
	static GLuint vbuffer;

	static void init();

	static void bind();
	static void unbind();
};
