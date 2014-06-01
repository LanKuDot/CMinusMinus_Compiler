#include <iostream>
#include "lexer.h"
#include "Syntax_Analyzer.h"

using namespace std;

int main()
{
	/* lexial_analyzer */
	lexial_analyzer("main.c", & tokenList);

	/* syntax analyzer */
	readFile("grammar.txt");
	// create terminal and nonterminal set
	createSet();
	createNullTable();
	createFirstTable();	
	createFollowTable();
	createLLTable();
	// create parse tree
	parser(tokenList, "Parse_Tree.txt");

	printTermNonterminal();
	printSet("Set.txt");
	printLLTable("Table.txt");

	return 0;	
}
