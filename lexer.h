#ifndef LEXER_H
#define LEXER_H

#define OUTPUT_FILE_NAME	"token.txt"

/* The enumeration of the category of tokens */
enum Category {
	KEYWORD = 0,
	OPERATOR,
	SPECIAL_SYMBOL,
	IDENTIFIER,
	CHAR,
	NUMBER,
	COMMENT,
	ERROR,
	CATEGORY_COUNT
};

/* The string of Category */
extern char category_str[CATEGORY_COUNT][16];

int lexial_analyzer( const char *source_file );

#endif	/* LEXER_H */
