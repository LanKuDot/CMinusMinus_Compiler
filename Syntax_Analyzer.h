#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <iomanip>
#include <stack>
#include "lexer.h"

using namespace std;

typedef string LHS;
// RHS is a 2-d vector storing grammar
typedef vector< vector<string> > RHS;
typedef map<LHS, RHS> GRAMMAR;

void readFile(const char *);
bool nullable(LHS);
set<string> getFirst(LHS);
void getFollow();
void eliminateNonterminal();
void tokenTrans(vector<Token_info> &);
void parser(vector<Token_info> &, const char *);

void createSet();
void createNullTable();
void createFirstTable();
void createFollowTable();
void createLLTable();

void printTermNonterminal();
void printNullable(ostream &);
void printFirst(ostream &);
void printFollow(ostream &);
void printLLTable(const char *);
void printGrammar();
void printSet(const char *);
bool isNonterminal(string);

/* All token in the input file */

extern vector<Token_info> tokenList;

#endif	/* SYNTAX_ANALYZER_H */
