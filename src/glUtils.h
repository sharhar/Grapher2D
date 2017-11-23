#pragma once

#include "utils.h"

typedef struct GLShader {
	GLuint vertShader;
	GLuint fragShader;
	GLuint program;

	GLuint* uniformLocs;
	uint32_t uniformCount;
} GLShader;

typedef struct GLMesh {
	GLuint vao;
	GLuint vbo0;
	GLuint vbo1;
} GLMesh;

GLShader* createShader(char* vertSource, char* fragSource, char** attribs, uint32_t attribsCount, char** uniforms, uint32_t uniformCount);
GLShader* createQuadShader();
GLShader* createLineShader();
GLMesh* createQuadMesh();

