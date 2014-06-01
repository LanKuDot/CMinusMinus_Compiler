OUTPUT_EXE = CMinusMinus
TOKEN_LIST_FILE = token.txt

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
	rm $(OUTPUT_EXE) $(TOKEN_LIST_FILE)
