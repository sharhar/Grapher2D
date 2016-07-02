#pragma once

#include "../utils/String.h"

typedef struct Opperator {
	char name;
	int order;
	double(*func) (double, double);
} Opperator;

typedef struct Function {
	String* name;
	double(*func) (double*);
} Function;

class Equation {
private:
	String m_string;
	std::vector<Opperator*> m_ops;
	std::vector<Function*> m_funcs;
	void* m_rootNode;
public:
	void setString(String string);
	void addOpperator(Opperator* op);
	void addFunction(Function* func);
	void parse();
	void eval();
};