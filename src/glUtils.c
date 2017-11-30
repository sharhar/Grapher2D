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
gl_Position = vec4(position.x, position.y, 0, 1);\n\
texcoord_out = texcoord;\n\
}\n";

	char* fragSource = "\
#version 330 core\n\
out vec4 out_color;\n\
uniform sampler2D tex;\n\
in vec2 texcoord_out;\n\
void main(void) {\n\
out_color = texture(tex, texcoord_out);\n\
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

GLShader* createCalcShader(char* eq, char* funcs) {
	char* vertSource = "#version 330 core\n\
in vec2 position;\n\
out vec2 coord;\n\
uniform float up;\n\
uniform float down;\n\
uniform float left;\n\
uniform float right;\n\
void main(void) {\n\
gl_Position = vec4(position.xy, 0, 1);\n\
coord = vec2(0.0, 0.0);\n\
if(position.x == 1) {coord.x = right;}\
else {coord.x = left;}\
if(position.y == 1) {coord.y = up;}\
else {coord.y = down;}\
}\n";

	char* preFragSource0 = "#version 330 core\n\
uniform float t;\n\
uniform float at;\n\
const float e = 2.718281828459045;\n\
const float pi = 3.141592653589793;\n\
const float tau = 3.141592653589793 * 2;\n\
in vec2 coord;\n\
out vec4 out_color;\n\
float powi_c(float b, int p) {\
if(p%2 == 0) {\
return pow(abs(b), p);\
}\
return sign(b)*pow(abs(b), p);\
}\n\
float fact(float num) {\
float result = 1;\
for(float i = 2; i <= num; i++) {\
result *= i;\
}\
return result;\
}\n\
float pow_c(float b, float p) {\n\
int pi = int(p);\
if(p < 0) {\
if(pi == p) {\
return 1/(powi_c(b, abs(pi)));\
} else {\
return 1/(pow(b, abs(p)));\
}\n\
}\n\
if(pi == p) {\
return powi_c(b, pi);\
} else {\
return pow(b, p);\
}\n\
}\n\
float gamma(float x) {\n\
float num = x+1;\n\
float result0_ = 0;\n\
float result1_ = 0;\n\
float a_ = 0;\n\
float b_ = num*25;\n\
float n_ = 250;\n\
float dx_ = (b_ - a_)/n_;\n\
float h = a_;\n\
result0_ += pow_c(h, (num-1))*exp(-h);\n\
h = b_;\n\
result0_ += pow_c(h, (num-1))*exp(-h);\n\
for(float k_ = 1; k_ < n_; k_++) {\n\
h = a_+(k_*dx_);\n\
result1_ += 2*pow_c(h, (num-1))*exp(-h);\n\
}\n\
return (dx_/2)*(result0_ + result1_)/x;\n\
\n}\n";

	char* preFragSource1 = "void main(void) {\n\
float x = coord.x;\n\
float y = coord.y;\n\
float result = ";

	char* preFragSource2 = ";\n\
float total = result;\n\
y = 0;\n\
result = ";

	char* preFragSource3 = ";\n\
float total0 = result;\n\
out_color = vec4(total, total0, 0.0, 1.0);\n\
}\n";

	int pfs0l = strlen(preFragSource0);
	int pfs1l = strlen(preFragSource1);
	int pfs2l = strlen(preFragSource2);
	int pfs3l = strlen(preFragSource3);
	int fl = strlen(funcs);
	int el = strlen(eq);

	int fsl = pfs0l + fl + pfs1l + el + pfs2l + el + pfs3l;

	char* fragSource = malloc(sizeof(char) * fsl + 1);
	memset(fragSource, 0, sizeof(char) * fsl + 1);

	strcat(fragSource, preFragSource0);
	strcat(fragSource, funcs);
	strcat(fragSource, preFragSource1);
	strcat(fragSource, eq);
	strcat(fragSource, preFragSource2);
	strcat(fragSource, eq);
	strcat(fragSource, preFragSource3);

	//printf("%s\n", fragSource);

	char* attribs[1] = { "position" };
	char* uniforms[6] = { "up", "down", "left", "right", "t", "at" };

	return createShader(vertSource, fragSource, attribs, 1, uniforms, 6);
}

GLShader* createEdgeShader(int port) {
	char* vertSource = "#version 330 core\n\
out vec2 coord;\n\
in vec2 position;\n\
void main(void) {\n\
gl_Position = vec4(position.xy, 0, 1);\n\
coord = position/2.0 + 0.5;\n\
}\n";

	char* preFragSource0 = "#version 330 core\n\
uniform sampler2D data;\n\
in vec2 coord;\n\
out vec4 out_color;\n\
    const float pxw = 1.0/";
    
    char* preFragSource1 = ".0;\n\
bool isColored(vec2 coord) {\
vec4 c = texture(data, coord);\n\
if(c.x == 0) {\
return true;\n\
}\n\
vec4 u = texture(data, vec2(coord.x, coord.y - pxw));\n\
vec4 d = texture(data, vec2(coord.x, coord.y + pxw));\n\
vec4 u2 = texture(data, vec2(coord.x, coord.y - pxw*2));\n\
if(coord.y + pxw*2 <= 1.0 && coord.y - pxw >= 0.0) {\
float m1 = (c.x - d.x);\
float m2 = (u2.x - u.x);\
float m = (u.x - c.x);\
bool p1 = m1 > 0;\
bool p2 = m2 > 0;\
bool p = m > 0;\
bool sk = p1 == p2 && p1 != p;\
if(sign(u.x) != sign(c.x) && !sk) {\
return true;\n\
}\n\
float m10 = (c.y - d.y);\
float m20 = (u2.y - u.y);\
float m0 = (u.y - c.y);\
bool p10 = m10 > 0;\
bool p20 = m20 > 0;\
bool p0 = m0 > 0;\
bool sk0 = p10 == p20 && p10 != p0;\
if(sign(u.x) == sign(c.x) && sign(u.y) != sign(c.y) && sk0) {\
return true;\n\
}\n\
}\n\
vec4 d2 = texture(data, vec2(coord.x, coord.y + pxw*2));\n\
if(coord.y + pxw*2 <= 1.0 && coord.y - pxw >= 0.0) {\
float m1 = (d.x - d2.x);\
float m2 = (u.x - c.x);\
float m = (c.x - d.x);\
bool p1 = m1 > 0;\
bool p2 = m2 > 0;\
bool p = m > 0;\
bool sk = p1 == p2 && p1 != p;\
if(sign(c.x) != sign(d.x) && !sk) {\
return true;\n\
}\n\
float m10 = (d.y - d2.y);\
float m20 = (u.y - c.y);\
float m0 = (c.y - d.y);\
bool p10 = m10 > 0;\
bool p20 = m20 > 0;\
bool p0 = m0 > 0;\
bool sk0 = p10 == p20 && p10 != p0;\
if(sign(c.x) == sign(d.x) && sign(c.y) != sign(d.y) && sk0) {\
return true;\n\
}\n\
}\n\
vec4 r = texture(data, vec2(coord.x + pxw, coord.y));\n\
vec4 l = texture(data, vec2(coord.x - pxw, coord.y));\n\
vec4 r2 = texture(data, vec2(coord.x + pxw*2, coord.y));\n\
if(coord.x + pxw*2 <= 1.0 && coord.x - pxw >= 0.0) {\
float m1 = (c.x - l.x);\
float m2 = (r2.x - r.x);\
float m = (r.x - c.x);\
bool p1 = m1 > 0;\
bool p2 = m2 > 0;\
bool p = m > 0;\
bool sk = p1 == p2 && p1 != p;\
if(sign(r.x) != sign(c.x) && !sk) {\
return true;\n\
}\n\
float m10 = (c.y - l.y);\
float m20 = (r2.y - r.y);\
float m0 = (r.y - c.y);\
bool p10 = m10 > 0;\
bool p20 = m20 > 0;\
bool p0 = m0 > 0;\
bool sk0 = p10 == p20 && p10 != p0;\
if(sign(r.x) == sign(c.x) && sign(r.y) != sign(c.y) && sk0) {\
return true;\n\
}\n\
}\n\
vec4 l2 = texture(data, vec2(coord.x - pxw*2, coord.y));\n\
if(coord.x + pxw*2 <= 1.0 && coord.x - pxw >= 0.0) {\
float m1 = (l.x - l2.x);\
float m2 = (r.x - c.x);\
float m = (c.x - l.x);\
bool p1 = m1 > 0;\
bool p2 = m2 > 0;\
bool p = m > 0;\
bool sk = p1 == p2 && p1 != p;\
if(sign(c.x) != sign(l.x) && !sk) {\
return true;\n\
}\n\
float m10 = (l.y - l2.y);\
float m20 = (r.y - c.y);\
float m0 = (c.y - l.y);\
bool p10 = m10 > 0;\
bool p20 = m20 > 0;\
bool p0 = m0 > 0;\
bool sk0 = p10 == p20 && p10 != p0;\
if(sign(c.x) == sign(l.x) && sign(c.y) != sign(l.y) && sk0) {\
return true;\n\
}\n\
}\n\
return false;\n\
}\
void main(void) {\n\
if(isColored(coord)) {out_color = vec4(1.0, 0.0, 0.0, 1.0); return;}\n\
out_color = vec4(0.0, 0.0, 0.0, 1.0);\
}\n";
	
    char* portstr = malloc(sizeof(char) * 8);
    memset(portstr, 0, sizeof(char) * 8);
    
    sprintf(portstr, "%d", port);
    
    int pfs0l = strlen(preFragSource0);
    int pfs1l = strlen(preFragSource1);
    int fl = strlen(portstr);
    
    int fsl = pfs0l + fl + pfs1l;
    
    char* fragSource = malloc(sizeof(char) * fsl + 1);
    memset(fragSource, 0, sizeof(char) * fsl + 1);
    
    strcat(fragSource, preFragSource0);
    strcat(fragSource, portstr);
    strcat(fragSource, preFragSource1);
    
	char* attribs[1] = { "position" };
	char* uniforms[1] = { "data" };

	return createShader(vertSource, fragSource, attribs, 1, uniforms, 1);
}

GLShader* createRenderShader(int port) {
	char* vertSource = "#version 330 core\n\
out vec2 coord;\n\
in vec2 position;\n\
void main(void) {\n\
gl_Position = vec4(position.xy, 0, 1);\n\
coord = position/2.0 + 0.5;\n\
}\n";

	char* preFragSource0 = "#version 330 core\n\
uniform sampler2D edge;\n\
uniform vec3 g_color;\n\
in vec2 coord;\n\
out vec4 out_color;\n\
    const float pw = 1.0/";
    
    char* preFragSource1 = ".0;\n\
void main(void) {\n\
if(texture(edge, vec2(coord.x, coord.y)).x == 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x + pw, coord.y)).x == 1.0 && coord.x + pw <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x - pw, coord.y)).x == 1.0 && coord.x - pw >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x, coord.y + pw)).x == 1.0 && coord.y + pw <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x, coord.y - pw)).x == 1.0 && coord.y - pw >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x + pw*2, coord.y)).x == 1.0 && coord.x + pw*2 <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x - pw*2, coord.y)).x == 1.0 && coord.x - pw*2 >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x, coord.y + pw*2)).x == 1.0 && coord.y + pw*2 <= 1.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
if(texture(edge, vec2(coord.x, coord.y - pw*2)).x == 1.0 && coord.y - pw*2 >= 0.0) {out_color = vec4(g_color.xyz, 1.0); return;}\n\
discard;\n\
}\n";
    
    char* portstr = malloc(sizeof(char) * 8);
    memset(portstr, 0, sizeof(char) * 8);
    
    sprintf(portstr, "%d", port);
    
    int pfs0l = strlen(preFragSource0);
    int pfs1l = strlen(preFragSource1);
    int fl = strlen(portstr);
    
    int fsl = pfs0l + fl + pfs1l;
    
    char* fragSource = malloc(sizeof(char) * fsl + 1);
    memset(fragSource, 0, sizeof(char) * fsl + 1);
    
    strcat(fragSource, preFragSource0);
    strcat(fragSource, portstr);
    strcat(fragSource, preFragSource1);

	char* attribs[1] = { "position" };
	char* uniforms[2] = { "edge" , "g_color" };
    
    printf("%s\n", fragSource);

	return createShader(vertSource, fragSource, attribs, 1, uniforms, 2);
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
	texCoords[1] = 0;

	texCoords[2] = 1;
	texCoords[3] = 0;

	texCoords[4] = 1;
	texCoords[5] = 1;

	texCoords[6] = 0;
	texCoords[7] = 0;

	texCoords[8] = 0;
	texCoords[9] = 1;

	texCoords[10] = 1;
	texCoords[11] = 1;

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
