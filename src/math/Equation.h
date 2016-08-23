#pragma once

#include "../utils/String.h"
#include <unordered_map>
#include <functional>

typedef struct Operator {
	char name;
	int order;
	double(*func) (double, double);
} Operator;

typedef struct Function {
	String* name;
	double(*func) (double*);
} Function;

typedef struct Variable {
	String* name;
	double value;
} Variable;

class Equation {
private:
	String m_string;
	std::vector<Operator*> m_ops;
	std::vector<Function*> m_funcs;
	std::vector<Variable*> m_vars;
	std::vector<String*> m_strings;
	void* m_rootNode;
	bool m_parsed;

	std::vector<void*> m_nodes;
public:
	Equation();

	String* parseError;
	void setString(String string);
	String getString() { return m_string; }
	Variable* createVariable(String name);
	void addOperator(Operator* op);
	void addFunction(Function* func);
	String* parse();
	double eval();
	void cleanUp();
	void* allocNode();
};