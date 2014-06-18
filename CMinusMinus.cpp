#include <iostream>
#include <vector>
#include "lexer.h"
#include "Syntax_Analyzer.h"
#include "Semantic_Analyzer.h"
#include "MachineCodeGenerator.h"
#include "config.h"

using namespace std;

vector<Symbol_Detail> Symbol_Table_Element;

int main()
{
	/* lexial_analyzer */
	lexial_analyzer( SOURCE_CODE_FILE, & tokenList);

	/* syntax analyzer */
	readFile( GRAMMAR_FILE );
	// create terminal and nonterminal set
	createSet();
	createNullTable();
	createFirstTable();	
	createFollowTable();
	createLLTable();
	// create parse tree
	parser(tokenList, PARSE_TREE_FILE );

	//printTermNonterminal();
	printSet(SET_FILE);
	printLLTable(LLTABLE_FILE);

	createSymbolTable(SYMBOL_TABLE_FILE);

	machineCodeGenerator();

	return 0;	
}
