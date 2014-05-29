#ifndef LEXER_H
#define LEXER_H

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

int lexial_analyzer( const char *source_file );

#endif	/* LEXER_H */
