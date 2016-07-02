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
	int childNum;
	Node** children;
	void** value;
} Node;

typedef struct Token {
	int type;
	String* value;
} Token;

void** parseExpression(std::vector<Node*> nodes);
double evalNode(Node* node, std::unordered_map<std::string, double>* vars);

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
	for (int i = 0; i < m_string.length; i++) {
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

		for (Opperator* o : m_ops) {
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
		}
		else if (lastType == tempType) {
			tempString = tempString + c;
		}
		else {
			Token* tempT = (Token*)malloc(sizeof(Token));
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

	for (Token* t : tempStack) {
		if ((t->type == TK_TYPE_OPP || t->type == TK_TYPE_FOP || t->type == TK_TYPE_FCL || t->type == TK_TYPE_FSP) && t->value->length > 1) {
			Token* temp = (Token*)malloc(sizeof(Token));
			temp->type = t->type;
			String* value = new String(t->value->buff[0]);
			temp->value = value;
			for (int i = 0; i < t->value->length; i++) {
				stack.push_back(temp);
			}
		}
		else {
			stack.push_back(t);
		}
	}

	std::vector<Node*> unsortedNodes;
	for (int i = 0; i < stack.size(); i++) {
		Token* t = stack[i];
		if (t->type == TK_TYPE_NUM) {
			std::string tempString;
			String val = *(t->value);
			for (int i = 0; i < val.length; i++) {
				tempString.push_back(val.buff[i]);
			}
			std::string::size_type sz;
			double value = std::stod(tempString, &sz);
			double* d = new double[1];
			d[0] = value;
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_NUM;
			tempNode->value = new void*[1];
			tempNode->value[0] = (void*)d;
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_OPP) {
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_OPP;
			tempNode->value = new void*[2];
			tempNode->value[0] = (void*)&(t->value->buff[0]);
			for (Opperator* o : m_ops) {
				if (o->name == t->value->buff[0]) {
					tempNode->value[1] = (void*)o;
					break;
				}
			}
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_FSP) {
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_FSP;
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_FCL) {
			Node* tempNode = (Node*)malloc(sizeof(Node));
			tempNode->type = NODE_TYPE_FCL;
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_FOP) {
			if (i == 0) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)new String("");
				tempNode->value[1] = NULL;
				unsortedNodes.push_back(tempNode);
			}
			else if (stack[i - 1]->type != TK_TYPE_STR) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)new String("");
				tempNode->value[1] = NULL;
				unsortedNodes.push_back(tempNode);
			}
		}
		else {
			if (i == stack.size() - 1) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				tempNode->value[0] = (void*)t->value;
				unsortedNodes.push_back(tempNode);
				continue;
			}
			Token* next = stack[i + 1];

			if (next->type == TK_TYPE_FOP) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)t->value;
				for (Function* f : m_funcs) {
					if (f->name->equals(*t->value)) {
						tempNode->value[1] = (void*)f;
						break;
					}
				}
				unsortedNodes.push_back(tempNode);
			}
			else {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				tempNode->value[0] = (void*)t->value;
				unsortedNodes.push_back(tempNode);
			}
		}
	}

	void** result = parseExpression(unsortedNodes);
	m_rootNode = result[1];
}

void*** getParts(std::vector<Node*> nodes) {
	std::vector<std::vector<Node*>> parts;
	parts.push_back(std::vector<Node*>());
	int part = 0;
	bool e = false;
	for (Node* node:nodes) {
		e = true;
		if (node->type == NODE_TYPE_FSP) {
			part++;
			parts.push_back(std::vector<Node*>());
		} else {
			parts[part].push_back(node);
		}
	}

	void*** result;
	if (e) {
		result = new void**[parts.size() + 1];
		result[0] = new void*[1];
		result[0][0] = (void*)parts.size();

		for (int i = 1; i <= parts.size(); i++) {
			int length = parts[i - 1].size();
			void** arr = new void*[length + 1];
			arr[0] = (void*)length;
			for (int j = 1; j <= length; j++) {
				arr[j] = (void*)parts[i - 1][j - 1];
			}

			result[i] = arr;
		}
	} else {
		result = new void**[1];
		result[0] = new void*[1];
		result[0][0] = (void*)0;
	}
	
	return result;
}

Node* parseSimpleExpression(std::vector<Node*> nodes) {
	if (nodes.size() == 1) {
		return nodes[0];
	}
	
	int level = -1;
	for (Node* node:nodes) {
		if (node->type == NODE_TYPE_OPP) {
			Opperator* op = (Opperator*)node->value[1];
			if (op->order > level) {
				level = op->order;
			}
		}
	}

	for (Node* node:nodes) {
		if (node->type == NODE_TYPE_OPP) {
			Opperator* op = (Opperator*)node->value[1];
		}
	}

	std::vector<Node*> tempVec;
	for (int i = 0; i < nodes.size();i++) {
		Node* node = nodes[i];
		if (node->type != NODE_TYPE_OPP) {
			tempVec.push_back(node);
		} else if (node->type == NODE_TYPE_OPP) {
			Opperator* op = (Opperator*)node->value[1];
			if (op->order != level) {
				tempVec.push_back(node);
			} else {
				Node* prev = parseSimpleExpression(tempVec);
				std::vector<Node*> temp2;
				int loc = -1;
				for (int j = i + 1; j < nodes.size();j++) {
					Node* node2 = nodes[j];
					if (node2->type == NODE_TYPE_OPP) {
						Opperator* op2 = (Opperator*)node2->value[1];
						if (op2->order == level) {
							loc = j;
							break;
						} else {
							temp2.push_back(node2);
						}
					} else {
						temp2.push_back(node2);
					}
				}
				Node* next = parseSimpleExpression(temp2);

				node->childNum = 2;
				node->children = new Node*[2];
				node->children[0] = prev;
				node->children[1] = next;

				if (loc != -1) {
					i = loc - 1;
					tempVec.clear();
					tempVec.push_back(node);
				} else {
					return node;
				}
			}
		}
	}
	return NULL;
}

void** parseExpression(std::vector<Node*> nodes) {
	std::vector<Node*> simple;
	std::vector<Node*> funcTemp;
	Function* curFunc = NULL;
	int level = 0;
	for (Node* node:nodes) {
		if (level == 0) {
			if (node->type == NODE_TYPE_FUN) {
				curFunc = (Function*) node->value[1];
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
				Node* funcNode = NULL;
				if (curFunc == NULL) {
					funcNode = (Node*)funcNodeArr[1];
				} else {
					funcNode = (Node*)malloc(sizeof(Node));
					int argc = (int)funcNodeArr[0];
					funcNode->type = NODE_TYPE_FFN;
					funcNode->value = new void*[1];
					funcNode->value[0] = (void*)curFunc;
					funcNode->childNum = argc;
					funcNode->children = new Node*[argc];
					
					for (int i = 0; i < argc; i++) {
						funcNode->children[i] = (Node*)funcNodeArr[i + 1];
					}
				}
				simple.push_back(funcNode);
				funcTemp.clear();
			}
		}
	}

	
	
	void*** parts = getParts(simple);
	int partNum = (int)parts[0][0];

	void** result = new void*[partNum+1];

	result[0] = (void*)partNum;
	
	for (int i = 1; i <= partNum;i++) {
		void** part = parts[i];
		int partLen = (int)part[0];
		std::vector<Node*> vec;
		for (int j = 1; j <= partLen;j++) {
			vec.push_back((Node*)part[j]);
		}
		Node* simplePart = parseSimpleExpression(vec);
		result[i] = (void*)simplePart;
	}

	return result;
}

void Equation::setVar(std::string name, double value) {
	m_vars[name] = value;
}

double Equation::eval() {
	Node* root = (Node*)m_rootNode;
	double result = evalNode(root, &m_vars);
	return result;
}

double evalNode(Node* node, std::unordered_map<std::string, double>* vars) {
	if (node->type == NODE_TYPE_NUM) {
		double result = ((double*)(node->value[0]))[0];
		return result;
	} else if (node->type == NODE_TYPE_VAR) {
		String* nameP = (String*)node->value[0];
		std::string name = nameP->getstdstring();
		double result = (*vars)[name];
		return result;
	} else if (node->type == NODE_TYPE_OPP) {
		Opperator* op = (Opperator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		double prevVal = evalNode(prev, vars);
		double nextVal = evalNode(next, vars);

		double result = (*op->func)(prevVal, nextVal);
		return result;
	} else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		int argc = node->childNum;
		double* vals = new double[argc];
		for (int i = 0; i < argc;i++) {
			vals[i] = evalNode(node->children[i], vars);
		}

		double result = (*func->func)(vals);
		return result;
	}

	return -1;
}