#pragma once

#include <swin/SWin.h>
#include <glad/glad.h>

extern double g_left;
extern double g_right;
extern double g_down;
extern double g_up;

extern int g_windowWidth;
extern int g_windowHeight;

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

typedef struct GLGraph {
	GLShader* calcShader;
	GLuint dfbo, dtex, efbo, etex;
	GLsizei xSize, ySize;
} GLGraph;

GLShader* createShader(char* vertSource, char* fragSource, char** attribs, uint32_t attribsCount, char** uniforms, uint32_t uniformCount);
void deleteShader(GLShader* shader);
GLShader* createQuadShader();
GLShader* createLineShader();
GLShader* createCalcShader(char* funcs, char* eq);
GLShader* createEdgeShader(int port);
GLShader* createRenderShader(int port);
GLMesh* createQuadMesh();
GLGraph* createGraph(char* eq, char* funcs, GLsizei xSize, GLsizei ySize);
void deleteGraph(GLGraph* graph);
void drawGraphData(GLGraph* graph, float relativeTime, float absoluteTime);
void drawGraphEdges(GLGraph* graph);

