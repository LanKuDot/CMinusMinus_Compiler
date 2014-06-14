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

All: $(SRCS)
	$(COMPILER) $(CFLAG) -o $(OUTPUT_EXE) $(SRCS)

run: All
	./$(OUTPUT_EXE)

dbg: All
	gdb $(OUTPUT_EXE)

clean:
	rm $(OUTPUT_EXE) $(TOKEN_LIST_FILE) $(SET_FILE) $(TABLE_FILE) $(PARSE_TREE_FILE)
