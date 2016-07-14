#include "Equation.h"
#include <math.h>

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
double evalNode(Node* node);

double addO(double n1, double n2) {
	return n1 + n2;
}

double mulO(double n1, double n2) {
	return n1 * n2;
}

double subO(double n1, double n2) {
	return n1 - n2;
}

double divO(double n1, double n2) {
	return n1 / n2;
}

double powO(double n1, double n2) {
	return pow(n1, n2);
}

double eqO(double n1, double n2) {
	if (n1 == n2) {
		return 1;
	}

	return 0;
}

double tanF(double* input) {
	return tan(input[0]);
}

double sinF(double* input) {
	return sin(input[0]);
}

double cosF(double* input) {
	return cos(input[0]);
}

double cotF(double* input) {
	return 1/tan(input[0]);
}

double cscF(double* input) {
	return 1/sin(input[0]);
}

double secF(double* input) {
	return 1/cos(input[0]);
}

double atanF(double* input) {
	return atan(input[0]);
}

double asinF(double* input) {
	return asin(input[0]);
}

double acosF(double* input) {
	return acos(input[0]);
}

double acotF(double* input) {
	return atan(1 / input[0]);
}

double acscF(double* input) {
	return asin(1 / input[0]);
}

double asecF(double* input) {
	return acos(1 / input[0]);
}

double maxF(double* input) {
	if (input[0] >= input[1]) {
		return input[0];
	}
	else {
		return input[1];
	}
}

double lnF(double* input) {
	return log(input[0]);
}

double logF(double* input) {
	return log10(input[0]);
}

double expF(double* input) {
	return exp(input[0]);
}

double blankF(double* args) {
	return args[0];
}

void loadDefaults(Equation* e) {
	Opperator* addOp = (Opperator*)malloc(sizeof(Opperator));
	addOp->name = '+';
	addOp->order = 10;
	addOp->func = &addO;

	Opperator* mulOp = (Opperator*)malloc(sizeof(Opperator));
	mulOp->name = '*';
	mulOp->order = 8;
	mulOp->func = &mulO;

	Opperator* subOp = (Opperator*)malloc(sizeof(Opperator));
	subOp->name = '-';
	subOp->order = 10;
	subOp->func = &subO;

	Opperator* divOp = (Opperator*)malloc(sizeof(Opperator));
	divOp->name = '/';
	divOp->order = 8;
	divOp->func = &divO;

	Opperator* powOp = (Opperator*)malloc(sizeof(Opperator));
	powOp->name = '^';
	powOp->order = 6;
	powOp->func = &powO;

	Opperator* eqOp = (Opperator*)malloc(sizeof(Opperator));
	eqOp->name = '=';
	eqOp->order = 100;
	eqOp->func = &eqO;

	e->addOpperator(addOp);
	e->addOpperator(mulOp);
	e->addOpperator(subOp);
	e->addOpperator(divOp);
	e->addOpperator(powOp);
	e->addOpperator(eqOp);

	Function* funcTan = (Function*)malloc(sizeof(Function));
	funcTan->name = new String("tan");
	funcTan->func = &tanF;

	Function* funcSin = (Function*)malloc(sizeof(Function));
	funcSin->name = new String("sin");
	funcSin->func = &sinF;

	Function* funcCos = (Function*)malloc(sizeof(Function));
	funcCos->name = new String("cos");
	funcCos->func = &cosF;

	Function* funcMax = (Function*)malloc(sizeof(Function));
	funcMax->name = new String("max");
	funcMax->func = &maxF;

	Function* funcBlank = (Function*)malloc(sizeof(Function));
	funcBlank->name = new String("");
	funcBlank->func = &blankF;

	Function* funcLn = (Function*)malloc(sizeof(Function));
	funcLn->name = new String("ln");
	funcLn->func = &lnF;

	Function* funcLog = (Function*)malloc(sizeof(Function));
	funcLog->name = new String("log");
	funcLog->func = &logF;

	Function* funcExp = (Function*)malloc(sizeof(Function));
	funcExp->name = new String("exp");
	funcExp->func = &expF;

	Function* funcCot = (Function*)malloc(sizeof(Function));
	funcCot->name = new String("cot");
	funcCot->func = &cotF;

	Function* funcCsc = (Function*)malloc(sizeof(Function));
	funcCsc->name = new String("csc");
	funcCsc->func = &cscF;

	Function* funcSec = (Function*)malloc(sizeof(Function));
	funcSec->name = new String("sec");
	funcSec->func = &secF;



	Function* funcaTan = (Function*)malloc(sizeof(Function));
	funcaTan->name = new String("atan");
	funcaTan->func = &atanF;

	Function* funcaSin = (Function*)malloc(sizeof(Function));
	funcaSin->name = new String("asin");
	funcaSin->func = &asinF;

	Function* funcaCos = (Function*)malloc(sizeof(Function));
	funcaCos->name = new String("acos");
	funcaCos->func = &acosF;

	Function* funcaCot = (Function*)malloc(sizeof(Function));
	funcaCot->name = new String("acot");
	funcaCot->func = &acotF;

	Function* funcaCsc = (Function*)malloc(sizeof(Function));
	funcaCsc->name = new String("acsc");
	funcaCsc->func = &acscF;

	Function* funcaSec = (Function*)malloc(sizeof(Function));
	funcaSec->name = new String("asec");
	funcaSec->func = &asecF;

	e->addFunction(funcTan);
	e->addFunction(funcSin);
	e->addFunction(funcCos);
	e->addFunction(funcMax);
	e->addFunction(funcBlank);
	e->addFunction(funcLn);
	e->addFunction(funcLog);
	e->addFunction(funcExp);
	e->addFunction(funcCot);
	e->addFunction(funcCsc);
	e->addFunction(funcSec);
	e->addFunction(funcaTan);
	e->addFunction(funcaSin);
	e->addFunction(funcaCos);
	e->addFunction(funcaCot);
	e->addFunction(funcaCsc);
	e->addFunction(funcaSec);

	Variable* eVar = e->createVariable("e");
	eVar->value = 2.718281828459045;
	Variable* piVar = e->createVariable("pi");
	piVar->value = 3.141592653589793;
	Variable* tauVar = e->createVariable("tau");
	tauVar->value = 3.141592653589793*2;
}

Equation::Equation() {
	loadDefaults(this);
}

void Equation::setString(String string) {
	m_string = String(string);
}

void Equation::addOpperator(Opperator* op) {
	m_ops.push_back(op);
}

void Equation::addFunction(Function* func) {
	m_funcs.push_back(func);
}

Variable* Equation::createVariable(String name) {
	Variable* var = (Variable*)malloc(sizeof(Variable));
	var->name = new String(name);
	
	m_vars.push_back(var);
	return var;
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
			c == '9' || c == '.' || c == '~') {
			tempType = TK_TYPE_NUM;	
		}

		for (Opperator* o : m_ops) {
			if (o->name == c) {
				tempType = TK_TYPE_OPP;
			}
		}

		if (tempType == TK_TYPE_NUM && c == '~') {
			c = '-';
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
				for (Function* f : m_funcs) {
					if (f->name->length == 0) {
						tempNode->value[1] = (void*)f;
						break;
					}
				}
				unsortedNodes.push_back(tempNode);
			}
			else if (stack[i - 1]->type != TK_TYPE_STR) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)new String("");
				for (Function* f : m_funcs) {
					if (f->name->length == 0) {
						tempNode->value[1] = (void*)f;
						break;
					}
				}
				unsortedNodes.push_back(tempNode);
			}
		}
		else {
			if (i == stack.size() - 1) {
				Node* tempNode = (Node*)malloc(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				for (Variable* v:m_vars) {
					if (v->name->equals(*t->value)) {
						tempNode->value[0] = (void*)v;
						break;
					}
				}
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
				for (Variable* v : m_vars) {
					if (v->name->equals(*t->value)) {
						tempNode->value[0] = (void*)v;
						break;
					}
				}
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
				
				funcNode = (Node*)malloc(sizeof(Node));
				long argc = (long)funcNodeArr[0];
				funcNode->type = NODE_TYPE_FFN;
				funcNode->value = new void*[1];
				funcNode->value[0] = (void*)curFunc;
				funcNode->childNum = argc;
				funcNode->children = new Node*[argc];
				
				for (long i = 0; i < argc; i++) {
					funcNode->children[i] = (Node*)funcNodeArr[i + 1];
				}
				simple.push_back(funcNode);
				funcTemp.clear();
			}
		}
	}
	
	void*** parts = getParts(simple);
	long partNum = (long)parts[0][0];

	void** result = new void*[partNum+1];

	result[0] = (void*)partNum;
	
	for (long i = 1; i <= partNum;i++) {
		void** part = parts[i];
		long partLen = (long)part[0];
		std::vector<Node*> vec;
		for (long j = 1; j <= partLen;j++) {
			vec.push_back((Node*)part[j]);
		}
		Node* simplePart = parseSimpleExpression(vec);
		result[i] = (void*)simplePart;
	}

	return result;
}

double Equation::eval() {
	Node* root = (Node*)m_rootNode;
	double result = evalNode(root);
	return result;
}

double evalNode(Node* node) {
	if (node->type == NODE_TYPE_NUM) {
		return ((double*)(node->value[0]))[0];
	} else if (node->type == NODE_TYPE_VAR) {
		Variable* var = (Variable*)node->value[0];
		return var->value;
	} else if (node->type == NODE_TYPE_OPP) {
		Opperator* op = (Opperator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		double prevVal = evalNode(prev);
		double nextVal = evalNode(next);

		double result = (*op->func)(prevVal, nextVal);
		return result;
	} else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		int argc = node->childNum;
		double* vals = new double[argc];
		for (int i = 0; i < argc;i++) {
			vals[i] = evalNode(node->children[i]);
		}

		double result = (*func->func)(vals);
		delete[] vals;
		return result;
	}

	return -1;
}