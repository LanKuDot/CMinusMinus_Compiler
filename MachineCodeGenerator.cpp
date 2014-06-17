/* Generate TAC and quadruples from parse tree. */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

#include "MachineCodeGenerator.h"
#include "simple_itoa.h"
#include "config.h"

using namespace std;

typedef struct parseTree_element {
	int stackLevel;
	char name[64];
} parseTree_Ele;

typedef struct Quadruple_element {
	char op[8], arg1[8], arg2[8], result[8];
} Quadruple;

/* Basic Function */
void createQuadruple();
void outputQuadruple();

/* Quadruple generating function. */
int Expr_recv( FILE *fp, int topStackLevel );

int readinElement( FILE *fp, parseTree_Ele *node );
int getFuncDeclStackLevel( FILE *fp, const char *funcName );

/* Quadruple related function */
void setQuadrupleUnaryOp( Quadruple *target, char *op );
void setQuadrupleArg2( Quadruple *target, int tmpVar_action );
void pushQuadrupleResult( Quadruple *target );
char *getQuadrupleLastTempVar();

vector< Quadruple > quadruples;

void machineCodeGenerator()
{
	/* Generate quadruples */
	createQuadruple();
	outputQuadruple();

}	// end of machineCodeGenerator()


void outputQuadruple()
{
	FILE *output_fp = fopen( QUADRUPLE_OUT_FILE, "w" );
	int i;

	for ( i = 0; i < quadruples.size(); ++i )
	{
		fprintf( output_fp, "%-8s%-8s%-8s%-8s\n", quadruples.at(i).op,
				quadruples.at(i).arg1, quadruples.at(i).arg2,
				quadruples.at(i).result );
	}

	fclose( output_fp );

}	// end of outputQuadruple()

void createQuadruple()
{
	FILE *fp = fopen( PARSE_TREE_FILE, "r" );
	parseTree_Ele node;

	/* Get the stack level of the scope of the main function */
	int mainFuncScopeStackLv = getFuncDeclStackLevel( fp, "main" );

	/* Only expression( Expr ) needs to generate quadruple. */
	while ( readinElement( fp, &node ) != -1 )
	{
		if ( strcmp( node.name, "Expr" ) == 0 )
		{
			Expr_recv( fp, mainFuncScopeStackLv );
			/* Only main scope needs to generate TAC */
			break;
		}
	}

	fclose( fp );

}	// end of createQuadruple()

#define VAR_AND_NUM   1
#define ASSIGN        2

int tmpVar_used = 0;

/* Genetare quadruple recursively for expression.
 * - *fp: [in] The file pointer of parse tree
 * - topStackLevel: [in] The stack level of the nonterminal
 *   Expr in the parse tree.
 */
int Expr_recv( FILE *fp, int topStackLevel )
{
	int tmpVar_action;		// Get tmpVar_state returned from function itself.
	int tmpVar_state = VAR_AND_NUM;	// Assume that Expr contains only a variable or number.
	parseTree_Ele node, last_node;
	Quadruple quad_ele;

	/* Initailize the quadruple */
	memset( &quad_ele, 0, sizeof( Quadruple ) );

	while( readinElement( fp, &node ) != -1 )
	{
		// Only Parse the elements below the stack level of Expr
		if ( node.stackLevel == topStackLevel ||
				node.stackLevel < topStackLevel )
			break;

		/* Expr -> Primary Expr" */
		if ( strcmp( node.name, "Primary" ) == 0 )
		{
			/* Primary -> id Primary' |
			 *            num |
			 *            ( Expr )
			 */
			readinElement( fp, &node );
			if ( strcmp( node.name, "(" ) != 0 )
			{
				// Temporarily store the id or number to arg1
				strcpy( quad_ele.arg1, node.name );
			}
			else
			{
				// Expr -> Primary Expr" -> ( Expr ) Expr"
				readinElement( fp, &node );
			}
		}
		/* Expr -> UnaryOp Expr Expr" */
		else if ( strcmp( node.name, "UnaryOp" ) == 0 )
		{
			/* UnaryOp -> - | ! */
			readinElement( fp, &node );
			setQuadrupleUnaryOp( &quad_ele, node.name );
		}
		/* Array
		 * Quadruple format:
		 * - ARRAYAD id index result: get the address of the index of the array.
		 * - ARRAYVA id index result: get the value of the index of the array.
		 */
		else if ( strcmp( node.name, "[" ) == 0 )
		{
			// Expr' -> [ Expr ] = Expr
			if ( strcmp( last_node.name, "Expr'" ) == 0 )
				strcpy( quad_ele.op, "ARRAYAD" );
			// Primary' -> [ Expr ]
			else
				strcpy( quad_ele.op, "ARRAYVA" );
		}
		/* Assign
		 * Expr' -> = Expr |
		 *          [ Expr ] = Expr
		 */
		else if ( strcmp( node.name, "=" ) == 0 )
		{
			// Set the op
			strcpy( quad_ele.op, "=" );

			// Get nonterminal Expr and call itself
			readinElement( fp, &node );
			tmpVar_action = Expr_recv( fp, node.stackLevel );

			// Assign the result
			setQuadrupleArg2( &quad_ele, tmpVar_action );

			// Push to the quadruple list
			quadruples.push_back( quad_ele );

			memset( &quad_ele, 0, sizeof( Quadruple ) );

			tmpVar_state = ASSIGN;
		}
		/* BinOp
		 * Expr" -> BinOp Expr Expr" | esplion */
		else if ( strcmp( node.name, "BinOp" ) == 0 )
		{
			// Get the BinOp and set the quadruple.op
			readinElement( fp, &node );
			strcpy( quad_ele.op, node.name );
		}
		/* Expr: call the function recursively. */
		else if ( strcmp( node.name, "Expr" ) == 0 )
		{
			// the function calls itself
			tmpVar_action = Expr_recv( fp, node.stackLevel );

			// Assignment would not generate new temporary variable,
			// so don't need to assign the arg2.
			if ( tmpVar_action == ASSIGN )
				continue;

			// Set the arg2 of quadruple
			setQuadrupleArg2( &quad_ele, tmpVar_action );

			// Store and push the quadruple
			pushQuadrupleResult( &quad_ele );

			// The quadruple only contains the result of "BinOp"
			tmpVar_state = VAR_AND_NUM;
		}

		// Update the last node
		last_node = node;
	}

	if ( tmpVar_state != ASSIGN )
		quadruples.push_back( quad_ele );

	return tmpVar_state;

}	// end of Expr_recv()

void setQuadrupleUnaryOp( Quadruple *target, char *op )
{
	if ( op[0] == '!' )
		strcpy( (*target).op, "!" );
	else
		strcpy( (*target).op, "MINUS" );

}	// end of setQuadrupleUnaryOp()

/* Setting the arg2 of the quadruple accroding to the tmpVar_action.
 * - *target: [in/out] the quadruple to set
 * - tmpVar_action: [in] VAR_AND_NUM or the id of temporary variable
 */
void setQuadrupleArg2( Quadruple *target, int tmpVar_action )
{
	Quadruple tmp;

	switch ( tmpVar_action )
	{
		case VAR_AND_NUM:
			/* If the Expr just conatin a number or a variable,
			 * pop the invaild quadruple and get the number or
			 * the variable.
			 */
			tmp = quadruples.back();
			quadruples.pop_back();
			break;

		default:
			/* The temporary variable stores the result of child Expr.
			 * And the id of temporary is tmpVar_action.
			 */
			strcpy( tmp.arg1, "t" );
			strcat( tmp.arg1, simple_itoa( tmpVar_action ) );
			break;
	}

	// UnaryOp set at Arg1
	if ( strcmp( (*target).op, "MINUS" ) == 0 ||
			strcmp( (*target).op, "!" ) == 0 )
	{
		strcpy( (*target).arg1, tmp.arg1 );
	}
	else if ( strcmp( (*target).op, "=" )  == 0 )
	{
		// Move the assign target to result
		strcpy( (*target).result, (*target).arg1 );
		strcpy( (*target).arg1, tmp.arg1 );
	}
	else
		strcpy( (*target).arg2, tmp.arg1 );

}	// end of setQuadrupleArg2();

/* Store the result of quadruple to a new temporary variable,
 * and push to the quadruple list. After pushing into the list,
 * the quadruple sent would be cleared, and the temporary variable
 * would be put at arg1.
 *
 * - *target: [in/out] the quadruple.
 */
void pushQuadrupleResult( Quadruple *target )
{
	strcpy( (*target).result, "t" );
	strcat( (*target).result, simple_itoa( ++tmpVar_used ) );

	quadruples.push_back( *target );

	memset( target, 0, sizeof( Quadruple ) );
	strcpy( (*target).arg1, getQuadrupleLastTempVar() );

}	// end of pushQuadrupleResult()

/* Get the last used temproary variable of quadruple.
 * Return by the char array format "t#"
 */
char *getQuadrupleLastTempVar()
{
	static char tmp[8];

	strcpy( tmp, "t" );
	strcat( tmp, simple_itoa( tmpVar_used ) );
	strcat( tmp, "\0" );
}

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
	char buffer[128], *token;

	if ( fgets( buffer, 128, fp ) != NULL )
	{
		// Parse space and new line character.
		token = strtok( buffer, " \n\t" );
		// Get stack level
		(*node).stackLevel = atoi( token );
		// Get the name of node
		token = strtok( NULL, " \n\t" );
		strcpy( (*node).name, token );

		return 1;
	}
	else	// nothing to read
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

	// Get the stack level of the id of the function
	while ( readinElement( fp, &node ) != -1 )
	{
		if ( strcmp( node.name, funcName ) == 0 )
		{
			declStackLevel = node.stackLevel;
			break;
		}
	}

	// Get the stack level of next symbol "{".
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
