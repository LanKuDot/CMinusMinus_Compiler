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
int GlobalDecl_quadruple( FILE *fp, int topStackLevel );
int Decl_quadruple( FILE *fp, int topStackLevel );
int Expr_recv( FILE *fp, int topStackLevel );

int readinElement( FILE *fp, parseTree_Ele *node );
int getFuncDeclStackLevel( FILE *fp, const char *funcName );

/* Quadruple related function */
void setQuadrupleUnaryOp( Quadruple *target, char *op );
void setQuadrupleArg2( Quadruple *target, int tmpVar_action );
void pushQuadrupleResult( Quadruple *target );
char *getQuadrupleLastTempVar();

#define VAR_AND_NUM   1
#define ASSIGN        2
#define VAR_DECL      3

int tmpVar_used = 0;
vector< Quadruple > quadruples;
long int bytesRead = 0;	// Record the bytes read from file

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
	Quadruple quad_ele;

	/* Get the stack level of the scope of the main function */
	int mainFuncScopeStackLv = getFuncDeclStackLevel( fp, "main" );

	// Reset the file pointer head
	fseek( fp, 0, SEEK_SET );

	GlobalDecl_quadruple( fp, 1 );


	fclose( fp );

}	// end of createQuadruple()

/* Generate quadruple of global variable
 */
int GlobalDecl_quadruple( FILE *fp, int topStackLevel )
{
	parseTree_Ele node;
	Quadruple quad_ele;

	while ( readinElement( fp, &node ) != -1 )
	{
		/* Global Variable Declaration
		 * Program -> DeclHead DeclTail DeclList
		 * DeclTail -> ; | [ num ] ; | ( ParamDecList ) Block */
		if ( strcmp( node.name, "DeclHead" ) == 0 )
		{
			Decl_quadruple( fp, node.stackLevel );

			// Pop the quadruple temporarily stored into list
			Quadruple tmp;
			tmp = quadruples.back();
			quadruples.pop_back();

			// Get DeclTail and first
			readinElement( fp, &node );
			readinElement( fp, &node );

			// DeclTail -> ; | [ num ] ; |
			//             ( ParamDecList ) Block
			switch ( node.name[0] )
			{
				// Variable declaration
				case ';':
					strcpy( tmp.op, "VARDECL" );
					quadruples.push_back( tmp );
					break;

				// Array declaration
				case '[':
					strcpy( tmp.op, "ARRAYDE" );

					// Get array length
					readinElement( fp, &node );
					strcpy( tmp.arg2,  node.name );

					quadruples.push_back( tmp );
					break;

				// Function declaration
				default:
					break;
			}	// end of switch( node.name[0] )
		}
	}	// end of while( readinElement )

	return VAR_DECL;

}	// end of GlobalDecl_quadruple()

/* Generate quadruple for variable declaration
 * DeclHead -> Type id */
int Decl_quadruple( FILE *fp, int topStackLevel )
{
	parseTree_Ele node;
	Quadruple quad_ele;

	// Initialize the quadruple
	memset( &quad_ele, 0, sizeof( Quadruple ) );

	// Get Type and type name
	readinElement( fp, &node );
	readinElement( fp, &node );
	strcpy( quad_ele.arg1, node.name );

	// Get the name of variable
	readinElement( fp, &node );
	strcpy( quad_ele.result, node.name );

	// Push the quadruple temporarily
	quadruples.push_back( quad_ele );

	return VAR_DECL;
}	// end of Decl_quadruple()

/* Generate quadruple recursively for expression.
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

		/* Expr -> Primary ExprB */
		if ( strcmp( node.name, "Primary" ) == 0 )
		{
			/* Primary -> num |
			 *            ( Expr )
			 */
			readinElement( fp, &node );
			if ( strcmp( node.name, "(" ) != 0 )
			{
				// Temporarily store number to arg1
				strcpy( quad_ele.arg1, node.name );
			}
			else
			{
				// Expr -> Primary Expr" -> ( Expr ) Expr"
				readinElement( fp, &node );
			}
		}
		/* Expr -> id ExprC ExprB */
		else if ( strcmp( node.name, "ExprC" ) == 0 )
		{
			strcpy( quad_ele.arg1, last_node.name );
		}
		/* Expr -> UnaryOp Expr Expr" */
		else if ( strcmp( node.name, "UnaryOp" ) == 0 )
		{
			/* UnaryOp -> - | ! */
			readinElement( fp, &node );
			setQuadrupleUnaryOp( &quad_ele, node.name );
		}
		/* Array
		 * Expr -*> ... ExprD ...
		 * ExprD -> [ Expr ] ExprE | esplion
		 * ExprE -> = Expr | esplion
		 *
		 * If ExprE products "= Expr", array element is lvalue.
		 * If ExprE products "esplion", array element is rvalue.
		 */
		else if ( strcmp( node.name, "ExprE" ) == 0 )
		{
			long int lastBytesRead = bytesRead;
			// Pop the last quadruples to assign opcode
			Quadruple tmp = quadruples.back();
			quadruples.pop_back();

			// Read 1 parse tree element ahead
			readinElement( fp, &node );

			if ( strcmp( node.name, "=" ) == 0 )
				strcpy( tmp.op, "ARRAYAD" );
			else
				strcpy( tmp.op, "ARRAYVA" );

			// Push back to the quadruples
			quadruples.push_back( tmp );
 
			// Reset to the last node read
			fseek( fp, lastBytesRead, SEEK_SET );
			bytesRead = lastBytesRead;
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
#define BUFFER_LEN  256

int readinElement( FILE *fp, parseTree_Ele *node )
{
	char buffer[BUFFER_LEN+1], *token;

	if ( fgets( buffer, BUFFER_LEN, fp ) != NULL )
	{
		bytesRead += strlen( buffer );

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
 * begin of the function scope is that of funcion id plus 1.
 * Here is the garmmar (the stack level):
 * Program -> DeclHead DecTail DeclList (x)
 * DeclHead (x) -> Type id (x+1) - Here is function id
 * DeclTail (x) -> ( ParamDeclList ) Block (x+1)
 * Block (x+1) -> { ... } (x+2) - The scope of func id starts at here.
 *
 * - *fp: [in] The file pointer to the parse tree file
 * - *funcName: [in] The identifier of function
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
			if ( node.stackLevel - declStackLevel == 1 )
			{
				return node.stackLevel;
			}
			else break;
		}
	}

	return -1;

}	// end of getFuncDeclStackLevel()
