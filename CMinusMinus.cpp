#include <iostream>
#include <vector>
#include <cstring>
#include "lexer.h"
#include "Syntax_Analyzer.h"
#include "Semantic_Analyzer.h"
#include "MachineCodeGenerator.h"
#include "config.h"

using namespace std;

vector<Symbol_Detail> Symbol_Table_Element;

int main( int argc, char *argv[] )
{
	char sourceFileName[32];

	if ( argc == 1 )
	{
		cout << "Missing the source file!\n" 
			<< "Usage: ./CMinusMinus <Filename>" << endl;
		return -1;
	}
	else if ( argc > 2 )
	{
		cout << "Too many arguments!\n"
			<< "Usage: ./CMinusMinus <Filename>" << endl;
		return -2;
	}
	else
		strcpy( sourceFileName, argv[1] );

	/* lexial_analyzer */
	if ( lexial_analyzer( sourceFileName, & tokenList) == -1 )
	{
		cout << sourceFileName << " is not existing." << endl;
		return -3;
	}

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
