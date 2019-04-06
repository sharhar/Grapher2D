#pragma once

#include "utils.h"
#include <string.h>

typedef struct ParsingInfo {
	char** vars;
	char** funcs;
} ParsingInfo;

ParsingInfo* eqGetDefaultParseInfo();
void eqConvert(ParsingInfo* parseInfo, char* text, char** body, char** funcs, char** error);