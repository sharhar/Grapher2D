#include "Equation.h"

#define TK_TYPE_OPP 1
#define TK_TYPE_NUM 2
#define TK_TYPE_STR 3
#define TK_TYPE_FOP 4
#define TK_TYPE_FCL 5
#define TK_TYPE_FSP 6

#define NODE_TYPE_OPP 1
#define NODE_TYPE_NUM 2
#define NODE_TYPE_FUN 3
#define NODE_TYPE_FSP 4
#define NODE_TYPE_VAR 5
#define NODE_TYPE_FCL 6
#define NODE_TYPE_EXP 7
#define NODE_TYPE_FFN 8

typedef struct Node {
	int type;
	Node** children;
	void** value;
} Node;

typedef struct Token {
	int type;
	String* value;
} Token;

void** parseExpression(std::vector<Node*> nodes);

void Equation::setString(String string) {
	m_string = String(string);
}

void Equation::addOpperator(Opperator* op) {
	m_ops.push_back(op);
}

void Equation::addFunction(Function* func) {
	m_funcs.push_back(func);
}

void Equation::parse() {
	std::vector<Token*> tempStack;

	int tempType;
	int lastType = 0;
	String tempString;
	for (int i = 0; i < m_string.length;i++) {
		char c = m_string[i];
		if (c == ' ') {
			continue;
		}
		tempType = 0;
		if (c == '0' || c == '1' || c == '2' ||
			c == '3' || c == '4' || c == '5' ||
			c == '6' || c == '7' || c == '8' ||
			c == '9' || c == '.') {
			tempType = TK_TYPE_NUM;
		}

		for (Opperator* o:m_ops) {
			if (o->name == c) {
				tempType = TK_TYPE_OPP;
			}
		}

		if (c == '(') {
			tempType = TK_TYPE_FOP;
		}

		if (c == ')') {
			tempType = TK_TYPE_FCL;
		}

		if (c == ',') {
			tempType = TK_TYPE_FSP;
		}

		if (tempType == 0) {
			tempType = TK_TYPE_STR;
		}

		if (lastType == 0) {
			tempString = tempString + c;
			lastType = tempType;
		} else if(lastType == tempType){
			tempString = tempString + c;
		} else {
			Token* tempT = (Token*) malloc(sizeof(Token));
			tempT->type = lastType;
			tempT->value = new String(tempString);
			tempStack.push_back(tempT);
			lastType = tempType;
			tempString = String();
			tempString = tempString + c;
		}
	}

	Token* tempT = (Token*)malloc(sizeof(Token));
	tempT->type = lastType;
	tempT->value = new String(tempString);
	tempStack.push_back(tempT);

	std::vector<Token*> stack;

	for (Token* t:tempStack) {
		if ((t->type == TK_TYPE_OPP || t->type == TK_TYPE_FOP || t->type == TK_TYPE_FCL || t->type == TK_TYPE_FSP) && t->value->length > 1) {
			Token* temp = (Token*)malloc(sizeof(Token));
			temp->type = t->type;
			String* value = new String(t->value->buff[0]);
			temp->value = value;
			for (int i = 0; i < t->value->length;i++) {
				stack.push_back(temp);
			}
		} else {
			stack.push_back(t);
		}
	}

	std::vector<Node*> unsortedNodes;
	for (int i = 0; i < stack.size();i++) {
		Token* t = stack[i];
		if (t->type == TK_TYPE_NUM) {
			std::string tempString;
			String val = *(t->value);
			for (int i = 0; i < val.length;i++) {
				tempString.push_back(val.buff[i]);
			}
			std::string::size_type sz;
			double value = std::stod(tempString, &sz);
			double* d = new double[1];
			d[0] = value;
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_NUM;
			tempNode->value = new void*[1];
			tempNode->value[0] = (void*) d;
			unsortedNodes.push_back(tempNode);
		} else if (t->type == TK_TYPE_OPP) {
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_OPP;
			tempNode->value = new void*[2];
			tempNode->value[0] = (void*) &(t->value->buff[0]);
			for (Opperator* o : m_ops) {
				if (o->name == t->value->buff[0]) {
					tempNode->value[1] = (void*) o;
					break;
				}
			}
			unsortedNodes.push_back(tempNode);
		} else if (t->type == TK_TYPE_FSP) {
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_FSP;
			unsortedNodes.push_back(tempNode);
		} else if (t->type == TK_TYPE_FCL) {
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_FCL;
			unsortedNodes.push_back(tempNode);
		} else if (t->type == TK_TYPE_FOP) {
			if (i == 0) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[1];
				tempNode->value[0] = (void*)new String("");
				unsortedNodes.push_back(tempNode);
			} else if (stack[i-1]->type != TK_TYPE_STR) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[1];
				tempNode->value[0] = (void*)new String("");
				unsortedNodes.push_back(tempNode);
			}
		} else {
			if (i == stack.size() - 1) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				tempNode->value[0] = (void*) t->value;
				unsortedNodes.push_back(tempNode);
				continue;
			}
			Token* next = stack[i+1];

			if (next->type == TK_TYPE_FOP) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)t->value;
				for (Function* f : m_funcs) {
					if (f->name.equals(*t->value)) {
						tempNode->value[1] = (void*)f;
						break;
					}
				}
				unsortedNodes.push_back(tempNode);
			} else {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				tempNode->value[0] = (void*)t->value;
				unsortedNodes.push_back(tempNode);
			}
		}
	}

	void** result = parseExpression(unsortedNodes);
}

void** parseExpression(std::vector<Node*> nodes) {
	std::vector<Node*> simple;
	std::vector<Node*> funcTemp;
	void** curFunc;
	int level = 0;
	for (Node* node:nodes) {
		if (level == 0) {
			if (node->type == NODE_TYPE_FUN) {
				curFunc = node->value;
				level++;
			} else {
				simple.push_back(node);
			}
		} else {
			if (node->type == NODE_TYPE_FUN) {
				level++;
			} else if (node->type == NODE_TYPE_FCL) {
				level--;
			}
			if (level != 0) {
				funcTemp.push_back(node);
			} else {
				void** funcNodeArr = parseExpression(funcTemp);
				Node* funcNode = (Node*)funcNodeArr[1];
				simple.push_back(funcNode);
				funcTemp.clear();
			}
		}
	}

	Node* resultN = (Node*)malloc(sizeof(Node));
	resultN->type = 0;
	
	void** result = new void*[2];
	result[0] = (void*)1;
	result[1] = (void*)resultN;
	return result;
}

void Equation::eval() {
	
}