#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <iomanip>
using namespace std;

typedef string LHS;
// RHS is a 2-d vector storing grammar
typedef vector< vector<string> > RHS;
typedef map<LHS, RHS> GRAMMAR;

void readFile(const char *);
void createSet();
bool isNonterminal(LHS);
bool nullable(LHS);
set<string> getFirst(LHS);
void getFollow();
void eliminateNonterminal();
void getLLTable();

set<string> terminal;
set<string> nonterminal;

GRAMMAR grammar;
map<LHS, set<string> > firstTable;
map<LHS, set<string> > followTable;
map<LHS, map<string, vector<string> > > LLTable;


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
					/*
					cout << "check tokeN " << endl;
					while(iss.good())
					{
						cout << (int)iss.get() << " ";	
					}
					*/
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

	
	rhs.clear();
	vector<string> tmp;
	tmp.push_back("Program");
	tmp.push_back("$");
	rhs.push_back(tmp);
	grammar.insert(pair<LHS, RHS>("S", rhs));

	return;
}

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

/*  print all symbol	*/
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

}

bool isNonterminal(string s) {
	/* All nonterminal would start with a upper-case character. */
	if( isupper( s.at(0)) ) {
		return true;
	} else {
		return false;
	}
}

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


/**************************************************************************
 * function : getFollow(LHS lhs)
 * input the lhs and it will found the Follow(lhs) and fill the result
 * into the followTable
 **************************************************************************/
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
	if (lhs == "Program") { followTable[lhs].insert("$"); }

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
}

/******************************************************
 * function eliminateNonterminal() :
 * must be done after getFollow() 
 * Eliminate all nonterminal in followTable
 * [NOTE]: This code is so ugly....who can help me....
 ******************************************************/
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

void getLLTable() {
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

	
	/* print the LLTable */
//	for (map<LHS, map<string, vector<string> > >::iterator it = LLTable.begin(); it != LLTable.end(); it++) {
//		LHS lhs = it -> first;
//		map<string, vector<string> > tmp = it -> second;
//
//		for(map<string, vector<string> >::iterator it_tmp = tmp.begin(); it_tmp != tmp.end(); it_tmp++) {
//			string input = it_tmp -> first;
//			vector<string> production = it_tmp -> second;
//			cout << setw(22) << left << lhs << setw(11) << input;
//			for (int i = 0; i != production.size(); i++) {
//				cout << production[i] + " ";
//			}
//			cout << endl;
//		}
//	}
	
}

int main() {

	GRAMMAR::iterator it;
	const char * fileName = "grammar.txt";

	readFile(fileName);
	// create terminal and nonterminal set
	createSet();

	/*	
	for (map<LHS, RHS>::iterator it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		RHS rhs = it -> second;
		cout << "LHS : " << lhs << endl;
		for (int j = 0; j != rhs.size(); j++) {
			for (int k = 0; k != rhs[j].size(); k++) {
				cout << rhs[j][k] << " ";
			}
			cout << endl;
		}
	}*/

	/* check nullable works */
	
	/*
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		if(lhs == "FunDecl")
		if(nullable(lhs)) {
			cout << lhs << " is nullable." << endl;
		}
		else {
			cout << lhs << " isn't nullable" << endl;	
		}
	} 
	*/

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


	/* show all elements in the firstTable */
	cout << "[First]" << endl;
	cout << "=============================" << endl;
	for (map<LHS, set<string> >::iterator itFirst = firstTable.begin(); itFirst != firstTable.end(); itFirst++) {
		LHS lhs = itFirst -> first;
	//	if (isNonterminal(lhs)) {
			cout << lhs + ": ";
		
			for (set<string>::iterator itSet = firstTable[lhs].begin(); itSet != firstTable[lhs].end(); itSet++) {
				cout << *itSet + " ";	
			}
			cout << endl;
	//	}
	}

	
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		getFollow(lhs);
	}
	eliminateNonterminal();

	cout << "[Follow]" << endl;
	cout << "=============================" << endl;
	for (map<LHS, set<string> >:: iterator itFirst = followTable.begin(); itFirst != followTable.end(); itFirst++) {
		LHS lhs = itFirst -> first;
		if (isNonterminal(lhs)) {
			cout << lhs + ": ";
		
			for (set<string>:: iterator itSet = followTable[lhs].begin(); itSet != followTable[lhs].end(); itSet++) {
				cout << *itSet + " ";	
			}
			cout << endl;
		}
	}
	return 0;	
}
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

/*  print all symbol	*/
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

}

