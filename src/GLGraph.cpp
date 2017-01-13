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

String getNodeString(Node* node);

GLGraph::GLGraph(Equation* e) {
	String eqt = getNodeString((Node*)e->getRootNode());

	calcShader = new GraphCalcShader(eqt.getstdstring());
	renderShader = new GraphRenderShader();

	GraphQuad::init();

	glGenTextures(1, &dtex);
	glBindTexture(GL_TEXTURE_2D, dtex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, 1200, 1200);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLGraph::render(glui::Color graphColor, float up, float down, float left, float right, float time, float atime) {
	glBindImageTexture(0, dtex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);

	calcShader->bind();
	calcShader->setUniforms(up, down,left, right, time, atime);
	GraphQuad::bind();
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	GraphQuad::unbind();
	calcShader->unbind();

	glFlush();

	renderShader->bind();
	renderShader->setUniforms(0, graphColor);
	GraphQuad::bind();
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	GraphQuad::unbind();
	renderShader->unbind();
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
		} else if (name == "log") {
			result = "(log(" + arg + ")/log(10))";
		} else if (name == "cot") {
			result = "(1/tan(" + arg + "))";
		} else if (name == "csc") {
			result = "(1/sin(" + arg + "))";
		} else if (name == "sec") {
			result = "(1/cos(" + arg + "))";
		} else if (name == "acot") {
			result = "atan(1/(" + arg + "))";
		} else if (name == "acsc") {
			result = "asin(1/(" + arg + "))";
		} else if (name == "asec") {
			result = "acos(1/(" + arg + "))";
		} else {
			result = name + "(" + arg + ")";
		}

		return result;
	}

	return "";
}