This is a copy of README.md from our github, recommanded you go to the website below : 
https://github.com/LanKuDot/CMinusMinus_Compiler

The limited compiler for C language.
------
This is a litimed compiler for C language, we only consider "int"(exclude array) and some basic logical statement like if else while, this compiler cannot deal with complicated situation. 

How to use : 
-
1. ```make```
2. ```./CMinusMinus```

Steps of the compiler : 
-

###lexer
- parsing the input file (EX. main.c), determine the types of all symbols in it. 

###Syntax_Analyzer (use LL(0) parsing)
- check if there is any syntax error in the input file (by grammar.txt).
- Steps of Syntax_Analyzer
- 1. find the nullable, first, and follow sets of each symbol
- 2. create LLTable
- 3. create parse tree 
- If you got an error in the console, you can go check the Set.txt, Table.txt, Parse_Tree.txt,
- and go check anything wrong.

###Semantic_Analyzer
- We didn't create semantic rule here(Don't know how), and we check invalid variable only. (replicated declared)
- The Semantic Analyzer only created symbol table here, the table will show scope, type, token, of each identifier.
- If you want to check the Semantic Analyzer is right or not, you can check the Symbol_Table.txt

###Quadruple
-

###Tiny Machine
-


## Vaild tokens
- **Keywords**: int char return if else while break
- **Operators**: = ! + - * / == != < > <= >= && ||
- **Special Symbols**: [ ] { } ( ) ; ,
- **Identifier**: [A-Za-z\_][A-Za-z0-9\_]\*
- **Number**: [0-9]+
- **Char**: '[.|\\n|\\t| ]'
- **Comment**: //

## Coding Style
The input file(Ex. main.c) must contain at least one space between two tokens.

## Grammar
The grammar can be switched to anything else, but note that all the indention in the grammar should be "space", do not use "tab".

