#ifndef SEMANTIC_ANZR
#define SEMANTIC_ANZR

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include "Syntax_Analyzer.h"
#include "lexer.h"
using namespace std;

// the Symbol_Detail records the detail of the symbol and will be
// stored into Symbol_Table.
typedef struct {
	string symbol;
	string token;
	bool initialized;
	bool declared; 
	int scope;
	string type;
} Symbol_Detail;

extern vector<Symbol_Detail> Symbol_Table_Element;

bool isDeclared(int);
bool isInitialized(int);

string lookUp (int, int, bool, bool);
void insert(int, string, int, bool, bool);
void createSymbolTable(const char * fileName);

#endif /* SEMANTIC_ANZR */
