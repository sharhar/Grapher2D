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
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORM1FPROC glUniform1f;
};

class GraphCalcShader {
private:
	GLuint shaderProgram;
	GLuint vertexShader;
	GLuint fragmentShader;
	GraphShaderFuncs* funcs;

	GLint upLoc, downLoc, leftLoc, rightLoc;

public:
	bool compiled;

	GraphCalcShader(std::string eq);

	void bind();
	void unbind();
	void setUniforms(float up, float down, float left, float right);
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
};