// Program : Semantic_Analyzer.cpp
// Desc : create the symbol table.
// [Warning]: 
// 1. We only check the type is char or int, other types will let this program abort.
// 2. We do not create semantic rule for this grammar.

#include "Semantic_Analyzer.h"

using namespace std;

// record the current scope, it will increment or decrement after enter
// another scope or leave the scope.
int Scope_Range = 0;

// the Symbol_Detail records the detail of the symbol and will be
// stored into Symbol_Table.
typedef struct {
	string symbol;
	string token;
	int scope;
	string type;
	bool initialized;
	bool declared; 
} Symbol_Detail;

// Symbol_Table : 
// A 2-d vector.
// Ex. Symbol_Table[0] : a vector of all identifiers in scope 0.
vector< vector<Symbol_Detail> > TA_Symbol_Table;

// this is a simplified symbol table...just for the
// foolish output !!!
vector< vector<Symbol_Detail> > Symbol_Table;

// isDeclared(bool) : check the input symbol is declared or not.
// parameter : 
// 1. i(int) : the position of the input symbol in global variable tokenList.
//
// Warning: for now, we only check the type is in char or int, other types will be ignored.
// if the symbol before the current sybmol is type, return true, else it will be other 
// things, like "keyword", "special symol" or somthing else
bool isDeclared(int i) {
	if (tokenList[i - 1].token == "int")
		return true;
	else if (tokenList[i - 1].token == "char")
		return true;
	else 
		return false;
}

// isInitialized(bool) : check the input is initialized or not.
// parameter : 
// 1. i(int) : the position of the input symbol in global variable tokenList.
//
// if the identifier after the current symbol is "=", we say that the identifier
// is initialized.

bool isInitialized(int i) {
	if (tokenList[i + 1].token == "=")
		return true;
	else
		return false;
}

// lookUp : Check the identifier is legal or not.
// parameter :
// 1. i(int) : the position of symbol in global variable tokenList.
// 2. scope(int) : the scope where the symbol in.
// 3. isDeclared(bool) : the symbol is declared or not.
// 4. isInitialized(bool) : the symbol is initialized or not.

string lookUp (int pos, int scope, bool isDeclared, bool isInitialized) {
	//cout << "[lookUp]" << endl;
	string debugToken = tokenList[pos].token + " : ";

	// cScope means check scope, in lookUp, we will check if all the identifier
	// in the cScope to determine the input identifier is legal or not.
	// If the identifier is declared , check the scope it exists, else 
	// check its parent scope to check it have been declared in parent scope.

	string type = "";
	
	//cout << debugToken << endl;
	if (isDeclared) {
		// cout << "declared" << endl;
		// because the symbol is initialized the type of 
		// this symbol must be the last symbol;
		type = tokenList[pos - 1].token;
		for (int i = 0; i != Symbol_Table[scope].size(); i++) {
			if (Symbol_Table[scope][i].declared && 
					Symbol_Table[scope][i].symbol == tokenList[pos].token) {
				cout << debugToken << " duplicate declared.";
				return "ERROR";
			}
		}
		return type;
	}

	// cout << "undeclared" << endl;
	// not declared
	for (int i = scope; i >= 0; i--) {
		for (int j = 0; j != Symbol_Table[i].size(); j++) {
			if (Symbol_Table[i][j].symbol == tokenList[pos].token) {
				return Symbol_Table[i][j].type;
			}
		}
	}
	cout << debugToken << "not declared." << endl;
	return "ERROR";


}

// insert : insert the identifier into symbol table.
// parameter :
// 1. pos(int) : the position of symbol in global variable tokenList.
// 2. type(string) : the type of the symbol.
// 3. scope(int) : the scope where the symbol in.
// 4. isDeclared(bool) : the symbol is declared or not.
// 5. isInitialized(bool) : the symbol is initialized or not.

void insert(int pos, string type, int scope, bool isDeclared, bool isInitialized) {
	//cout << "[insert]" << endl;

	Symbol_Detail symDetail;
	symDetail.symbol = tokenList[pos].token;
	symDetail.token = tokenList[pos].tranToken;
	symDetail.scope = scope;
	symDetail.type = type;
	symDetail.initialized = isInitialized;
	symDetail.declared = isDeclared;

	//cout << "[scope] : " << scope << endl;
	Symbol_Table[scope].push_back(symDetail);
	//cout << "[insert] : done" << endl;

	return;
}


void createSymbolTable() {
	cout << "[START] create Symbol_Table" << endl;

	// * check every symbol in main.c .
	// 1. symbols which are "{", will let the global variable Scope_Range plus
	// one, since this symbol start a new scope and create a vector of Symbol_Detail
	// then pushed it into the symbol_table.
	// 2. symbols which are "}", will let the global variable Scope_Range minus
	// one, since this symbol end a scope.
	// 3. oters : check it is identifer and then loopUp	in the Symbol_Table, if 
	// symbol is legal, insert it into the Symbol_Table.

	// create a vector of the current scope and it will be pushed
	// into the symbol after entering another scope.
	vector<Symbol_Detail> tmp;
	Symbol_Table.push_back(tmp);

	for (int i = 0; i != tokenList.size(); i++) {
		string symbol = tokenList[i].token;
		if (tokenList[i].tranToken == "id") {
			string type = lookUp(i, Scope_Range, isDeclared(i), isInitialized(i));
			if (type != "ERROR"){
				insert(i, type, Scope_Range, isDeclared(i), isInitialized(i));
			} else {
				cout << "error: in " << tokenList[i].token << endl;
				return ;
			}
		}
		else if (tokenList[i].token == "(") {
			vector<Symbol_Detail> tmp;
			Symbol_Table.push_back(tmp);
			Scope_Range++;
		}
		else if (tokenList[i].token == "}") {
			Scope_Range--;
		}
	}
}

void printTA_Symbol_Table(const char * fileName) {
	ofstream outFile;	
	outFile.open(fileName);

	if (!outFile) {
		cout << "[Error] : Fail to open file " << fileName  << " -> output to console." << endl;
	} 

	ostream & output(outFile.is_open() ? outFile : cout);

	forTAOutput();
	output << setw(10) << left << "symbol";
	output << setw(10) << left << "token";
	output << setw(10) << left << "scope";
	output << setw(10) << left << "type";
	output << endl;
	for (int i = 0; i != TA_Symbol_Table.size(); i++) {
		for (int j = 0; j != TA_Symbol_Table[i].size(); j++) {
			output << setw(10) << left << TA_Symbol_Table[i][j].symbol;
			output << setw(10) << left << TA_Symbol_Table[i][j].token;
			output << setw(10) << left << TA_Symbol_Table[i][j].scope;
			output << setw(10) << left << TA_Symbol_Table[i][j].type;
			output << endl;
		}	
	}
}


/*
void forTAOutputCheck(Symbol_Detail iSymbol) {
	for (int i = TA_Symbol_Table.size(); i >= 0; i--) {
		for (int j = 0; j != TA_Symbol_Table[i].size(); j++) {
			if(TA_Symbol_Table[i][j].symbol == iSymbol.sybmol) {
				Symbol_Detail symDetail;
				symDetail.symbol = tokenList[pos].token;
				symDetail.token = tokenList[pos].tranToken;
				symDetail.scope = scope;
				symDetail.type = type;

				TA_Symbol_Table[i].push_back(symDetail);
			}
		}
	}
}*/

void forTAOutput() {
	vector<Symbol_Detail> forTA;
	TA_Symbol_Table.push_back(forTA);
	for (int i = 0; i != Symbol_Table.size(); i++) {
		vector<Symbol_Detail> forTA;
		for (int j = 0; j != Symbol_Table[i].size(); j++) {
			if (Symbol_Table[i][j].declared == true) {
				Symbol_Detail symDetail;
				symDetail.symbol = Symbol_Table[i][j].symbol;
				symDetail.token = Symbol_Table[i][j].token;
				symDetail.scope = Symbol_Table[i][j].scope;
				symDetail.type = Symbol_Table[i][j].type;

				TA_Symbol_Table[i].push_back(symDetail);
			}
			/*else {
				forTAOutputCheck(Symbol_Table[i][j]);
			}*/
		}
		TA_Symbol_Table.push_back(forTA);
	}
}

