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

class Equation {
private:
	String m_string;
	std::vector<Opperator*> m_ops;
	std::vector<Function*> m_funcs;
	std::unordered_map<std::string, double> m_vars;
	void* m_rootNode;
public:
	void setString(String string);
	void addOpperator(Opperator* op);
	void addFunction(Function* func);
	void setVar(std::string name, double value);
	void parse();
	double eval();
};