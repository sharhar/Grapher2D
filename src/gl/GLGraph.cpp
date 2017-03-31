#include "GLGraph.h"

#include <iostream>

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

void getNodeString(Node* node, String* pString, int pos);

GLGraph::GLGraph(Equation* e, int portSize) {
	String* eqt = new String("");

	getNodeString((Node*)e->getRootNode(), eqt, 0);

	calcShader = new GraphCalcShader(eqt->getstdstring());

	glGenFramebuffers(1, &dfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &dtex);
	glBindTexture(GL_TEXTURE_2D, dtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, portSize, portSize,
		0, GL_RG, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dtex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &efbo);
	glBindFramebuffer(GL_FRAMEBUFFER, efbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &etex);
	glBindTexture(GL_TEXTURE_2D, etex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, portSize, portSize,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, etex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLGraph::calc(float up, float down, float left, float right, float time, float atime) {
	glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
    
    glClear(GL_COLOR_BUFFER_BIT);

	calcShader->bind();
	calcShader->setUniforms(up, down, left, right, time, atime);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	calcShader->unbind();
}

void GLGraph::cleanUp() {
	glDeleteTextures(1, &dtex);
	glDeleteTextures(1, &etex);

	calcShader->cleanUp();
	delete calcShader;
}

void getNodeString(Node* node, String* pString, int pos) {
	if (node->type == NODE_TYPE_ZRO) {
		pString->insert((char*)"0", pos);
	} else if (node->type == NODE_TYPE_NUM) {
		pString->insert(std::to_string(((double*)(node->value[0]))[0]), pos);
	} else if (node->type == NODE_TYPE_VAR) {
		Variable* var = (Variable*)node->value[0];
		pString->insert(*var->name, pos);
	} else if (node->type == NODE_TYPE_OPP) {
		Operator* op = (Operator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		if (op->name == '+') {
			pString->insert("()+()", pos);
			getNodeString(next, pString, pos + 4);
			getNodeString(prev, pString, pos + 1);
		}
		else if (op->name == '-') {
			pString->insert("()-()", pos);
			getNodeString(next, pString, pos + 4);
			getNodeString(prev, pString, pos + 1);
		}
		else if (op->name == '*') {
			pString->insert("()*()", pos);
			getNodeString(next, pString, pos + 4);
			getNodeString(prev, pString, pos + 1);
		}
		else if (op->name == '/') {
			pString->insert("()/()", pos);
			getNodeString(next, pString, pos + 4);
			getNodeString(prev, pString, pos + 1);
		}
		else if (op->name == '=') {
			pString->insert("()-()", pos);
			getNodeString(next, pString, pos + 4);
			getNodeString(prev, pString, pos + 1);
		}
		else if (op->name == '^') {
			pString->insert("pow_c(,)", pos);
			getNodeString(next, pString, pos + 7);
			getNodeString(prev, pString, pos + 6);
		}
		
	} else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		if (func == NULL) {
			getNodeString(node->children[0], pString, pos);
			return;
		}
		
		String name = *func->name;
		
		if (name == "ln") {
			pString->insert("log()", pos);
			getNodeString(node->children[0], pString, pos + 4);
		} else if (name == "log") {
			pString->insert("(log()/log(10.0))", pos);
			getNodeString(node->children[0], pString, pos + 5);
		} else if (name == "cot") {
			pString->insert("(1/tan())", pos);
			getNodeString(node->children[0], pString, pos + 7);
		} else if (name == "csc") {
			pString->insert("(1/sin())", pos);
			getNodeString(node->children[0], pString, pos + 7);
		} else if (name == "sec") {
			pString->insert("(1/cos())", pos);
			getNodeString(node->children[0], pString, pos + 7);
		} else if (name == "acot") {
			pString->insert("atan(1/())", pos);
			getNodeString(node->children[0], pString, pos + 8);
		} else if (name == "acsc") {
			pString->insert("asin(1/())", pos);
			getNodeString(node->children[0], pString, pos + 8);
		} else if (name == "asec") {
			pString->insert("acos(1/())", pos);
			getNodeString(node->children[0], pString, pos + 8);
		} else if (name == "pow") {
			pString->insert("pow_c()", pos);
			getNodeString(node->children[0], pString, pos + 6);
		} else if (name == "min") {
			pString->insert("min(,)", pos);
			getNodeString(node->children[1], pString, pos + 5);
			getNodeString(node->children[0], pString, pos + 4);
		} else if (name == "max") {
			pString->insert("max(,)", pos);
			getNodeString(node->children[1], pString, pos + 5);
			getNodeString(node->children[0], pString, pos + 4);
		} else if (name == "sigma") {
			pString->insert("()", pos);
			getNodeString(node->children[0], pString, pos + 1);
		} else {
			pString->insert(name + "()", pos);
			getNodeString(node->children[0], pString, pos + name.len + 1);
		}
	}
}
