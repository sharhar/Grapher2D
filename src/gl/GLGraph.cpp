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

typedef struct VarNode {
	String* name;
	int id;
	VarNode* next;
};

void getNodeString(Node* node, String* pString, String* pFuncsString, int pos, int* funcID, VarNode* currentVars, std::vector<String*>* pError);

GLGraph::GLGraph(Equation* e, int portSize, String** error) {
	inited = false;
	
	String* eqt = new String("");
	String* funcs = new String("");
	int funcNum = 0;

	std::vector<String*>* errors = new std::vector<String*>();

	VarNode* piVarNode = (VarNode*)malloc(sizeof(VarNode));
	piVarNode->name = new String("pi");
	piVarNode->id = 0;
	piVarNode->next = NULL;

	VarNode* tauVarNode = (VarNode*)malloc(sizeof(VarNode));
	tauVarNode->name = new String("tau");
	tauVarNode->id = 1;
	tauVarNode->next = piVarNode;

	VarNode* eVarNode = (VarNode*)malloc(sizeof(VarNode));
	eVarNode->name = new String("e");
	eVarNode->id = 2;
	eVarNode->next = tauVarNode;

	VarNode* tVarNode = (VarNode*)malloc(sizeof(VarNode));
	tVarNode->name = new String("t");
	tVarNode->id = 3;
	tVarNode->next = eVarNode;

	VarNode* atVarNode = (VarNode*)malloc(sizeof(VarNode));
	atVarNode->name = new String("at");
	atVarNode->id = 4;
	atVarNode->next = tVarNode;

	VarNode* xVarNode = (VarNode*)malloc(sizeof(VarNode));
	xVarNode->name = new String("x");
	xVarNode->id = 5;
	xVarNode->next = atVarNode;

	VarNode* yVarNode = (VarNode*)malloc(sizeof(VarNode));
	yVarNode->name = new String("y");
	yVarNode->id = 6;
	yVarNode->next = xVarNode;

	getNodeString((Node*)e->getRootNode(), eqt, funcs, 0, &funcNum, yVarNode, errors);

	if (errors->size() != 0) {
		String** errorsData = errors->data();
		error[0] = errorsData[0];
		return;
	}

	calcShader = new GraphCalcShader(funcs->getstdstring(), eqt->getstdstring());

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

	inited = true;
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
	if (inited) {
		glDeleteTextures(1, &dtex);
		glDeleteTextures(1, &etex);

		calcShader->cleanUp();
		delete calcShader;
	}
}

void getNodeString(Node* node, String* pString, String* pFuncsString, int pos, int* funcID, VarNode* currentVars, std::vector<String*>* pError) {
	if (node->type == NODE_TYPE_ZRO) {
		pString->insert((char*)"0", pos);
	} else if (node->type == NODE_TYPE_NUM) {
		pString->insert(std::to_string(((double*)(node->value[0]))[0]), pos);
	} else if (node->type == NODE_TYPE_VAR) {
		Variable* var = (Variable*)node->value[0];
		String variableName = *var->name;

		VarNode* cVar = currentVars;
		for (int i = 0; i < currentVars->id + 1;i++) {
			if (cVar->name->equals(variableName)) {
				pString->insert(variableName, pos);
				return;
			}

			cVar = cVar->next;
		}

		pString->insert(variableName, pos);
		pError->push_back(new String("Unkown Variable: " + variableName));
	} else if (node->type == NODE_TYPE_OPP) {
		Operator* op = (Operator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		if (op->name == '+') {
			pString->insert("()+()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (op->name == '-') {
			pString->insert("()-()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (op->name == '*') {
			pString->insert("()*()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (op->name == '/') {
			pString->insert("()/()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (op->name == '=') {
			pString->insert("()-()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (op->name == '^') {
			pString->insert("pow_c(,)", pos);
			getNodeString(next, pString, pFuncsString, pos + 7, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 6, funcID, currentVars, pError);
		}
		
	}
	else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		if (func == NULL) {
			getNodeString(node->children[0], pString, pFuncsString, pos, funcID, currentVars, pError);
			return;
		}

		String name = *func->name;

		if (name == "ln") {
			pString->insert("log()", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 4, funcID, currentVars, pError);
		}
		else if (name == "log") {
			pString->insert("(log()/log(10.0))", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 5, funcID, currentVars, pError);
		}
		else if (name == "cot") {
			pString->insert("(1/tan())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 7, funcID, currentVars, pError);
		}
		else if (name == "csc") {
			pString->insert("(1/sin())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 7, funcID, currentVars, pError);
		}
		else if (name == "sec") {
			pString->insert("(1/cos())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 7, funcID, currentVars, pError);
		}
		else if (name == "acot") {
			pString->insert("atan(1/())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 8, funcID, currentVars, pError);
		}
		else if (name == "acsc") {
			pString->insert("asin(1/())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 8, funcID, currentVars, pError);
		}
		else if (name == "asec") {
			pString->insert("acos(1/())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 8, funcID, currentVars, pError);
		}
		else if (name == "pow") {
			pString->insert("pow_c()", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 6, funcID, currentVars, pError);
		}
		else if (name == "min") {
			pString->insert("min(,)", pos);
			getNodeString(node->children[1], pString, pFuncsString, pos + 5, funcID, currentVars, pError);
			getNodeString(node->children[0], pString, pFuncsString, pos + 4, funcID, currentVars, pError);
		}
		else if (name == "max") {
			pString->insert("max(,)", pos);
			getNodeString(node->children[1], pString, pFuncsString, pos + 5, funcID, currentVars, pError);
			getNodeString(node->children[0], pString, pFuncsString, pos + 4, funcID, currentVars, pError);
		}
		else if (name == "sigma") {
			std::string addFunctionName = "custom_math_func_" + std::to_string(*funcID);

			String* incrementName = new String("");
			String* incrementStart = new String("");
			String* incrementEnd = new String("");

			String* tempFuncString = new String("");

			if (node->children[0]->type == NODE_TYPE_VAR) {
				Variable* var = (Variable*)node->children[0]->value[0];
				incrementName->insert(*var->name, 0);
			} else {
				pError->push_back(new String("Increment argument of sigma function was not a variable!"));
				return;
			}

			VarNode* tempVarNode = (VarNode*)malloc(sizeof(VarNode));
			tempVarNode->name = incrementName;
			tempVarNode->id = currentVars->id + 1;
			tempVarNode->next = currentVars;

			getNodeString(node->children[1], incrementStart, tempFuncString, 0, funcID, tempVarNode, pError);
			getNodeString(node->children[2], incrementEnd, tempFuncString, 0, funcID, tempVarNode, pError);
			
			std::string addVarArgs = "";
			std::string addVarCall = "";

			VarNode* cVar = currentVars;
			for (int i = 0; i < currentVars->id - 6; i++) {
				addVarArgs += ", float " + cVar->name->getstdstring();
				addVarCall += ", " + cVar->name->getstdstring();

				cVar = cVar->next;
			}

			std::string addFunctionStringPre = "";
			addFunctionStringPre += "float " + addFunctionName + "(float x, float y" + addVarArgs + ") {\n";
			addFunctionStringPre += "float result = 0;\n";
			addFunctionStringPre += "for(float "
				+ incrementName->getstdstring() + " = "
				+ incrementStart->getstdstring() + "; "
				+ incrementName->getstdstring() + " < "
				+ incrementEnd->getstdstring() + "; "
				+ incrementName->getstdstring() + "++) {\n";
			addFunctionStringPre += "result += ";

			std::string addFunctionStringPost = "";
			addFunctionStringPost += ";\n";
			addFunctionStringPost += "}\n";
			addFunctionStringPost += "return result;";
			addFunctionStringPost += "\n}\n";

			*funcID = *funcID + 1;

			pFuncsString->insert(addFunctionStringPre + addFunctionStringPost, 0);
			getNodeString(node->children[3], pFuncsString, tempFuncString, addFunctionStringPre.size(), funcID, tempVarNode, pError);
			pFuncsString->insert(*tempFuncString, 0);

			pString->insert(addFunctionName + "(x, y" + addVarCall + ")", pos);
		}
		else if (name == "integral") {
			std::string addFunctionName = "custom_math_func_" + std::to_string(*funcID);

			String* integrationVar = new String("");
			String* integrationStart = new String("");
			String* integrationEnd = new String("");
			String* integrationDivisions = new String("");

			String* tempFuncString = new String("");

			if (node->children[0]->type == NODE_TYPE_VAR) {
				Variable* var = (Variable*)node->children[0]->value[0];
				integrationVar->insert(*var->name, 0);
			}
			else {
				pError->push_back(new String("Increment argument of sigma function was not a variable!"));
				return;
			}

			VarNode* tempVarNode = (VarNode*)malloc(sizeof(VarNode));
			tempVarNode->name = integrationVar;
			tempVarNode->id = currentVars->id + 1;
			tempVarNode->next = currentVars;

			getNodeString(node->children[1], integrationStart, tempFuncString, 0, funcID, currentVars, pError);
			getNodeString(node->children[2], integrationEnd, tempFuncString, 0, funcID, currentVars, pError);
			getNodeString(node->children[3], integrationDivisions, tempFuncString, 0, funcID, currentVars, pError);

			std::string addVarArgs = "";
			std::string addVarCall = "";

			VarNode* cVar = currentVars;
			for (int i = 0; i < currentVars->id - 6; i++) {
				addVarArgs += ", float " + cVar->name->getstdstring();
				addVarCall += ", " + cVar->name->getstdstring();

				cVar = cVar->next;
			}

			*funcID = *funcID + 1;

			String* pFuncName = new String("");

			getNodeString(node->children[4], pFuncName, tempFuncString, 0, funcID, tempVarNode, pError);

			std::string addFunctionString = "";
			addFunctionString += "float " + addFunctionName + "(float x, float y" + addVarArgs + ") {\n";
			
			addFunctionString += "float result0_ = 0;\n";
			addFunctionString += "float result1_ = 0;\n";

			addFunctionString += "float a_ = " + integrationStart->getstdstring() + ";\n";
			addFunctionString += "float b_ = " + integrationEnd->getstdstring() + ";\n";
			addFunctionString += "float n_ = " + integrationDivisions->getstdstring() + ";\n";

			addFunctionString += "float dx_ = (b_ - a_)/n_;\n";

			addFunctionString += "float " + integrationVar->getstdstring() + " = a_;\n";

			addFunctionString += "result0_ += " + pFuncName->getstdstring() + ";\n";
			
			addFunctionString += integrationVar->getstdstring() + " = b_;\n";

			addFunctionString += "result0_ += " + pFuncName->getstdstring() + ";\n";

			addFunctionString += "for(float k_ = 1; k_ < n_; k_++) {\n";
			addFunctionString += integrationVar->getstdstring() + " = a_+(k_*dx_);\n";
			addFunctionString += "result1_ += 2*" + pFuncName->getstdstring() + ";\n";
			addFunctionString += "}\n";

			addFunctionString += "return (dx_/2)*(result0_ + result1_);;\n";

			addFunctionString += "\n}\n";

			pFuncsString->insert(addFunctionString, 0);
			pFuncsString->insert(*tempFuncString, 0);

			pString->insert(addFunctionName + "(x, y" + addVarCall + ")", pos);
		}
		else {
			pString->insert(name + "()", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + name.len + 1, funcID, currentVars, pError);
		}
	}
}
