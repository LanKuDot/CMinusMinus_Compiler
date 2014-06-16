/* Generate TAC and Quadruples from parse tree. */
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "MachineCodeGenerator.h"
#include "config.h"

typedef struct parseTree_element {
	int stackLevel;
	char name[64];
} parseTree_Ele;

void createQuadruple();
int readinElement( FILE *fp, parseTree_Ele *node );
int getFuncDeclStackLevel( FILE *fp, const char *funcName );

int mainFuncScopeStackLv;

void machineCodeGenerator()
{
	/* Generate quadruples */
	createQuadruple();

}	// end of machineCodeGenerator()

void createQuadruple()
{
	FILE *fp = fopen( PARSE_TREE_FILE, "r" );
	parseTree_Ele node;

	/* Get the stack level of the scope of the main function */
	mainFuncScopeStackLv = getFuncDeclStackLevel( fp, "main" );

	/* Only expression( Expr ) needs to generate quadruple. */
	while ( readinElement( fp, &node ) != -1 )
	{
		if ( strcmp( node.name, "Expr" ) == 0 )
			printf( "%2d, %s\n", node.stackLevel, node.name );
	}

	fclose( fp );

}	// end of createQuadruple()

/* Read in an element from parse tree each time called
 * and store in the "node".
 * - *fp: [in] The pointer to the file
 * - *node: [out] The node read from parse tree.
 *   If there is nothing to read from the parse tree,
 *   node.stackLevel = -1;
 *
 * Return 1 if got an element successfully, otherwise,
 * return -1.
 */
int readinElement( FILE *fp, parseTree_Ele *node )
{
	char buffer[64], *token;

	if ( fgets( buffer, 64, fp ) != NULL )
	{
		/* Parse space and noew line character. */
		token = strtok( buffer, " \n" );
		/* Get stack level */
		(*node).stackLevel = atoi( token );
		/* Get the name of node */
		token = strtok( NULL, " \n" );
		strcpy( (*node).name, token );

		return 1;
	}
	else	/* nothing to read */
	{
		(*node).stackLevel = -1;

		return -1;
	}

}	// end of readinElement()

/* Get the stack level of begin of scope of the function.
 * According to the modified grammar, the stack level of
 * begin of the function scope is that of funcion id plus 2.
 * Here is the garmmar (the stack level):
 * DeclHead -> Type id DeclTail (x)
 * DeclTail (x) -> ( ParamDeclList ) Block DeclHead (x+1)
 * Block (x+1) -> { ... } (x+2) - The scope of func id starts at here.
 *
 * - *fp: [in] The file pointer to the parse tree file
 *
 * Return the stack level of the begin of the scope of the
 * function. or return -1, if not found.
 */
int getFuncDeclStackLevel( FILE *fp, const char *funcName )
{
	parseTree_Ele node;
	int declStackLevel;

	/* Get the stack level of the id of the function */
	while ( readinElement( fp, &node ) != -1 )
	{
		if ( strcmp( node.name, funcName ) == 0 )
		{
			declStackLevel = node.stackLevel;
			break;
		}
	}

	/* Get the stack level of next symbol "{". */
	while ( readinElement( fp, &node ) != -1 )
	{
		if ( strcmp( node.name, "{" ) == 0 )
		{
			if ( node.stackLevel - declStackLevel == 2 )
			{
				return node.stackLevel;
			}
			else break;
		}
	}

	return -1;

}	// end of getFuncDeclStackLevel()
