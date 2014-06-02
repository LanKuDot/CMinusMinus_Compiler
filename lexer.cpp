#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "lexer.h"

using namespace std;

/* The string of Category */
char category_str[CATEGORY_COUNT][16] = {
	"<Keyword>",
	"<Operator>",
	"<Special>",
	"<Identifier>",
	"<Char>",
	"<Number>",
	"<Comment>",
	"<Error>"
};


/* Analyzing states
 * input : the token to be analyzing.
 * now   : the now-th character is the next char to analyze. */
enum Category start( char *input, int now );
enum Category number( char *input, int now );
enum Category character( char *input, int now );
enum Category identifier( char *input, int now );

/*
 * Open and read the source code from "source_file", and
 * store the information of tokens into "token_list".
 * Do lexial analyzing and generate a token table which
 * would be saved as a file - "token.txt".
 * If the file doesn't exist, the funtcion will return -1.
 * Otherwise, return 0.
 */
int lexial_analyzer( const char *source_file, vector<Token_info> *token_list )
{
	FILE *fp, *output_fp;
	int line_number = 1;
	char input_buf[128], output_buf[32], *token;
	Token_info token_info;

	/* Open the file */
	output_fp = fopen( OUTPUT_FILE_NAME, "w" );
	fp = fopen( source_file, "r" );
	if ( fp == NULL )	// File dose not existing
		return -1;

	/* Do lexial analyzing and output the table */
	// Read one line at a time
	while( fgets( input_buf, sizeof( input_buf ), fp ) != NULL )
	{
		// Line number
		fprintf( output_fp, "Line %2d :\n", line_number++ );

		// Get the first token
		token = strtok( input_buf, " \n\t\r" );

		// Analyze the category of the input token
		while ( token != NULL )
		{
			token_info.category = start( token, 0 );

			// Get a comment token then ignore the remain
			// input in that line.
			if ( token_info.category == COMMENT )
				break;

			// Push the analyzing result into the token list.
			token_info.token.assign( token );
			token_info.lineNumber = line_number;
			(*token_list).push_back( token_info );

			fprintf( output_fp, "        %-12s : %s\n", \
					category_str[token_info.category], token );

			token = strtok( NULL, " \n\t\r" );
		}
	}

	/* Close the file */
	fclose( fp );
	fclose( output_fp );

	return 0;
}	// end of lexial_analyzer()

enum Category start( char *input, int now )
{
	switch ( input[now] )
	{
		case 'a' ... 'z' :
		case 'A' ... 'Z' :
		case '_' :
			return identifier( input, ++now );

		/* Got a number: goto state Number */
		case '0' ... '9':
			return number( input, ++now );

		/* Got left ': goto state Char */
		case '\'' :
			return character( input, ++now );

		/* Special symbols */
		case '[' : case ']' :
		case '(' : case ')' :
		case '{' : case '}' :
		case ';' : case ',' :
			// If there is one more characters behind
			// the token, this token is invaild.
			if ( strlen( input ) != 1 )
				return ERROR;
			else
				return SPECIAL_SYMBOL;

		/* Plus, minus, and multiply */
		case '+' : case '-' :
		case '*' :
			if ( strlen( input ) != 1 )
				return ERROR;
			else
				return OPERATOR;

		/* Divide and comment both begin with '/' */
		case '/':
			// The token contains only '/'.
			if ( strlen( input ) == 1 )
				return OPERATOR;
			// The token begins with "//".
			else if ( input[ ++now ] == '/' )
				return COMMENT;
			else
				return ERROR;

		/* AND and OR operator */
		case '&': case '|':
			// && and || is vaild, and cannot conatin
			// other characters.
			if ( strlen( input ) < 3 &&
					( input[now] == input[now+1] ) )
				return OPERATOR;
			else
				return ERROR;

		/* Relational operators and assignment operator "=" */
		case '=': case '!':
		case '<': case '>':
			if ( strlen( input ) == 1 || ( strlen( input ) < 3 && ( input[now+1] == '=' )))
				return OPERATOR;
			else
				return ERROR;

		default:
			return ERROR;
			break;
	}
}	// end of start()

enum Category number( char *input, int now )
{
	/* At the state Number, the token must be all numbers,
	 * otherwise, the tokem is invaild. */
	while( now < strlen( input ) )
	{
		if ( input[ now ] > 47 && input[ now ] < 58 )
			++now;
		else
			return ERROR;
	}

	return NUMBER;
}	// end of number()

/* Matching Char. Regex: '[.|\\n|\\t| ]' */
enum Category character( char *input, int now )
{
	switch( input[now] )
	{
		// match characters and space
		case 'A' ... 'Z' :
		case 'a' ... 'z' :
		case ' ' :
			++now;
			break;

		// match \\n and \\t
		case '\\' :
			++now;
			if ( input[now] == 'n' || input[now] == 't' )
				++now;
			else
				return ERROR;
			break;

		default :
			return ERROR;
	}

	// match right '
	if ( input[now++] != '\'' )
		return ERROR;
	// If there are some characters behind the right '
	else if ( input[now] != '\0' )
		return ERROR;

	return CHAR;
}

enum Category identifier( char *input, int now )
{
	/* Match Keyword */
	if ( strcmp( input, "int" ) == 0 ||
			strcmp( input, "char" ) == 0 ||
			strcmp( input, "return" ) == 0 ||
			strcmp( input, "if" ) == 0 ||
			strcmp( input, "else" ) == 0 ||
			strcmp( input, "while" ) == 0 ||
			strcmp( input, "break" ) == 0 )
		return KEYWORD;

	/* Match Identifier: regex: [A-Za-z_][A-Za-z0-9_]*
	 * The first bracket is matched at start state,
	 * so turn to match the second bracket. */
	while( now < strlen( input ) )
	{
		switch ( input[now] )
		{
			case 'A' ... 'Z' :
			case 'a' ... 'z' :
			case '0' ... '9' :
			case '_' :
				++now;
				break;

			default:
				return ERROR;
		}
	}

	return IDENTIFIER;
}
