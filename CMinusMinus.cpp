#include <iostream>
#include "lexer.h"

using namespace std;

int main()
{
	vector<Token_info> token_list;

	lexial_analyzer( "main.c", &token_list );
}
