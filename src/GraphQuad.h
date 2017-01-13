#pragma once

#include <GL/glew.h>
#include <string>
#include <glcorearb.h>
#include <GLFW/glfw3.h>

class GraphQuad {
public:
	static GLuint vao;
	static GLuint vbuffer;

	static void init();

	static void bind();
	static void unbind();
};
