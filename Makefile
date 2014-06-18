COMPILER = g++
CFLAG = -g

OUTPUT_EXE = CMinusMinus

TOKEN_LIST_FILE = token.txt
SET_FILE = Set.txt
TABLE_FILE = Table.txt
PARSE_TREE_FILE = Parse_Tree.txt
SYMBOL_TABLE_FILE = Symbol_Table.txt
QUADRUPLE_OUT_FILE = quadruples.txt

SRCS = CMinusMinus.cpp \
	   lexer.cpp \
	   Syntax_Analyzer.cpp \
	   Semantic_Analyzer.cpp \
	   QuadrupleGenerator.cpp \
	   MachineCodeGTR.cpp \
	   simple_itoa.cpp

All: $(SRCS)
	$(COMPILER) $(CFLAG) -o $(OUTPUT_EXE) $(SRCS)

run:
	./$(OUTPUT_EXE) main.c

dbg:
	gdb $(OUTPUT_EXE)

clean:
	$(RM) $(OUTPUT_EXE) $(TOKEN_LIST_FILE) $(SET_FILE) $(TABLE_FILE) $(PARSE_TREE_FILE) \
		$(SYMBOL_TABLE_FILE) $(QUADRUPLE_OUT_FILE)
