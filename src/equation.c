#include "equation.h"
#include <stdio.h>
#include <math.h>

void* malloc(size_t);
void free(void*);

size_t allocationLogNum = 0;
size_t allocationLogSize = 0;
void** allocationLog;

void* malloc_c(size_t size) {
	void* result = malloc(size);
	memset(result, 0, size);
	allocationLogNum += size;
	
	void** newLog = malloc(sizeof(void*) * (allocationLogSize + 1));
	memcpy(newLog, allocationLog, sizeof(void*) * allocationLogSize);
	newLog[allocationLogSize] = result;
	allocationLogSize++;
	free(allocationLog);

	allocationLog = newLog;

	return result;
}

void cleanUp() {
	printf("Allocated: %d bytes\n", allocationLogNum);
	printf("Allocations: %d\n", allocationLogSize);

	for (int i = 0; i < allocationLogSize;i++) {
		free(allocationLog[i]);
	}

	free(allocationLog);
}

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
	struct Node** children;
	char* value;
} Node;

typedef struct Token {
	int type;
	char* value;
} Token;

typedef struct VarNode {
	char* name;
	int id;
	struct VarNode* next;
} VarNode;

ParsingInfo* eqGetDefaultParseInfo() {
	ParsingInfo* result = malloc(sizeof(ParsingInfo));

	result->vars = malloc(sizeof(char*) * 5);
	result->vars[0] = "x";
	result->vars[1] = "y";
	result->vars[2] = "t";
	result->vars[3] = "at";
	result->vars[4] = NULL;
	
	result->funcs = malloc(sizeof(char*) * 29);
	result->funcs[0]  = "tan";
	result->funcs[1]  = "sin";
	result->funcs[2]  = "cos";
	result->funcs[3]  = "cot";
	result->funcs[4]  = "csc";
	result->funcs[5]  = "sec";
	result->funcs[6]  = "atan";
	result->funcs[7]  = "asin";
	result->funcs[8]  = "acos";
	result->funcs[9]  = "acot";
	result->funcs[10] = "acsc";
	result->funcs[11] = "asec";
	result->funcs[12] = "max";
	result->funcs[13] = "min";
	result->funcs[14] = "ln";
	result->funcs[15] = "log";
	result->funcs[16] = "exp";
	result->funcs[17] = "abs";
	result->funcs[18] = "mod";
	result->funcs[19] = "pow";
	result->funcs[20] = "sqrt";
	result->funcs[21] = "sign";
	result->funcs[22] = "floor";
	result->funcs[23] = "sigma";
	result->funcs[24] = "fact";
	result->funcs[25] = "PI";
	result->funcs[26] = "integral";
	result->funcs[27] = "gamma";
	result->funcs[28] = NULL;

	return result;
}

void*** getParts(Node** nodes, int nodeLength) {
	if (nodeLength == 0) {
		void*** result = malloc_c(sizeof(void**));
		result[0] = malloc_c(sizeof(void*));
		result[0][0] = 0;
		return result;
	}

	int sepNum = 0;
	for (int i = 0; i < nodeLength; i++) {
		if (nodes[i]->type == NODE_TYPE_FSP) {
			sepNum++;
		}
	}

	int* partSize = malloc_c(sizeof(int) * (sepNum + 1));
	int part = 0;

	for (int i = 0; i < nodeLength; i++) {
		if (nodes[i]->type == NODE_TYPE_FSP) {
			part++;
		} else {
			partSize[part]++;
		}
	}


	Node*** parts = malloc_c(sizeof(Node**) * (sepNum + 1));

	for (int i = 0; i < sepNum + 1;i++) {
		parts[i] = malloc_c(sizeof(Node*) * partSize[i]);
	}

	int sep = 0;
	part = 0;
	for (int i = 0; i < nodeLength; i++) {
		if (nodes[i]->type == NODE_TYPE_FSP) {
			sep++;
			part = 0;
		}
		else {
			parts[sep][part] = nodes[i];
			part++;
		}
	}

	void*** result;

	result = malloc_c(sizeof(void**) * (sepNum + 2));
	result[0] = malloc_c(sizeof(void*));
	result[0][0] = (void*)(sepNum + 1);

	for (int i = 1; i <= sepNum + 1; i++) {
		int length = partSize[i - 1];
		void** arr = malloc_c(sizeof(void*) * (length + 1));
		arr[0] = (void*)length;
		for (int j = 1; j <= length; j++) {
			arr[j] = (void*)parts[i - 1][j - 1];
		}

		result[i] = arr;
	}

	return result;
}

int getOpporatorOrder(char op) {
	if (op == '^') {
		return 1;
	}

	if (op == '*' || op == '/') {
		return 2;
	}

	if (op == '+' || op == '-') {
		return 3;
	}

	if (op == '=') {
		return 4;
	}

	printf("Error!\n");

	return -1;
}

Node* parseSimpleExpression(Node** nodes, int nodeLength, char** error) {
	if (nodeLength == 0) {
		return NULL;
	}

	if (nodeLength == 1) {
		return nodes[0];
	}

	int level = -1;
	for (int i = 0; i < nodeLength;i++) {
		if (nodes[i]->type == NODE_TYPE_OPP) {
			int opLevel = getOpporatorOrder(nodes[i]->value[0]);
			if (opLevel > level) {
				level = opLevel;
			}
		}
	}

	Node** tempVec = malloc_c(sizeof(Node*) * nodeLength);
	int tempVecPos = 0;
	for (int i = 0; i < nodeLength;i++) {
		Node* node = nodes[i];
		if (node->type != NODE_TYPE_OPP) {
			tempVec[tempVecPos] = node;
			tempVecPos++;
		}
		else if (node->type == NODE_TYPE_OPP) {
			if (getOpporatorOrder(node->value[0]) != level) {
				tempVec[tempVecPos] = node;
				tempVecPos++;
			}
			else {
				Node* prev = parseSimpleExpression(tempVec, tempVecPos, error);

				if (prev == NULL) {
					if (node->value[0] == '-') {
						prev = (Node*)malloc_c(sizeof(Node));
						prev->type = NODE_TYPE_ZRO;
					}
					else {
						if (*error != NULL) {
							return NULL;
						}

						char* prefix = "Left side of operator '";
						char* sufix = "' is blank";

						int totalLength = strlen(prefix) + strlen(node->value) + strlen(sufix);

						char* finalError = malloc_c(sizeof(char) * (totalLength + 1));

						strcat(finalError, prefix);
						strcat(finalError, node->value);
						strcat(finalError, sufix);

						*error = finalError;

						return NULL;
					}
				}

				Node** temp2 = malloc_c(sizeof(Node*) * nodeLength);
				int temp2Pos = 0;
				int loc = -1;
				for (int j = i + 1; j < nodeLength; j++) {
					Node* node2 = nodes[j];
					if (node2->type == NODE_TYPE_OPP) {
						if (getOpporatorOrder(node2->value[0]) == level) {
							loc = j;
							break;
						}
						else {
							temp2[temp2Pos] = node2;
							temp2Pos++;
						}
					}
					else {
						temp2[temp2Pos] = node2;
						temp2Pos++;
					}
				}
				Node* next = parseSimpleExpression(temp2, temp2Pos, error);

				if (next == NULL) {
					if (*error != NULL) {
						return NULL;
					}

					char* prefix = "Right side of operator '";
					char* sufix = "' is blank";

					int totalLength = strlen(prefix) + strlen(node->value) + strlen(sufix);

					char* finalError = malloc_c(sizeof(char) * (totalLength + 1));

					strcat(finalError, prefix);
					strcat(finalError, node->value);
					strcat(finalError, sufix);

					*error = finalError;

					return NULL;
				}

				node->childNum = 2;
				node->children = malloc_c(sizeof(Node*) * 2);
				node->children[0] = prev;
				node->children[1] = next;

				if (loc != -1) {
					i = loc - 1;
					tempVecPos = 0;

					tempVec[tempVecPos] = node;
					tempVecPos++;
				}
				else {
					return node;
				}
			}
		}
	}

	*error = "Missing opperator";

	return NULL;
}

void** parseExpression(Node** nodes, int nodeLength, char** error) {
	Node** simple = malloc_c(sizeof(Node*) * nodeLength);
	int simplePos = 0;

	Node** funcTemp = malloc_c(sizeof(Node*) * nodeLength);
	int funcTempPos = 0;

	char* curFunc = NULL;
	int level = 0;

	for (int i = 0; i < nodeLength;i++) {
		Node* node = nodes[i];

		if (level == 0) {
			if (node->type == NODE_TYPE_FUN) {
				curFunc = node->value;
				level++;
			} else {
				simple[simplePos] = node;
				simplePos++;
			}
		} else {
			if (node->type == NODE_TYPE_FUN) {
				level++;
			} else if (node->type == NODE_TYPE_FCL) {
				level--;
			}
			if (level != 0) {
				funcTemp[funcTempPos] = node;
				funcTempPos++;
			} else {
				void** funcNodeArr = parseExpression(funcTemp, funcTempPos, error);

				if (funcNodeArr == NULL) {
					return NULL;
				}

				Node* funcNode = NULL;
				
				funcNode = (Node*)malloc_c(sizeof(Node));
				long argc = (long)funcNodeArr[0];
				funcNode->type = NODE_TYPE_FFN;
				funcNode->value = curFunc;
				funcNode->childNum = argc;
				funcNode->children = malloc_c(sizeof(Node*) * argc);
				
				for (long i = 0; i < argc; i++) {
					funcNode->children[i] = (Node*)funcNodeArr[i + 1];
				}

				simple[simplePos] = funcNode;
				simplePos++;

				funcTempPos = 0;
			}
		}
	}

	if (level != 0) {
		*error = "Parentheses error";
		return NULL;
	}

	void*** parts = getParts(simple, simplePos);

	long partNum = (long)parts[0][0];

	void** result = malloc_c(sizeof(void*) * (partNum + 1));

	result[0] = (void*)partNum;

	for (long i = 1; i <= partNum;i++) {
		void** part = parts[i];
		long partLen = (long)part[0];
		Node** vec = malloc_c(sizeof(Node*)  * partLen);
		for (long j = 1; j <= partLen;j++) {
			vec[j-1] = (Node*)part[j];
		}

		Node* simplePart = parseSimpleExpression(vec, partLen, error);

		if (simplePart == NULL) {
			return NULL;
		}
		result[i] = (void*)simplePart;
	}

	return result;
}

char* strInsert(char* src, char* regex, int pos) {
	int totalLength = strlen(src) + strlen(regex);

	char* result = malloc_c(sizeof(char) * (totalLength + 1));
	memcpy(result, src, sizeof(char) * pos);
	strcat(result, regex);
	strcat(result, src + sizeof(char) * pos);

	return result;
}

char* strConcat(char* str1, char* str2) {
	int totalLength = strlen(str1) + strlen(str2);

	char* result = malloc_c(sizeof(char) * (totalLength + 1));
	strcat(result, str1);
	strcat(result, str2);

	return result;
}

void getNodeString(Node* node, char** pString, char** pFuncsString, int pos, int* funcID, VarNode* currentVars, char** pError) {
	if (node->type == NODE_TYPE_ZRO) {
		*pString = strInsert(*pString, "0", pos);
	}
	else if (node->type == NODE_TYPE_NUM) {
		*pString = strInsert(*pString, node->value, pos);
	}
	else if (node->type == NODE_TYPE_VAR) {
		VarNode* cVar = currentVars;
		for (int i = 0; i < currentVars->id + 1;i++) {
			if (strcmp(node->value, cVar->name) == 0) {
				*pString = strInsert(*pString, node->value, pos);
				return;
			}

			cVar = cVar->next;
		}

		*pString = strInsert(*pString, node->value, pos);

		char* prefix = "Unkown Variable: ";

		int totalLength = strlen(prefix) + strlen(node->value);

		char* finalError = malloc_c(sizeof(char) * (totalLength + 1));
		strcat(finalError, prefix);
		strcat(finalError, node->value);

		*pError = finalError;
	}
	else if (node->type == NODE_TYPE_OPP) {
		Node* prev = node->children[0];
		Node* next = node->children[1];

		if (node->value[0] == '+') {
			*pString = strInsert(*pString, "()+()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (node->value[0] == '-') {
			*pString = strInsert(*pString, "()-()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (node->value[0] == '*') {
			*pString = strInsert(*pString, "()*()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (node->value[0] == '/') {
			*pString = strInsert(*pString, "()/()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (node->value[0] == '=') {
			*pString = strInsert(*pString, "()-()", pos);
			getNodeString(next, pString, pFuncsString, pos + 4, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 1, funcID, currentVars, pError);
		}
		else if (node->value[0] == '^') {
			*pString = strInsert(*pString, "pow_c(,)", pos);
			getNodeString(next, pString, pFuncsString, pos + 7, funcID, currentVars, pError);
			getNodeString(prev, pString, pFuncsString, pos + 6, funcID, currentVars, pError);
		}

	}
	else if (node->type == NODE_TYPE_FFN) {
		char* name = node->value;
		if (name == NULL) {
			getNodeString(node->children[0], pString, pFuncsString, pos, funcID, currentVars, pError);
			return;
		}

		if (strcmp(name, "ln") == 0) {
			*pString = strInsert(*pString, "log()", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 4, funcID, currentVars, pError);
		}
		else if (strcmp(name, "log") == 0) {
			*pString = strInsert(*pString, "(log()/log(10.0))", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 5, funcID, currentVars, pError);
		}
		else if (strcmp(name, "cot") == 0) {
			*pString = strInsert(*pString, "(1/tan())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 7, funcID, currentVars, pError);
		}
		else if (strcmp(name, "csc") == 0) {
			*pString = strInsert(*pString, "(1/sin())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 7, funcID, currentVars, pError);
		}
		else if (strcmp(name, "sec") == 0) {
			*pString = strInsert(*pString, "(1/cos())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 7, funcID, currentVars, pError);
		}
		else if (strcmp(name, "acot") == 0) {
			*pString = strInsert(*pString, "atan(1/())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 8, funcID, currentVars, pError);
		}
		else if (strcmp(name, "acsc") == 0) {
			*pString = strInsert(*pString, "asin(1/())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 8, funcID, currentVars, pError);
		}
		else if (strcmp(name, "asec") == 0) {
			*pString = strInsert(*pString, "acos(1/())", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 8, funcID, currentVars, pError);
		}
		else if (strcmp(name, "pow") == 0) {
			*pString = strInsert(*pString, "pow_c()", pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + 6, funcID, currentVars, pError);
		}
		else if (strcmp(name, "min") == 0) {
			*pString = strInsert(*pString, "min(,)", pos);
			getNodeString(node->children[1], pString, pFuncsString, pos + 5, funcID, currentVars, pError);
			getNodeString(node->children[0], pString, pFuncsString, pos + 4, funcID, currentVars, pError);
		}
		else if (strcmp(name, "max") == 0) {
			*pString = strInsert(*pString, "max(,)", pos);
			getNodeString(node->children[1], pString, pFuncsString, pos + 5, funcID, currentVars, pError);
			getNodeString(node->children[0], pString, pFuncsString, pos + 4, funcID, currentVars, pError);
		}
		else if (strcmp(name, "sigma") == 0) {
			char* prefixAddFuncName = "custom_math_func_";
			int numLen = (int)ceil(log10(fmax(*funcID, 1.2f)));
			char* numStr = malloc_c(sizeof(char) * (numLen + 1));
			sprintf(numStr, "%d", *funcID);

			*funcID = *funcID + 1;

			int addFunctionNameLen = strlen(numStr) + strlen(prefixAddFuncName);

			char* addFunctionName = malloc_c(sizeof(char) * (addFunctionNameLen + 1));
			strcat(addFunctionName, prefixAddFuncName);
			strcat(addFunctionName, numStr);

			char* incrementName = "";
			char* incrementStart = "";
			char* incrementEnd = "";

			char* tempFuncString = "";

			if (node->children[0]->type == NODE_TYPE_VAR) {
				incrementName = node->children[0]->value;
			} else {
				*pError = "Increment argument of sigma function was not a variable!";
				return;
			}

			VarNode* tempVarNode = (VarNode*)malloc(sizeof(VarNode));
			tempVarNode->name = incrementName;
			tempVarNode->id = currentVars->id + 1;
			tempVarNode->next = currentVars;

			getNodeString(node->children[1], &incrementStart, &tempFuncString, 0, funcID, tempVarNode, pError);
			getNodeString(node->children[2], &incrementEnd, &tempFuncString, 0, funcID, tempVarNode, pError);

			char* addVarArgs = "";
			char* addVarCall = "";

			VarNode* cVar = currentVars;
			for (int i = 0; i < currentVars->id - 6; i++) {
				addVarArgs = strConcat(addVarArgs, ", float ");
				addVarArgs = strConcat(addVarArgs, cVar->name);

				addVarCall = strConcat(addVarCall, ", ");
				addVarCall = strConcat(addVarCall, cVar->name);

				cVar = cVar->next;
			}

			char* addFunctionStringPre = "float ";
			addFunctionStringPre = strConcat(addFunctionStringPre, addFunctionName);
			addFunctionStringPre = strConcat(addFunctionStringPre, "(float x, float y");
			addFunctionStringPre = strConcat(addFunctionStringPre, addVarArgs);
			addFunctionStringPre = strConcat(addFunctionStringPre, ") {\n");
			addFunctionStringPre = strConcat(addFunctionStringPre, "float result = 0;\n");
			addFunctionStringPre = strConcat(addFunctionStringPre, "for(float ");
			addFunctionStringPre = strConcat(addFunctionStringPre, incrementName);
			addFunctionStringPre = strConcat(addFunctionStringPre, " = ");
			addFunctionStringPre = strConcat(addFunctionStringPre, incrementStart);
			addFunctionStringPre = strConcat(addFunctionStringPre, "; ");
			addFunctionStringPre = strConcat(addFunctionStringPre, incrementName);
			addFunctionStringPre = strConcat(addFunctionStringPre, " < ");
			addFunctionStringPre = strConcat(addFunctionStringPre, incrementEnd);
			addFunctionStringPre = strConcat(addFunctionStringPre, "; ");
			addFunctionStringPre = strConcat(addFunctionStringPre, incrementName);
			addFunctionStringPre = strConcat(addFunctionStringPre, "++) {\n");
			addFunctionStringPre = strConcat(addFunctionStringPre, "result += ");

			char* addFunctionStringPost = ";\n}\nreturn result;\n}\n";

			*pFuncsString = strInsert(*pFuncsString, strConcat(addFunctionStringPre, addFunctionStringPost), 0);
			getNodeString(node->children[3], pFuncsString, &tempFuncString, strlen(addFunctionStringPre), funcID, tempVarNode, pError);
			*pFuncsString = strInsert(*pFuncsString, tempFuncString, 0);

			*pString = strInsert(*pString, strConcat(strConcat(addFunctionName, "(x, y"), strConcat(addVarCall, ")")), pos);
		}
		else if (strcmp(name, "integral") == 0) {
			char* prefixAddFuncName = "custom_math_func_";
			int numLen = (int)ceil(log10(fmax(*funcID, 1.2f)));
			char* numStr = malloc_c(sizeof(char) * (numLen + 1));
			sprintf(numStr, "%d", *funcID);

			*funcID = *funcID + 1;

			int addFunctionNameLen = strlen(numStr) + strlen(prefixAddFuncName);

			char* addFunctionName = malloc_c(sizeof(char) * (addFunctionNameLen + 1));
			strcat(addFunctionName, prefixAddFuncName);
			strcat(addFunctionName, numStr);

			char* integrationVar = "";
			char* integrationStart = "";
			char* integrationEnd = "";
			char* integrationDivisions = "";

			char* tempFuncString = "";

			if (node->children[0]->type == NODE_TYPE_VAR) {
				integrationVar = node->children[0]->value;
			} else {
				*pError = "Integration variable was not a variable!";
				return;
			}

			VarNode* tempVarNode = (VarNode*)malloc(sizeof(VarNode));
			tempVarNode->name = integrationVar;
			tempVarNode->id = currentVars->id + 1;
			tempVarNode->next = currentVars;

			getNodeString(node->children[1], &integrationStart, &tempFuncString, 0, funcID, currentVars, pError);
			getNodeString(node->children[2], &integrationEnd, &tempFuncString, 0, funcID, currentVars, pError);
			getNodeString(node->children[3], &integrationDivisions, &tempFuncString, 0, funcID, currentVars, pError);

			char* addVarArgs = "";
			char* addVarCall = "";

			VarNode* cVar = currentVars;
			for (int i = 0; i < currentVars->id - 6; i++) {
				addVarArgs = strConcat(addVarArgs, ", float ");
				addVarArgs = strConcat(addVarArgs, cVar->name);

				addVarCall = strConcat(addVarCall, ", ");
				addVarCall = strConcat(addVarCall, cVar->name);

				cVar = cVar->next;
			}

			char* pFuncName = "";

			getNodeString(node->children[4], &pFuncName, &tempFuncString, 0, funcID, tempVarNode, pError);

			char* addFunctionString = "float ";
			addFunctionString = strConcat(addFunctionString, addFunctionName);
			addFunctionString = strConcat(addFunctionString, "(float x, float y");
			addFunctionString = strConcat(addFunctionString, addVarArgs);
			addFunctionString = strConcat(addFunctionString, ") {\n");
			addFunctionString = strConcat(addFunctionString, "float result0_ = 0;\nfloat result1_ = 0;\n");
			addFunctionString = strConcat(addFunctionString, "float a_ = ");
			addFunctionString = strConcat(addFunctionString, integrationStart);
			addFunctionString = strConcat(addFunctionString, ";\n");
			addFunctionString = strConcat(addFunctionString, "float b_ = ");
			addFunctionString = strConcat(addFunctionString, integrationEnd);
			addFunctionString = strConcat(addFunctionString, ";\n");
			addFunctionString = strConcat(addFunctionString, "float n_ = ");
			addFunctionString = strConcat(addFunctionString, integrationDivisions);
			addFunctionString = strConcat(addFunctionString, ";\n");
			addFunctionString = strConcat(addFunctionString, "float dx_ = (b_ - a_)/n_;\n");
			addFunctionString = strConcat(addFunctionString, "float ");
			addFunctionString = strConcat(addFunctionString, integrationVar);
			addFunctionString = strConcat(addFunctionString, " = a_;\n");
			addFunctionString = strConcat(addFunctionString, "result0_ += ");
			addFunctionString = strConcat(addFunctionString, pFuncName);
			addFunctionString = strConcat(addFunctionString, ";\n");
			addFunctionString = strConcat(addFunctionString, integrationVar);
			addFunctionString = strConcat(addFunctionString, " = b_;\n");
			addFunctionString = strConcat(addFunctionString, "result0_ += ");
			addFunctionString = strConcat(addFunctionString, pFuncName);
			addFunctionString = strConcat(addFunctionString, ";\n");
			addFunctionString = strConcat(addFunctionString, "for(float k_ = 1; k_ < n_; k_++) {\n");
			addFunctionString = strConcat(addFunctionString, integrationVar);
			addFunctionString = strConcat(addFunctionString, " = a_+(k_*dx_);\n");
			addFunctionString = strConcat(addFunctionString, "result1_ += 2*");
			addFunctionString = strConcat(addFunctionString, pFuncName);
			addFunctionString = strConcat(addFunctionString, ";\n");
			addFunctionString = strConcat(addFunctionString, "}\n");
			addFunctionString = strConcat(addFunctionString, "return (dx_/2)*(result0_ + result1_);\n");
			addFunctionString = strConcat(addFunctionString, "\n}\n");

			*pFuncsString = strInsert(*pFuncsString, addFunctionString, 0);
			*pFuncsString = strInsert(*pFuncsString, tempFuncString, 0);

			*pString = strInsert(*pString, strConcat(strConcat(addFunctionName, "(x, y"), strConcat(addVarCall, ")")), pos);
		}
		else {
			char* nameCall = malloc_c(sizeof(char) * (strlen(name) + 3));
			strcat(nameCall, name);
			strcat(nameCall, "()");
			*pString = strInsert(*pString, nameCall, pos);
			getNodeString(node->children[0], pString, pFuncsString, pos + strlen(name) + 1, funcID, currentVars, pError);
		}
	}
}

void eqConvert(ParsingInfo* parseInfo, char* pre_text, char** body, char** funcs, char** error) {
	allocationLogNum = 0;
	allocationLogSize = 0;

	int preTextLength = strlen(pre_text);

	char* text = NULL;
	int textLength = 0;

	for (int i = 0; i < preTextLength;i++) {
		if (pre_text[i] == '=') {
			text = pre_text;
			textLength = preTextLength;
			break;
		}
	}

	if (text == NULL) {
		text = malloc_c(sizeof(char) * preTextLength + 3);
		memset(text, 0, sizeof(char) * preTextLength + 3);
		strcat(text, pre_text);
		strcat(text, "=y");
		textLength = preTextLength + 2;
	}

	char* tempStr = "";
	int tempType;
	int lastType = 0;

	Token** tokenStack = malloc_c(sizeof(Token*) * (textLength + 1));
	memset(tokenStack, 0, sizeof(Token*) * (textLength + 1));
	int tokenStackPos = 0;

	for (int i = 0; i < textLength; i++) {
		char c = text[i];
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

		if (c == '+' || c == '-' || c == '*' ||
			c == '/' || c == '^' || c == '=') {
			tempType = TK_TYPE_OPP;
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
			int prevLen = strlen(tempStr);
			char* reserve = malloc_c(sizeof(char) * (prevLen + 2));
			memset(reserve, 0, sizeof(char) * (prevLen + 2));
			memcpy(reserve, tempStr, sizeof(char) * prevLen);
			reserve[sizeof(char) * prevLen] = c;
			
			tempStr = reserve;

			lastType = tempType;
		}
		else if (lastType == tempType) {
			int prevLen = strlen(tempStr);
			char* reserve = malloc_c(sizeof(char) * (prevLen + 2));
			memset(reserve, 0, sizeof(char) * (prevLen + 2));
			memcpy(reserve, tempStr, sizeof(char) * prevLen);
			reserve[sizeof(char) * prevLen] = c;
			
			tempStr = reserve;
		}
		else {
			int prevLen = strlen(tempStr);
			char* valueStr = malloc_c(sizeof(char) * (prevLen + 1));
			memset(valueStr, 0, sizeof(char) * (prevLen + 1));
			memcpy(valueStr, tempStr, sizeof(char) * prevLen);

			Token* tempT = (Token*)malloc_c(sizeof(Token));
			tempT->type = lastType;
			tempT->value = valueStr;

			tokenStack[tokenStackPos] = tempT;
			tokenStackPos++;

			lastType = tempType;

			tempStr = malloc_c(sizeof(char) * 2);
			tempStr[0] = c;
			tempStr[1] = '\0';
		}
	}

	Token* tempT = (Token*)malloc_c(sizeof(Token));
	tempT->type = lastType;
	tempT->value = tempStr;
	tokenStack[tokenStackPos] = tempT;
	tokenStackPos++;

	int trueTokenNum = 0;

	for (int i = 0; i < tokenStackPos;i++) {
		Token* t = tokenStack[i];

		if ((t->type == TK_TYPE_OPP || t->type == TK_TYPE_FOP || t->type == TK_TYPE_FCL || t->type == TK_TYPE_FSP) && strlen(t->value) > 1) {
			trueTokenNum += strlen(t->value);
		}
		else {
			trueTokenNum++;
		}
	}

	Token** stack = malloc_c(sizeof(Token*) * trueTokenNum);
	int stackPos = 0;

	for (int i = 0; i < tokenStackPos;i++) {
		Token* t = tokenStack[i];

		if ((t->type == TK_TYPE_OPP || t->type == TK_TYPE_FOP || t->type == TK_TYPE_FCL || t->type == TK_TYPE_FSP) && strlen(t->value) > 1) {
			int repeatLen = strlen(t->value);
			for (int i = 0; i < repeatLen; i++) {
				Token* temp = (Token*)malloc_c(sizeof(Token));
				temp->type = t->type;
				char* value = malloc_c(sizeof(char) * 2);
				value[0] = t->value[0];
				value[1] = '\0';
				temp->value = value;

				stack[stackPos] = temp;
				stackPos++;
			}
		}
		else {
			stack[stackPos] = t;
			stackPos++;
		}
	}

	Node** unsortedNodes = malloc_c(sizeof(Node*) * stackPos);
	int nodePos = 0;
	for (int i = 0; i < stackPos; i++) {
		Token* t = stack[i];
		
		if (t->type == TK_TYPE_NUM) {
			Node* tempNode = (Node*)malloc_c(sizeof(Node));
			tempNode->type = NODE_TYPE_NUM;
			tempNode->value = t->value;
			unsortedNodes[nodePos] = tempNode;
			nodePos++;
		}
		else if (t->type == TK_TYPE_OPP) {
			Node* tempNode = (Node*)malloc_c(sizeof(Node));
			tempNode->type = NODE_TYPE_OPP;
			tempNode->value = t->value;
			unsortedNodes[nodePos] = tempNode;
			nodePos++;
		}
		else if (t->type == TK_TYPE_FSP) {
			Node* tempNode = (Node*)malloc_c(sizeof(Node));
			tempNode->type = NODE_TYPE_FSP;
			unsortedNodes[nodePos] = tempNode;
			nodePos++;
		}
		else if (t->type == TK_TYPE_FCL) {
			Node* tempNode = (Node*)malloc_c(sizeof(Node));
			tempNode->type = NODE_TYPE_FCL;
			unsortedNodes[nodePos] = tempNode;
			nodePos++;
		}
		else if (t->type == TK_TYPE_FOP) {
			if (i == 0) {
				Node* tempNode = (Node*)malloc_c(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = NULL;

				unsortedNodes[nodePos] = tempNode;
				nodePos++;
			}
			else if (stack[i - 1]->type != TK_TYPE_STR) {
				Node* tempNode = (Node*)malloc_c(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = NULL;
				unsortedNodes[nodePos] = tempNode;
				nodePos++;
			}
		}
		else {
			if (i == stackPos - 1) {
				Node* tempNode = (Node*)malloc_c(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = t->value;
				unsortedNodes[nodePos] = tempNode;
				nodePos++;
				continue;
			}

			Token* next = stack[i + 1];

			if (next->type == TK_TYPE_FOP) {
				Node* tempNode = (Node*)malloc_c(sizeof(Node));
				tempNode->type = NODE_TYPE_FUN;
				tempNode->value = t->value;

				int found = 0;
				for (int j = 0; parseInfo->funcs[j] != NULL;j++) {
					if (strcmp(parseInfo->funcs[j], t->value) == 0) {
						found = 1;
						break;
					}
				}

				if (found == 0) {
					char* prefix = "Unknow Function: ";

					int totalLength = strlen(prefix) + strlen(t->value);

					char* errorResult = malloc(sizeof(char) * (totalLength + 1));
					memset(errorResult, 0, sizeof(char) * (totalLength + 1));

					strcat(errorResult, prefix);
					strcat(errorResult, t->value);

					*error = errorResult;

					cleanUp();

					return NULL;
				}

				unsortedNodes[nodePos] = tempNode;
				nodePos++;
			}
			else {
				Node* tempNode = (Node*)malloc_c(sizeof(Node));
				tempNode->type = NODE_TYPE_VAR;
				tempNode->value = t->value;
				unsortedNodes[nodePos] = tempNode;
				nodePos++;
			}
		}
	}

	void** parseResult = parseExpression(unsortedNodes, nodePos, error);

	if (parseResult == NULL) {
		if (*error != NULL) {
			char* preError = *error;
			*error = malloc(sizeof(char) * (strlen(preError) + 1));
			memset(*error, 0, sizeof(char) * (strlen(preError) + 1));
			strcat(*error, preError);
			cleanUp();
			return NULL;
		}
		else {
			cleanUp();
			*error = "Unkown error";
			return NULL;
		}
	}

	VarNode* piVarNode = (VarNode*)malloc(sizeof(VarNode));
	piVarNode->name = "pi";
	piVarNode->id = 0;
	piVarNode->next = NULL;

	VarNode* tauVarNode = (VarNode*)malloc(sizeof(VarNode));
	tauVarNode->name = "tau";
	tauVarNode->id = 1;
	tauVarNode->next = piVarNode;

	VarNode* eVarNode = (VarNode*)malloc(sizeof(VarNode));
	eVarNode->name = "e";
	eVarNode->id = 2;
	eVarNode->next = tauVarNode;

	VarNode* tVarNode = (VarNode*)malloc(sizeof(VarNode));
	tVarNode->name = "t";
	tVarNode->id = 3;
	tVarNode->next = eVarNode;

	VarNode* atVarNode = (VarNode*)malloc(sizeof(VarNode));
	atVarNode->name = "at";
	atVarNode->id = 4;
	atVarNode->next = tVarNode;

	VarNode* xVarNode = (VarNode*)malloc(sizeof(VarNode));
	xVarNode->name = "x";
	xVarNode->id = 5;
	xVarNode->next = atVarNode;

	VarNode* yVarNode = (VarNode*)malloc(sizeof(VarNode));
	yVarNode->name = "y";
	yVarNode->id = 6;
	yVarNode->next = xVarNode;

	char* preResult = "";
	char* preFuncResult = "";
	int funcID = 1;
	getNodeString(parseResult[1], &preResult, &preFuncResult, 0, &funcID, yVarNode, error);
	
	char* result = malloc(sizeof(char) * (strlen(preResult) + 1));
	memset(result, 0, sizeof(char) * (strlen(preResult) + 1));
	strcat(result, preResult);

	char* funcResult = malloc(sizeof(char) * (strlen(preFuncResult) + 1));
	memset(funcResult, 0, sizeof(char) * (strlen(preFuncResult) + 1));
	strcat(funcResult, preFuncResult);

	*body = result;
	*funcs = funcResult;

	if (*error != NULL) {
		char* preError = *error;
		*error = malloc(sizeof(char) * (strlen(preError) + 1));
		memset(*error, 0, sizeof(char) * (strlen(preError) + 1));
		strcat(*error, preError);
	}

	cleanUp();
}