#include <iostream>
#include <stdio.h>
#include <string.h>
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

/*
 * Open and read the source code from "source_file".
 * Do lexial analyzing and generate a token table which
 * would be saved as a file - "token.txt".
 */
int lexial_analyzer( const char *source_file )
{
	FILE *fp, *output_fp;
	int line_number = 1;
	char input_buf[128], output_buf[32], *token;
	Category category;

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
			category = start( token, 0 );

			fprintf( output_fp, "        %-12s : %s\n", \
					category_str[category], token );

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
	/* Numbers: goto state Number
	 * ASCII code: '0' is 48, '9' is 57. */
	if ( input[ now ] > 47 && input[ now ] < 58 )
		return number( input, ++now );
	else
		return ERROR;
}	// end of start()

enum Category number( char *input, int now )
{
	while( now < strlen( input ) )
	{
		if ( input[ now ] > 47 && input[ now ] < 58 )
			++now;
		else
			return ERROR;
	}

	return NUMBER;
}	// end of number()
