#include "lexer.h"
#include "Syntax_Analyzer.h"

set<string> terminal;
set<string> nonterminal;
GRAMMAR grammar;
map<LHS, bool> nullTable;
map<LHS, set<string> > firstTable;
map<LHS, set<string> > followTable;
map<LHS, map<string, vector<string> > > LLTable;

/* All token in the input file */
vector<Token_info> tokenList;

/*
 *  Function : readFile 
 * Parameter : const char * fileName
 *	    Desc : read the grammar file and parse it into
 *             the data structure - grammar.
 */
void readFile(const char * fileName) {
	ifstream fin (fileName);
	string line;
	int index = 0;	
	LHS lhs;
	RHS rhs;

	if (fin.is_open()) {
		while (1) {
			if(!getline(fin, line)) break;
			if (line.at(line.length() - 1) == '\r') {
				line = line.substr(0, line.length() - 1);
			}
			if(line.at(0) != ' ') {
				if(rhs.size() != 0) {
					grammar.insert(pair<LHS, RHS>(lhs, rhs));
				}
				index = 0;
				// clear lhs and rhs for next grammar
				lhs = ""; rhs.clear();
				lhs = line;
			} else {
				istringstream iss(line);
				vector<string> tmp;
				// split line into tokens with whitespaces chars
				while (iss) {
					string token;
					
					iss >> token;
					if (token != "") {
						tmp.push_back(token); 
					}
				};
				rhs.push_back(tmp);
				index++;
			}
		}
		grammar.insert(pair<LHS, RHS>(lhs, rhs));
		fin.close();	
	} else {
		cout << "Unable to open file." << endl;
	}

	// add rule S -> Program $ in grammar
	rhs.clear();
	vector<string> tmp;
	tmp.push_back("Program");
	tmp.push_back("$");
	rhs.push_back(tmp);
	grammar.insert(pair<LHS, RHS>("S", rhs));

	return;
}

/*
 * function : createSet
 * parameter : none
 * desc : record all terminal and nonterminal in each set.
 */
void createSet() {
	for (map<LHS, RHS>::iterator it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		RHS rhs = it -> second;

		if(isNonterminal(lhs)) {
			nonterminal.insert(lhs);
		}
		for (int i = 0; i != rhs.size(); i++) {
			for (int j = 0; j != rhs[i].size(); j++) {
				if (isNonterminal(rhs[i][j])) {
					nonterminal.insert(rhs[i][j]);	
				} else {
					terminal.insert(rhs[i][j]);
				}
			}
		}
	}
}

/*
 * function : nullable
 * parameter : LHS lhs --- a terminal or nonterminal
 * desc : determine the lhs is nullable or not. 
 *        this function will return bool type
 */

bool nullable(LHS lhs) {

	if(!isNonterminal(lhs) ) {
		if(lhs == "epsilon") {
			return true;		
		} else {
			return false;	
		}
	}

	RHS rhs = grammar.find(lhs) -> second;
	// check all rules
	for (int i = 0; i != rhs.size(); i++) {
		// check all elements in the rule
		for (int j = 0; j != rhs[i].size(); j++) {
			// check if the element is nonterminal
			if(isNonterminal(rhs[i][j])) {
				if(nullable(rhs[i][j])) {
					// do nothing
				} else {
					// don't need to check the next element, go to 
					// the next rule
					break;
				}
			} else {
				if (rhs[i][j] == "epsilon") {
					return true;
				} else {
					// if the terminal is not epsilon, go to next rule
					break;
				}
			}
		}
	}
	/* now we have checked all the rules, but none of them are nullable,
	 * then return false.
	 */
	return false;
}

/*
 * function : getFirst
 * parameter : LHS lhs - a terminal or nonterminal
 * desc : return a set of FIRST(lhs)
 */

set<string> getFirst(LHS lhs) {
	RHS rhs = grammar.find(lhs)	-> second;
	set<string> list;

	/* check the table to find if we have done the LHS before 
	 * if we have, then return the set in the table instead of
	 * going ahead.
	 */
	if (firstTable[lhs].size() != 0) {
		return firstTable[lhs];
	}

	/* if lhs is epsilon , then return null set */
	if (lhs == "epsilon") {
		return list;
	}
	if (!isNonterminal(lhs) && firstTable[lhs].size() == 0) {
		firstTable[lhs].insert(lhs);
	}

	/*
	 * check if lhs is terminal, if yes, then insert lhs into
	 * list, else recursivly search all rules and insert the 
	 * terminals.
	 */
	if (!isNonterminal(lhs)) {
		list.insert(lhs);
		return list;
	} else {
		for (int i = 0; i != rhs.size(); i++) {
			for (int j = 0; j != rhs[i].size(); j++) {
				set<string> tmp = getFirst(rhs[i][j]);
				list.insert(tmp.begin(), tmp.end());
				if (!nullable(rhs[i][j]))
					break;
			}
		}
		return list;
	}

}


/*
 * function : getFollow(LHS lhs)
 * input the lhs and it will found the Follow(lhs) and fill the result
 * into the followTable
 */

void getFollow(LHS lhs) {
	//cout << "[START] getFollow : Follow(" + lhs + "): "<< endl;

	RHS rhs = grammar.find(lhs) -> second;
	set<string> tmp;

	// A -> BC
	// Rules : 
	// 1. Follow(B) contains First(C)
	// 2. Follow(B) contains Follow(C) if nullable(C) is true
	// 3. Follow(B) contains Follow(A) if nullable(C) is true
	// 4. Follow(C) contains Follow(A) 
	// 

	// if lhs is start symbol, add $ 
	//if (lhs == "Program") { followTable[lhs].insert("$"); }

	for (int i = 0; i != rhs.size(); i++) {
		for (int j = 0; j != rhs[i].size(); j++) {
			string nextToken;
			string token = rhs[i][j];  
			// check if next token exist
			if((j + 1) < rhs[i].size()) {
				nextToken = rhs[i][j + 1];
				
				// Rule 1
				if (isNonterminal(nextToken)) {
					tmp = firstTable[nextToken];
					followTable[token].insert(tmp.begin(), tmp.end());
					// cout << "1. " + token + " add First(" + nextToken + ")" << endl;
				} else {
					followTable[token].insert(nextToken);
					// cout << "1. " + token + " add " + nextToken << endl;
				}

				// Rule 2
				if (isNonterminal(token)) {
					if(nullable(nextToken)) {
						followTable[token].insert(nextToken);	
						// cout << "2. " + token + " add Follow(" + nextToken + ")" << endl;

				// Rule 3
						followTable[token].insert(lhs);
						// cout << "3. " + token + " add Follow(" + lhs + ")" << endl;
					} else {
						// cout << nextToken + " is not nullable" << endl;	
					}
				}
			} else {
				if (isNonterminal(token)) {
				// Rule 4
					followTable[token].insert(lhs);
					// cout << "4. " + token + " add Follow(" + lhs + ")" << endl;
				}
			}
		}
	}

	return;
}

/*
 * function eliminateNonterminal() :
 * must be done after getFollow() 
 * Eliminate all nonterminal in followTable
 * [NOTE]: This code is so ugly....who can help me....
 */

void eliminateNonterminal () {
	/*
	 */
	map<LHS, set<string> >:: iterator itMap;
	set<string>:: iterator itSet;

	// delete the situation : 
	// expr -> expr expr'' , the expr is duplicated, we have to delete the rhs of expr
	// to eliminate infinite loop.
	for(int i = 0; i != followTable.size(); i++) {
		for (itMap = followTable.begin(); itMap != followTable.end(); itMap++) {
			LHS lhs = itMap -> first;
			if (isNonterminal(lhs)) {
				for (itSet = followTable[lhs].begin(); itSet != followTable[lhs].end(); itSet++) {
					string token = *itSet;
					if (token == lhs) {
						followTable[lhs].erase(token);
					}
				}
			}
		}
	}

	// scanning the followTable[lhs] and if there is nonterminal
	// find followTable[nonterminal] and add the followTable[nonterminal]
	for(int i = 0; i != followTable.size(); i++) {
		for (itMap = followTable.begin(); itMap != followTable.end(); itMap++) {
			LHS lhs = itMap -> first;
			if (isNonterminal(lhs)) {
				for (itSet = followTable[lhs].begin(); itSet != followTable[lhs].end(); itSet++) {
					string token = *itSet;
					if (isNonterminal(token)) {
						set<string> tmp = followTable[token];
						followTable[lhs].insert(tmp.begin(), tmp.end());
					}
				}
			}
		}
	}	

	// delete all nonterminal
	
	for(int i = 0; i != followTable.size(); i++) {
		for (itMap = followTable.begin(); itMap != followTable.end(); itMap++) {
			LHS lhs = itMap -> first;
			if (isNonterminal(lhs)) {
				for (itSet = followTable[lhs].begin(); itSet != followTable[lhs].end(); itSet++) {
					string token = *itSet;
					if (isNonterminal(token)) {
						followTable[lhs].erase(token);
					}
				}
			}
		}
	}

	return ;
}

/*
 * function : createLLTable
 * parameter : none
 * desc : according to the rule of LL
 *        fill in the LLTable
 */
void createLLTable() {
	for (map<LHS, RHS>::iterator it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		RHS rhs = it -> second;
		set<string>firstOfLHS = firstTable[lhs];

		// all input will be terminal, so we find all productions that will create that input
		// in the terminal set.
		for (set<string>::iterator input = terminal.begin(); input != terminal.end(); input++) {
			// each production
			// cout << "check " << *input << endl;
			for	(int i = 0; i != rhs.size(); i++) {
				// each token in the production
				// cout << "production : " << i << endl;
				for (int j = 0; j != rhs[i].size(); j++) {
					// Rule : FIRST(rhs[i][j]) contains input
					// ex. T' -> T$  T -> aTc and input = a, FIRST(T) contains "a"
					//	   so we can fill the table like this
					//       | input: a | .....
					//    ---|----------|-----------
					//     T'| T' -> T$ | .....
					
					if (firstTable[rhs[i][j]].find(*input) != firstTable[rhs[i][j]].end()) {
						LLTable[lhs].insert(pair<string, vector<string> >(*input, rhs[i]));
						// cout << "rule 1 - find " << *input << " insert " << "rhs[" << i << "]" << endl;
					}

					// Rule : FOLLOW(lhs) contains input , and this production can derive ""epsilon
					// then add this production.
					
					// if the last token of this is "also" nullable
					// check if the FOLLOW(lhs) contains input
					if ((j == (rhs[i].size() - 1)) && nullable(rhs[i][j])) {
						if (followTable[lhs].find(*input) != followTable[lhs].end() ) {
							// cout << "rule 2 - find" << *input << " insert "<< "rhs[" << i << "]" << endl;

							LLTable[lhs].insert(pair<string, vector<string> >(*input, rhs[i]));
						}
					}

					// if the token in this production isn't nullable or is terminal, we don't need to 
					// move to next token, everything need to check is done above.
					if(!nullable(rhs[i][j]) || !isNonterminal(rhs[i][j]))
						break;
				}
			}	
		}
	}

	return;
}

/*
 * this function is design for parser in Syntax_Analyzer to
 * record property - [tranToken] in Token_info. 
 * 
 * property : tranToken is a token in grammar.
 * ex. int x;
 * in main.c it is [int x;] undoubtedly, but in grammar.txt 
 * parser will hope the tokens above are [int id;] , it will 
 * be easily to match each token and quickly show their original 
 * meaning.
 */
void tokenTrans( vector<Token_info> & token_list ) {
	
	for (int i = 0; i != tokenList.size(); i++) {
		switch (tokenList[i].category) {
			case IDENTIFIER :
				tokenList[i].tranToken = "id";
				break;
			case NUMBER :
				tokenList[i].tranToken = "num";
				break;
			default : 
				tokenList[i].tranToken = tokenList[i].token;
				break;
		}
	}

	return;
}

/*
 * function : parser
 * parameter : vector<string> input
 *             an input vector which have been parsed py lexer
 * desc : 
 */
void parser(vector<Token_info> & tokenList, const char * fileName) {
	// trans tokens in tokenList
	tokenTrans(tokenList);

	ofstream outFile;
	outFile.open(fileName);
	ostream & output(outFile.is_open() ? outFile : cout);

	cout << "[Start] : parser" << endl;

	stack<string> pStack;  // production stack
	stack<Token_info> iStack;  // input stack
	stack<int> oStack; // number of parse order
	int i = tokenList.size();
	int order = 0;

	Token_info endSym;
	endSym.token = "$";
	endSym.tranToken = "$";

	if (iStack.empty()) { iStack.push(endSym); }
	else {
		cout << "[Error] : ParseTree input stack isn't empty." << endl;
		return;
	}
	
	if (pStack.empty()) { pStack.push("S"); }
	else {
		cout << "[Error] : ParseTree production stack isn't empty." << endl;
		return;
	}

	// push all token of input file into iStack
	while ((--i) != -1) {
		iStack.push(tokenList[i]);
	}
	cout << "string push in iStack" << endl;

	if (oStack.empty()) { oStack.push(order); }
	while (!pStack.empty()) {
			
		// Debug : trace stack //

		/*
		stack<string> dStack;
		string debug = "";

		cout << "pStack : ";
		// debugging : for pStack
		while (!pStack.empty()) {
			cout << pStack.top() << " ";
			dStack.push(pStack.top());	
			pStack.pop();
		}
		cout << endl;
		while (!dStack.empty()) {
			pStack.push(dStack.top());	
			dStack.pop();	
		}

		// debugging : for iStack
		cout << "iStack : ";
		while (!iStack.empty()) {
			cout << iStack.top() << " ";
			dStack.push(iStack.top());	
			iStack.pop();
		}
		cout << endl;
		while (!dStack.empty()) {
			iStack.push(dStack.top());	
			dStack.pop();	
		}*/

		// if the top of the production stack is 
		// epsilon, then pop unconditionally.
		if (pStack.top() == "epsilon") {
			pStack.pop();	
			oStack.pop();
		}
		// if top of pStack is terminal then match it and pop
		// both in pStack and iStack
		if (!isNonterminal(pStack.top())) {
			if (pStack.top() == iStack.top().tranToken) {
				// print
				//cout << "[top] : " <<  setw(14) << left << iStack.top();
				for (int j = oStack.top(); j >= 1; j--) {
					output << "  ";
				}
				output << oStack.top() << " " << iStack.top().token << endl;

				pStack.pop();
				oStack.pop();	
				iStack.pop();
			} 
			// terminal not match : this may be the error from 
			// first() and follow()
			else {
				cout << "[Error] : terminal do not match for '" 
					 << pStack.top() << "' and '" 
					 << iStack.top().token << "'" << endl;
				break;
			}
		} 
		// if parser cannot find rule for the input(terminal) of nonterminal
		// print error.
		else if (LLTable[pStack.top()][iStack.top().tranToken].size() == 0){
			cout << "[Error] : no such rule for " 
				 <<"nonterminal : " << pStack.top() 
				 << " input : " << iStack.top().token << endl;
			break;
		} 
		// match the rule and replace the top element of pStack with
		// the production of the rule
		else {
			vector<string> rhs = LLTable[pStack.top()][iStack.top().tranToken];

			order = oStack.top() + 1;

			// print
			//cout << "[top] : " <<  setw(14) << left << iStack.top().token;
			for (int j = oStack.top(); j >= 1; j--) {
				output << "  ";
			}
			output << oStack.top() << " " << pStack.top() << endl;
			
			// pop the top
			pStack.pop();
			oStack.pop();
			// push the production
			for (int i = rhs.size() - 1; i != -1; i--) {
				pStack.push(rhs[i]);		
				oStack.push(order);		
			}
		}
	}

	return;	
} 

/*
 * function : isNonterminal
 * desc : determine the token is nonterminal or not
 */

bool isNonterminal(string s) {
	/* All nonterminal would start with a upper-case character. */
	if( isupper( s.at(0)) ) {
		return true;
	} else {
		return false;
	}
}


/*  print all symbol	*/
void printTermNonterminal() {
 	cout << "[terminal]" << endl;
 	for (set<string>:: iterator itSet = terminal.begin(); itSet != terminal.end(); itSet++) {
 				cout << *itSet + " ";	
 	}
 	cout << endl;
 	cout << "[nonterminal]" << endl;
 	for (set<string>:: iterator itSet = nonterminal.begin(); itSet != nonterminal.end(); itSet++) {
 			cout << *itSet + " ";	
 	}
 	cout << endl;
	return;
}

/* print grammar */
void printGrammar () {
	GRAMMAR::iterator it;
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		RHS rhs = it -> second;
		cout << "LHS : " << lhs << endl;
		for (int j = 0; j != rhs.size(); j++) {
			for (int k = 0; k != rhs[j].size(); k++) {
				cout << rhs[j][k] << " ";
			}
			cout << endl;
		}
	}
	return;
}

/* print nullTable */
void printNullable (ostream & output) {
	GRAMMAR::iterator it;
	string result;

	output << "Nullable" << endl;
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		if (nullTable[lhs] == true) 
			result = "true";
		else
			result = "false";

		output << setw(24) << left << lhs
	   	       << ":  " <<  result << endl;
	} 
	return ;
}

/* print FirstTable */
void printFirst (ostream & output) {
	/* show elements in the firstTable */

	// [NOTE] only print nonterminal elements
	// but remind that the firstTable will alse record First(terminal)
	output << "First" << endl;
	for (map<LHS, set<string> >::iterator itFirst = firstTable.begin(); itFirst != firstTable.end(); itFirst++) {
		LHS lhs = itFirst -> first;
		if (isNonterminal(lhs)) {
			output << setw(24) << left << lhs << ":  ";
		
			for (set<string>::iterator itSet = firstTable[lhs].begin(); itSet != firstTable[lhs].end(); itSet++) {
				output << *itSet + " ";	
			}
			output << endl;
		}
	}
	return;
}

/* print followTable */
void printFollow (ostream & output) {

	output << "Follow" << endl;
	for (map<LHS, set<string> >:: iterator itFirst = followTable.begin(); itFirst != followTable.end(); itFirst++) {
		LHS lhs = itFirst -> first;
		if (isNonterminal(lhs)) {
			output << setw(24) << left << lhs << ":  ";
		
			for (set<string>:: iterator itSet = followTable[lhs].begin(); itSet != followTable[lhs].end(); itSet++) {
				output << *itSet + " ";	
			}
			output << endl;
		}
	}

	return;
}

/* print nullable first follow */
void printSet (const char * fileName) {
	ofstream outFile;	
	outFile.open(fileName);

	if (!outFile) {
		cout << "[Error] : Fail to open file " << fileName  << " -> output to console." << endl;
	} 

	ostream & output(outFile.is_open() ? outFile : cout);

	printNullable(output);
	output << endl;
	output << endl;

	printFirst(output);
	output << endl;
	output << endl;

	printFollow(output);
	output << endl;
	output << endl;

	return;
}

/* print LLTable */
void printLLTable (const char * fileName) {
	ofstream outFile;
	outFile.open(fileName);
	ostream & output(outFile.is_open() ? outFile : cout);

	/* print the LLTable */
	for (map<LHS, map<string, vector<string> > >::iterator it = LLTable.begin(); it != LLTable.end(); it++) {
		LHS lhs = it -> first;
		map<string, vector<string> > tmp = it -> second;

		for(map<string, vector<string> >::iterator it_tmp = tmp.begin(); it_tmp != tmp.end(); it_tmp++) {
			string input = it_tmp -> first;
			vector<string> production = it_tmp -> second;
			output << setw(22) << left << lhs << setw(11) << input;
			for (int i = 0; i != production.size(); i++) {
				output << production[i] + " ";
			}
			output << endl;
		}
	}
	return;
}

void createNullTable() {
	GRAMMAR::iterator it;
	// create nullTable
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		if(nullable(lhs)) {
			nullTable[lhs] = true;
		}
		else {
			nullTable[lhs] = false;
		}
	} 
	return;
}

void createFirstTable() {
	GRAMMAR::iterator it;
	// create first table
	for (map<LHS, RHS>::iterator it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		RHS rhs = it -> second;

		firstTable[lhs] = getFirst(lhs);
		for (int i = 0; i != rhs.size(); i++) {
			for (int j = 0; j != rhs[i].size(); j++) {
				firstTable[rhs[i][j]] = getFirst(rhs[i][j]);
			}
		}
	}
	return;
}

void createFollowTable() {
	GRAMMAR::iterator it;
	// create follow table
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		getFollow(lhs);
	}
	eliminateNonterminal();
	return;
}

