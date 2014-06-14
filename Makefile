OUTPUT_EXE = CMinusMinus
TOKEN_LIST_FILE = token.txt
SET_FILE = Set.txt
TABLE_FILE = Table.txt
PARSE_TREE_FILE = Parse_Tree.txt

All: CMinusMinus.cpp lexer.cpp Syntax_Analyzer.cpp
	g++ -g -o $(OUTPUT_EXE) \
		CMinusMinus.cpp \
		lexer.cpp \
		Syntax_Analyzer.cpp

run: All
	./$(OUTPUT_EXE)

dbg: All
	gdb $(OUTPUT_EXE)

clean:
	rm $(OUTPUT_EXE) $(TOKEN_LIST_FILE) $(SET_FILE) $(TABLE_FILE) $(PARSE_TREE_FILE)
