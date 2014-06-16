#include <iostream>
#include "lexer.h"
#include "Syntax_Analyzer.h"
#include "config.h"

using namespace std;

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

	printTermNonterminal();
	printSet(SET_FILE);
	printLLTable(LLTABLE_FILE);

	return 0;	
}
