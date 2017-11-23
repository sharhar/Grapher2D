#include "glUtils.h"
#include <stdio.h>

void* malloc(size_t);
void free(void*);

GLShader* createShader(char* vertSource, char* fragSource, char** attribs, uint32_t attribsCount, char** uniforms, uint32_t uniformCount) {
	GLShader* result = malloc(sizeof(GLShader));

	result->vertShader = glCreateShader(GL_VERTEX_SHADER);
	result->fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(result->vertShader, 1, &vertSource, 0);
	glShaderSource(result->fragShader, 1, &fragSource, 0);

	glCompileShader(result->vertShader);

	GLint compiled = 0;
	glGetShaderiv(result->vertShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(result->vertShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(result->vertShader, maxLength, &maxLength, message);

		printf("Vertex Shader failed to compile:\n");
		printf("%s\n", message);

		glDeleteShader(result->vertShader);
		free(result);
		return NULL;
	}

	glCompileShader(result->fragShader);

	glGetShaderiv(result->fragShader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(result->fragShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* message = (GLchar*)malloc(sizeof(GLchar)*maxLength);
		glGetShaderInfoLog(result->fragShader, maxLength, &maxLength, message);

		printf("Fragment Shader failed to compile:\n");
		printf("%s\n", message);

		glDeleteShader(result->fragShader);
		free(result);
		return NULL;
	}

	result->program = glCreateProgram();

	glAttachShader(result->program, result->vertShader);
	glAttachShader(result->program, result->fragShader);

	for (uint32_t i = 0; i < attribsCount; i++) {
		glBindAttribLocation(result->program, i, attribs[i]);
	}

	glLinkProgram(result->program);
	glValidateProgram(result->program);

	result->uniformCount = uniformCount;
	result->uniformLocs = malloc(sizeof(GLuint) * uniformCount);

	glUseProgram(result->program);

	for (uint32_t i = 0; i < uniformCount; i++) {
		result->uniformLocs[i] = glGetUniformLocation(result->program, uniforms[i]);
	}

	glUseProgram(0);

	return result;
}

GLShader* createQuadShader() {
	char* vertSource = "\
#version 330 core\n\
in vec2 position;\n\
in vec2 texcoord;\n\
out vec2 texcoord_out;\n\
void main(void) {\n\
gl_Position = vec4(position.x*0.5, position.y*0.5, 0, 1);\n\
texcoord_out = texcoord;\n\
}\n";

	char* fragSource = "\
#version 330 core\n\
out vec4 out_color;\n\
uniform sampler2D tex;\n\
in vec2 texcoord_out;\n\
void main(void) {\n\
out_color = vec4(0.0, 0.0, 0.0, 1.0);//texture(tex, texcoord_out).r);\n\
}\n";

	char* attribs[2] = {"position", "texcoord"};
	char* uniforms[1] = {"tex"};

	return createShader(vertSource, fragSource, attribs, 2, uniforms, 1);
}

GLShader* createLineShader() {
	char* vertSource = "\
#version 330 core\n\
in vec2 position;\n\
uniform mat4 projection;\n\
uniform mat4 modelview;\n\
void main(void) {\n\
gl_Position = projection * modelview * vec4(position.x, position.y, 0, 1);\n\
}\n";

	char* fragSource = "\
#version 330 core\n\
out vec4 out_color;\n\
uniform vec3 color;\n\
void main(void) {\n\
out_color = vec4(color.xyz, 1.0);\
}\n";

	char* attribs[1] = { "position" };
	char* uniforms[3] = { "projection" , "modelview", "color"};

	return createShader(vertSource, fragSource, attribs, 1, uniforms, 3);
}

GLMesh* createQuadMesh() {
	GLMesh* result = malloc(sizeof(GLMesh));

	GLfloat* verts = malloc(sizeof(GLfloat) * 6 * 2);

	verts[0] = -1;
	verts[1] = -1;

	verts[2] = 1;
	verts[3] = -1;

	verts[4] = 1;
	verts[5] = 1;

	verts[6] = -1;
	verts[7] = -1;

	verts[8] = -1;
	verts[9] = 1;

	verts[10] = 1;
	verts[11] = 1;

	GLfloat* texCoords = (GLfloat*)malloc(sizeof(GLfloat) * 6 * 2);

	texCoords[0] = 0;
	texCoords[1] = 1;

	texCoords[2] = 1;
	texCoords[3] = 1;

	texCoords[4] = 1;
	texCoords[5] = 0;

	texCoords[6] = 0;
	texCoords[7] = 1;

	texCoords[8] = 0;
	texCoords[9] = 0;

	texCoords[10] = 1;
	texCoords[11] = 0;

	glGenVertexArrays(1, &result->vao);
	glBindVertexArray(result->vao);

	glGenBuffers(1, &result->vbo0);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &result->vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return result;
}