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

void machineCodeGenerator()
{
	/* Generate quadruples */
	createQuadruple();

}	// end of machineCodeGenerator()

void createQuadruple()
{
	/* Read in parse tree */
	FILE *fp = fopen( PARSE_TREE_FILE, "r" );
	parseTree_Ele node;

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
