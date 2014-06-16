#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#define OUTPUT_FILE_NAME	"token.txt"

using namespace std;

/* The enumeration of the category of tokens */
// if there are some new data type going to be add here,
// you should also add in function getType() in Semantic_Analyzer.cpp
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

/* The information of a token */
typedef struct {
	string token;
	string tranToken;
	enum Category category;
} Token_info;

/* The string of Category */
extern char category_str[CATEGORY_COUNT][16];

int lexial_analyzer( const char *source_file, vector<Token_info> *token_list );

#endif	/* LEXER_H */
