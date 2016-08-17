#pragma once

#include "../utils/String.h"
#include <unordered_map>

typedef struct Opperator {
	char name;
	int order;
	double(*func) (double, double);
} Opperator;

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
	std::vector<Opperator*> m_ops;
	std::vector<Function*> m_funcs;
	std::vector<Variable*> m_vars;
	void* m_rootNode;
public:
	Equation();
	void setString(String string);
	String getString() { return m_string; }
	Variable* createVariable(String name);
	void addOpperator(Opperator* op);
	void addFunction(Function* func);
	void parse();
	double eval();
};