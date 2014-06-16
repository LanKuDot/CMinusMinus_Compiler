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

bool isDeclared(int);
bool isInitialized(int);

string lookUp (int, int, bool, bool);
void insert(int, string, int, bool, bool);

void createSymbolTable();
void printTA_Symbol_Table(const char *);
void forTAOutput();

#endif /* SEMANTIC_ANZR */
