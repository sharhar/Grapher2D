#pragma once

#include <glad/glad.h>
#include <string>
#include <GLFW/glfw3.h>

class GraphQuad {
public:
	static GLuint vao;
	static GLuint vbuffer;

	static void init();

	static void bind();
	static void unbind();
};
