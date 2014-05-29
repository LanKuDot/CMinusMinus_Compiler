#include <iostream>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

using namespace std;

/*
 * Open and read the source code from "source_file".
 * Do lexial analyzing and generate a token table which
 * would be saved as a file - "token.txt".
 */
int lexial_analyzer( const char *source_file )
{
	FILE *fp;
	int line_number = 1;
	char input_buf[128], output_buf[32], *token;
	Category category;

	/* Open the file */
	fp = fopen( source_file, "r" );
	if ( fp == NULL )	// File dose not existing
		return -1;

	/* Do lexial analyzing and output the table */
	// Read one line at a time
	while( fgets( input_buf, sizeof( input_buf ), fp ) != NULL )
	{
		// Get the first token
		token = strtok( input_buf, " \n\t\r" );

		// Analyze the category of the input token
		while ( token != NULL )
		{
			cout << token << endl;
			token = strtok( NULL, " \n\t\r" );
		}
	}

	/* Close the file */
	fclose( fp );

	return 0;
}	// end of lexial_analyzer()
