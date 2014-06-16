COMPILER = g++
CFLAG = -g

OUTPUT_EXE = CMinusMinus

TOKEN_LIST_FILE = token.txt
SET_FILE = Set.txt
TABLE_FILE = Table.txt
PARSE_TREE_FILE = Parse_Tree.txt

SRCS = CMinusMinus.cpp \
	   lexer.cpp \
	   Syntax_Analyzer.cpp

<<<<<<< HEAD:makefile
All: CMinusMinus.cpp lexer.cpp Syntax_Analyzer.cpp Semantic_Analyzer.cpp
	g++ -g -o $(OUTPUT_EXE) \
		CMinusMinus.cpp \
		lexer.cpp \
		Syntax_Analyzer.cpp \
		Semantic_Analyzer.cpp
=======
All: $(SRCS)
	$(COMPILER) $(CFLAG) -o $(OUTPUT_EXE) $(SRCS)
>>>>>>> 132787d77709af02dbff78f0fc998eabbd7dc365:Makefile

run: All
	./$(OUTPUT_EXE)

dbg: All
	gdb $(OUTPUT_EXE)

clean:
	rm $(OUTPUT_EXE) $(TOKEN_LIST_FILE) $(SET_FILE) $(TABLE_FILE) $(PARSE_TREE_FILE)
