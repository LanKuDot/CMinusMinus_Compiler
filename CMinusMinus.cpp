#include <iostream>
#include <vector>
#include "lexer.h"
#include "Syntax_Analyzer.h"
#include "Semantic_Analyzer.h"
#include "MachineCodeGenerator.h"
#include "config.h"

/* The marcos of related files */
#define SOURCE_CODE_FILE	"main.c"
#define GRAMMAR_FILE		"grammar.txt"
#define PARSE_TREE_FILE		"Parse_Tree.txt"
#define SET_FILE			"Set.txt"
#define LLTABLE_FILE		"Table.txt"
#define SYMBOL_TABLE_FILE	"Symbol_Table.txt"

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

	printTermNonterminal();
	printSet(SET_FILE);
	printLLTable(LLTABLE_FILE);

	createSymbolTable(SYMBOL_TABLE_FILE);

	machineCodeGenerator();

	return 0;	
}
