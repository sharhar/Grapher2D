#include "GLGraph.h"

#include <iostream>

#define glEnableVertexAttribArray funcs->glEnableVertexAttribArray
#define glDisableVertexAttribArray funcs->glDisableVertexAttribArray
#define glGenFramebuffers funcs->glGenFramebuffers
#define glBindFramebuffer funcs->glBindFramebuffer
#define glFramebufferTexture funcs->glFramebufferTexture
#define glActiveTexture funcs->glActiveTexture

#define NODE_TYPE_OPP 1
#define NODE_TYPE_NUM 2
#define NODE_TYPE_FUN 3
#define NODE_TYPE_FSP 4
#define NODE_TYPE_VAR 5
#define NODE_TYPE_FCL 6
#define NODE_TYPE_EXP 7
#define NODE_TYPE_FFN 8
#define NODE_TYPE_ZRO 9

typedef struct Node {
	int type;
	int childNum;
	Node** children;
	void** value;
} Node;

String getNodeString(Node* node);

static GLGraphFuncs* getFuncs() {
	GLGraphFuncs* funcs = (GLGraphFuncs*)malloc(sizeof(GLGraphFuncs));

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glDisableVertexAttribArray");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)glfwGetProcAddress("glFramebufferTexture");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture");

	return funcs;
}

GLGraph::GLGraph(Equation* e) {
	String eqt = getNodeString((Node*)e->getRootNode());

	calcShader = new GraphCalcShader(eqt.getstdstring());
	renderShader = new GraphRenderShader();
	renderShader->bind();
	renderShader->setUniforms(0);
	renderShader->unbind();

	funcs = getFuncs();

	GraphQuad::init();

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &dtex);
	glBindTexture(GL_TEXTURE_2D, dtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600,
		0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLGraph::render(GLuint pfbo, float up, float down, float left, float right, float time, float atime) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 600, 600);

	calcShader->bind();
	calcShader->setUniforms(up, down,left, right, time, atime);
	GraphQuad::bind();
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	GraphQuad::unbind();
	calcShader->unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, pfbo);

	glBindTexture(GL_TEXTURE_2D, dtex);

	glActiveTexture(GL_TEXTURE0);
	
	renderShader->bind();
	GraphQuad::bind();
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	GraphQuad::unbind();
	renderShader->unbind();
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLGraph::cleanUp() {
	calcShader->cleanUp();
	renderShader->cleanUp();

	delete calcShader;
	delete renderShader;


}

String getNodeString(Node* node) {
	if (node->type == NODE_TYPE_ZRO) {
		return "0";
	}
	else if (node->type == NODE_TYPE_NUM) {
		return String(std::to_string(((double*)(node->value[0]))[0]));
	}
	else if (node->type == NODE_TYPE_VAR) {
		Variable* var = (Variable*)node->value[0];
		return *var->name;
	}
	else if (node->type == NODE_TYPE_OPP) {
		Operator* op = (Operator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		String prevVal = getNodeString(prev);
		String nextVal = getNodeString(next);

		String result;

		if (op->name == '+') {
			result = "(" + prevVal + ")+(" + nextVal + ")";
		}
		else if (op->name == '-') {
			result = "(" + prevVal + ")-(" + nextVal + ")";
		}
		else if (op->name == '*') {
			result = "(" + prevVal + ")*(" + nextVal + ")";
		}
		else if (op->name == '/') {
			result = "(" + prevVal + ")/(" + nextVal + ")";
		}
		else if (op->name == '=') {
			result = "(" + prevVal + ")-(" + nextVal + ")";
		}
		else if (op->name == '^') {
			result = "pow(" + prevVal + "," + nextVal + ")";
		}
		return result;
	}
	else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		if (func == NULL) {
			return getNodeString(node->children[0]);
		}

		String* values = new String[node->childNum];

		for (int i = 0; i < node->childNum; i++) {
			values[i] = getNodeString(node->children[i]);
		}

		String arg = "";
		for (int i = 0; i < node->childNum - 1; i++) {
			arg = arg + values[i] + ",";
		}

		arg = arg + values[node->childNum - 1];

		String name = *func->name;
		String result = "";

		if (name == "ln") {
			result = "log(" + arg + ")";
		} else {
			result = name + "(" + arg + ")";
		}

		return result;
	}

	return "";
}