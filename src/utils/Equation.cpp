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
#define NODE_TYPE_ZRO 9

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

void** parseExpression(std::vector<Node*> nodes, Equation* e);
double evalNode(Node* node, Equation* e);

void* Equation::allocNode() {
	Node* result = (Node*)malloc(sizeof(Node));
	result->childNum = 0;
	result->children = NULL;
	result->value = NULL;
	result->type = 0;
	m_nodes.push_back(result);
	return result;
}

void* Equation::getRootNode() {
	return m_rootNode;
}

void loadDefaults(Equation* e) {
	Operator* addOp = (Operator*)malloc(sizeof(Operator));
	addOp->name = '+';
	addOp->order = 10;
	addOp->func = [](double n1, double n2)->double {
		return n1 + n2;
	};

	Operator* mulOp = (Operator*)malloc(sizeof(Operator));
	mulOp->name = '*';
	mulOp->order = 8;
	mulOp->func = [](double n1, double n2)->double {
		return n1 * n2;
	};
	
	Operator* subOp = (Operator*)malloc(sizeof(Operator));
	subOp->name = '-';
	subOp->order = 10;
	subOp->func = [](double n1, double n2)->double {
		return n1 - n2;
	};
	
	Operator* divOp = (Operator*)malloc(sizeof(Operator));
	divOp->name = '/';
	divOp->order = 8;
	divOp->func = [](double n1, double n2)->double {
		return n1 / n2;
	};

	Operator* powOp = (Operator*)malloc(sizeof(Operator));
	powOp->name = '^';
	powOp->order = 6;
	powOp->func = [](double n1, double n2)->double {
		int pi = (int)(n2);
		if (n2 == (double)(pi)) {
			double result = n1;
			for (int i = 1; i < pi;i++) {
				result *= n1;
			}

			return result;
		}
		return pow(n1, n2);
	};

	Operator* equOp = (Operator*)malloc(sizeof(Operator));
	equOp->name = '=';
	equOp->order = 100;
	equOp->func = [](double n1, double n2)->double {
		return n1 - n2;
	};

	e->addOperator(addOp);
	e->addOperator(mulOp);
	e->addOperator(subOp);
	e->addOperator(divOp);
	e->addOperator(powOp);
	e->addOperator(equOp);


	Function* funcTan = (Function*)malloc(sizeof(Function));
	funcTan->name = new String((char*)"tan");
	funcTan->func = [](int len, double* input)->double {
		return tan(input[0]);
	};

	Function* funcSin = (Function*)malloc(sizeof(Function));
	funcSin->name = new String((char*)"sin");
	funcSin->func = [](int len, double* input)->double {
		return sin(input[0]);
	};

	
	Function* funcCos = (Function*)malloc(sizeof(Function));
	funcCos->name = new String((char*)"cos");
	funcCos->func = [](int len, double* input)->double {
		return cos(input[0]);
	};

	Function* funcCot = (Function*)malloc(sizeof(Function));
	funcCot->name = new String((char*)"cot");
	funcCot->func = [](int len, double* input)->double {
		return 1/tan(input[0]);
	};

	Function* funcCsc = (Function*)malloc(sizeof(Function));
	funcCsc->name = new String((char*)"csc");
	funcCsc->func = [](int len, double* input)->double {
		return 1/sin(input[0]);
	};

	Function* funcSec = (Function*)malloc(sizeof(Function));
	funcSec->name = new String((char*)"sec");
	funcSec->func = [](int len, double* input)->double {
		return 1/cos(input[0]);
	};

	Function* funcaTan = (Function*)malloc(sizeof(Function));
	funcaTan->name = new String((char*)"atan");
	funcaTan->func = [](int len, double* input)->double {
		return atan(input[0]);
	};

	Function* funcaSin = (Function*)malloc(sizeof(Function));
	funcaSin->name = new String((char*)"asin");
	funcaSin->func = [](int len, double* input)->double {
		return asin(input[0]);
	};

	Function* funcaCos = (Function*)malloc(sizeof(Function));
	funcaCos->name = new String((char*)"acos");
	funcaCos->func = [](int len, double* input)->double {
		return acos(input[0]);
	};

	Function* funcaCot = (Function*)malloc(sizeof(Function));
	funcaCot->name = new String((char*)"acot");
	funcaCot->func = [](int len, double* input)->double {
		return atan(1 / input[0]);
	};

	Function* funcaCsc = (Function*)malloc(sizeof(Function));
	funcaCsc->name = new String((char*)"acsc");
	funcaCsc->func = [](int len, double* input)->double {
		return asin(1 / input[0]);
	};

	Function* funcaSec = (Function*)malloc(sizeof(Function));
	funcaSec->name = new String((char*)"asec");
	funcaSec->func = [](int len, double* input)->double {
		return acos(1 / input[0]);
	};

	Function* funcMax = (Function*)malloc(sizeof(Function));
	funcMax->name = new String((char*)"max");
	funcMax->func = [](int len, double* input)->double {
		if (input[0] >= input[1]) {
			return input[0];
		}
		else {
			return input[1];
		}
	};

	Function* funcMin = (Function*)malloc(sizeof(Function));
	funcMin->name = new String((char*)"min");
	funcMin->func = [](int len, double* input)->double {
		if (input[0] <= input[1]) {
			return input[0];
		}
		else {
			return input[1];
		}
	};

	Function* funcLn = (Function*)malloc(sizeof(Function));
	funcLn->name = new String((char*)"ln");
	funcLn->func = [](int len, double* input)->double {
		return log(input[0]);
	};

	Function* funcLog = (Function*)malloc(sizeof(Function));
	funcLog->name = new String((char*)"log");
	funcLog->func = [](int len, double* input)->double {
		return log10(input[0]);
	};

	Function* funcExp = (Function*)malloc(sizeof(Function));
	funcExp->name = new String((char*)"exp");
	funcExp->func = [](int len, double* input)->double {
		return exp(input[0]);
	};

	Function* funcAbs = (Function*)malloc(sizeof(Function));
	funcAbs->name = new String((char*)"abs");
	funcAbs->func = [](int len, double* input)->double {
		return input[0] > 0 ? input[0] : -(input[0]);
	};

	Function* funcMod = (Function*)malloc(sizeof(Function));
	funcMod->name = new String((char*)"mod");
	funcMod->func = [](int len, double* input)->double {
		return modf(input[0], &input[1]);
	};

	Function* funcPow = (Function*)malloc(sizeof(Function));
	funcPow->name = new String((char*)"pow");
	funcPow->func = [](int len, double* input)->double {
		return pow(input[0], input[1]);
	};

	Function* funcSqrt = (Function*)malloc(sizeof(Function));
	funcSqrt->name = new String((char*)"sqrt");
	funcSqrt->func = [](int len, double* input)->double {
		return sqrt(input[0]);
	};

	Function* funcSign = (Function*)malloc(sizeof(Function));
	funcSign->name = new String((char*)"sign");
	funcSign->func = [](int len, double* input)->double {
		return sqrt(input[0]);
	};

	Function* funcFloor = (Function*)malloc(sizeof(Function));
	funcFloor->name = new String((char*)"floor");
	funcFloor->func = [](int len, double* input)->double {
		return floor(input[0]);
	};

	Function* funcSigma = (Function*)malloc(sizeof(Function));
	funcSigma->name = new String((char*)"sigma");
	funcSigma->func = [](int len, double* input)->double {
		return floor(input[0]);
	};

	e->addFunction(funcTan);
	e->addFunction(funcSin);
	e->addFunction(funcCos);
	e->addFunction(funcCot);
	e->addFunction(funcCsc);
	e->addFunction(funcSec);
	e->addFunction(funcaTan);
	e->addFunction(funcaSin);
	e->addFunction(funcaCos);
	e->addFunction(funcaCot);
	e->addFunction(funcaCsc);
	e->addFunction(funcaSec);
	e->addFunction(funcMax);
	e->addFunction(funcMod);
	e->addFunction(funcMin);
	e->addFunction(funcLn);
	e->addFunction(funcLog);
	e->addFunction(funcExp);
	e->addFunction(funcAbs);
	e->addFunction(funcPow);
	e->addFunction(funcSqrt);
	e->addFunction(funcSign);
	e->addFunction(funcFloor);
	e->addFunction(funcSigma);

	e->createVariable("e")->value = 2.718281828459045;
	e->createVariable("pi")->value = 3.141592653589793;
	e->createVariable("tau")->value = 3.141592653589793 * 2;
}

Equation::Equation() {
	loadDefaults(this);
	parseError = NULL;
	m_parsed = false;
}

void Equation::setString(String string) {
	m_string = String(string);
}

void Equation::addOperator(Operator* op) {
	m_ops.push_back(op);
}

void Equation::addFunction(Function* func) {
	m_funcs.push_back(func);
}

Variable* Equation::createVariable(String name) {
	Variable* var = (Variable*)malloc(sizeof(Variable));
	var->name = new String(name);
	var->value = 0;
	
	m_vars.push_back(var);
	return var;
}

String* Equation::parse() {
	std::vector<Token*> tempStack;
	int tempType;
	int lastType = 0;
	String tempString;
	for (int i = 0; i < m_string.len; i++) {
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

		for (Operator* o : m_ops) {
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
		if ((t->type == TK_TYPE_OPP || t->type == TK_TYPE_FOP || t->type == TK_TYPE_FCL || t->type == TK_TYPE_FSP) && t->value->len > 1) {
			for (int i = 0; i < t->value->len; i++) {
				Token* temp = (Token*)malloc(sizeof(Token));
				temp->type = t->type;
				String* value = new String(t->value->buff[0]);
				temp->value = value;

				stack.push_back(temp);
			}

			free(t);
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
			for (int i = 0; i < val.len; i++) {
				tempString.push_back(val.buff[i]);
			}
			std::string::size_type sz;
			double value = std::stod(tempString, &sz);
			double* d = new double[1];
			d[0] = value;
			Node* tempNode = (Node*)allocNode();
			tempNode->type = NODE_TYPE_NUM;
			tempNode->value = new void*[1];
			tempNode->value[0] = (void*)d;
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_OPP) {
			Node* tempNode = (Node*)allocNode();
			tempNode->type = NODE_TYPE_OPP;
			tempNode->value = new void*[2];
			tempNode->value[0] = (void*)&(t->value->buff[0]);
			tempNode->value[1] = NULL;
			for (Operator* o : m_ops) {
				if (o->name == t->value->buff[0]) {
					tempNode->value[1] = (void*)o;
					break;
				}
			}

			if (tempNode->value[1] == NULL) {
				return new String("Unknown operator: " + String(t->value->buff));
			}

			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_FSP) {
			Node* tempNode = (Node*)allocNode();
			tempNode->type = NODE_TYPE_FSP;
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_FCL) {
			Node* tempNode = (Node*)allocNode();
			tempNode->type = NODE_TYPE_FCL;
			unsortedNodes.push_back(tempNode);
		}
		else if (t->type == TK_TYPE_FOP) {
			if (i == 0) {
				Node* tempNode = (Node*)allocNode();
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)new String((char*)"");
				tempNode->value[1] = NULL;

				unsortedNodes.push_back(tempNode);
			}
			else if (stack[i - 1]->type != TK_TYPE_STR) {
				Node* tempNode = (Node*)allocNode();
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)new String((char*)"");
				tempNode->value[1] = NULL;

				unsortedNodes.push_back(tempNode);
			}
		}
		else {
			if (i == stack.size() - 1) {
				Node* tempNode = (Node*)allocNode();
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				tempNode->value[0] = NULL;
				for (Variable* v:m_vars) {
					if (v->name->equals(*t->value)) {
						tempNode->value[0] = (void*)v;
						break;
					}
				}

				if (tempNode->value[0] == NULL) {
					return new String("Unknown variable: " + *t->value);
				}

				unsortedNodes.push_back(tempNode);
				continue;
			}
			Token* next = stack[i + 1];

			if (next->type == TK_TYPE_FOP) {
				Node* tempNode = (Node*)allocNode();
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = new void*[2];
				tempNode->value[0] = (void*)t->value;
				tempNode->value[1] = NULL;
				for (Function* f : m_funcs) {
					if (f->name->equals(*t->value)) {
						tempNode->value[1] = (void*)f;
						break;
					}
				}

				if (tempNode->value[1] == NULL) {
					return new String("Unknown function: " + *t->value);
				}

				unsortedNodes.push_back(tempNode);
			}
			else {
				Node* tempNode = (Node*)allocNode();
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = new void*[1];
				tempNode->value[0] = NULL;
				for (Variable* v : m_vars) {
					if (v->name->equals(*t->value)) {
						tempNode->value[0] = (void*)v;
						break;
					}
				}

				if (tempNode->value[0] == NULL) {
					return new String("Unknown variable: " + *t->value);
				}

				unsortedNodes.push_back(tempNode);
			}
		}
	}

	void** result = parseExpression(unsortedNodes, this);

	if (result == NULL) {
		if (parseError != NULL) {
			free(result);
			return parseError;
		}
		else {
			free(result);
			return new String((char*)"Unkown error");
		}
	}

	for (Token* p:stack) {
		m_strings.push_back(p->value);
		free(p);
	}

	m_rootNode = result[1];
	m_parsed = true;
	return NULL;
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

Node* parseSimpleExpression(std::vector<Node*> nodes, Equation* e) {
	if (nodes.size() == 0) {
		return NULL;
	}
	
	if (nodes.size() == 1) {
		return nodes[0];
	}

	int level = -1;
	for (Node* node:nodes) {
		if (node->type == NODE_TYPE_OPP) {
			
			Operator* op = (Operator*)node->value[1];
			if (op->order > level) {
				level = op->order;
			}
		}
	}

	for (Node* node:nodes) {
		if (node->type == NODE_TYPE_OPP) {
			Operator* op = (Operator*)node->value[1];
		}
	}

	std::vector<Node*> tempVec;
	for (int i = 0; i < nodes.size();i++) {
		Node* node = nodes[i];
		if (node->type != NODE_TYPE_OPP) {
			tempVec.push_back(node);
		} else if (node->type == NODE_TYPE_OPP) {
			Operator* op = (Operator*)node->value[1];
			if (op->order != level) {
				tempVec.push_back(node);
			} else {
				Node* prev = parseSimpleExpression(tempVec, e);

				if (prev == NULL) {
					if (op->name == '-') {
						prev = (Node*)e->allocNode();
						prev->type = NODE_TYPE_ZRO;
					} else {
						e->parseError = new String(String((char*)"Left side of operator '") + String(op->name) + String((char*)"' is blank"));
						return NULL;
					}
				}

				std::vector<Node*> temp2;
				int loc = -1;
				for (int j = i + 1; j < nodes.size();j++) {
					Node* node2 = nodes[j];
					if (node2->type == NODE_TYPE_OPP) {
						Operator* op2 = (Operator*)node2->value[1];
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
				Node* next = parseSimpleExpression(temp2, e);

				if (next == NULL) {
					e->parseError = new String(String((char*)"Right side of operator '") + String(op->name) + String((char*)"' is blank"));
					return NULL;
				}

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

void** parseExpression(std::vector<Node*> nodes, Equation* e) {
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
				void** funcNodeArr = parseExpression(funcTemp, e);

				if (funcNodeArr == NULL) {
					return NULL;
				}

				Node* funcNode = NULL;
				
				funcNode = (Node*)e->allocNode();
				long argc = (long)funcNodeArr[0];
				funcNode->type = NODE_TYPE_FFN;
				funcNode->value = new void*[2];
				funcNode->value[0] = (void*)curFunc;
				funcNode->value[1] = new double[argc];
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

	if (level != 0) {
		e->parseError = new String((char*)"Parentheses error");
		return NULL;
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
		Node* simplePart = parseSimpleExpression(vec, e);

		if (simplePart == NULL) {
			return NULL;
		}
		result[i] = (void*)simplePart;
	}

	return result;
}

double Equation::eval() {
	Node* root = (Node*)m_rootNode;
	double result = evalNode(root, this);
	return result;
}

double evalNode(Node* node, Equation* e) {
	if (node->type == NODE_TYPE_ZRO) {
		return 0;
	}
	else if (node->type == NODE_TYPE_NUM) {
		return ((double*)(node->value[0]))[0];
	}
	else if (node->type == NODE_TYPE_VAR) {
		Variable* var = (Variable*)node->value[0];
		return var->value;
	}
	else if (node->type == NODE_TYPE_OPP) {
		Operator* op = (Operator*)node->value[1];
		Node* prev = node->children[0];
		Node* next = node->children[1];

		double prevVal = evalNode(prev, e);
		double nextVal = evalNode(next, e);

		double result = op->func(prevVal, nextVal);
		return result;
	}
	else if (node->type == NODE_TYPE_FFN) {
		Function* func = (Function*)node->value[0];
		if (func == NULL) {
			return evalNode(node->children[0], e);
		}

		double* values = (double*)node->value[1];

		for (int i = 0; i < node->childNum; i++) {
			values[i] = evalNode(node->children[i], e);
		}

		double result = func->func(node->childNum, values);
		return result;
	}

	return -1;
}

void Equation::cleanUp() {
	for (void* p : m_nodes) {
		Node* n = (Node*)p;

		if (n->type == NODE_TYPE_FFN) {
			delete[] n->value[1];
		}

		if (n->children != NULL) {
			delete[] n->children;
		}

		if (n->value != NULL) {
			delete[] n->value;
		}

		free(p);
	}

	for (Variable* v : m_vars) {
		delete v->name;

		free(v);
	}

	for (Operator* o : m_ops) {
		free(o);
	}

	for (Function* f : m_funcs) {
		delete f->name;

		free(f);
	}

	for (String* s : m_strings) {
		delete s;
	}
}
