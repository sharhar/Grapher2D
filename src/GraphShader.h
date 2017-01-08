#pragma once

#include <string>
#include <glcorearb.h>
#include <GLFW/glfw3.h>

struct GraphShaderFuncs {
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLGETSHADERIVPROC glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLVALIDATEPROGRAMPROC glValidateProgram;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORM1FPROC glUniform1f;
	PFNGLUNIFORM1IPROC glUniform1i;
	PFNGLDETACHSHADERPROC glDetachShader;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
};

class GraphCalcShader {
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;
	GraphShaderFuncs* funcs;

	GLint upLoc, downLoc, leftLoc, rightLoc, tLoc, atLoc;

public:
	bool compiled;

	GraphCalcShader(std::string eq);

	void bind();
	void unbind();
	void setUniforms(float up, float down, float left, float right, float time, float atime);
	void cleanUp();
};

class GraphRenderShader {
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;
	GraphShaderFuncs* funcs;

	GLint texLoc;

public:
	bool compiled;

	GraphRenderShader();

	void bind();
	void unbind();
	void setUniforms(GLuint tex);
	void cleanUp();
};