#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <set>
using namespace std;

typedef string LHS;
// RHS is a 2-d vector storing grammar
typedef vector< vector<string> > RHS;
typedef map<LHS, RHS> GRAMMAR;

void readFile(const char *);
bool isNonterminal(LHS);
bool nullable(LHS);
set<string> getFirst(LHS);
void gerFollow();

GRAMMAR grammar;
map<LHS, set<string> > firstTable;
map<LHS, set<string> > followTable;

void readFile(const char * fileName) {
	ifstream fin (fileName);
	string line;
	int index = 0;	
	LHS lhs;
	RHS rhs;

	if (fin.is_open()) {
		while (1) {
			//cout << "Parsing : " << line << endl;
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
				//cout << "lhs : " << line << endl;

				lhs = line;
			} else {
				//cout << "rhs : " << endl;
				//cout << "rule" << index << " : "<< line << endl;	

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
						// cout << "SubString : " << token << endl;
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
	}
*/

	return;
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
	if (firstTable[lhs].size() != 0) 
		return firstTable[lhs];

	/* if lhs is epsilon , then return null set */
	if (lhs == "epsilon") 
		return list;

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

int main() {

	GRAMMAR::iterator it;
	const char * fileName = "grammar.txt";

	readFile(fileName);

	/*
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		//if(lhs == "VarDeclList")
			if(nullable(lhs)) {
				cout << lhs << " is nullable." << endl;
			}
	} */

	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		firstTable[lhs] = getFirst(lhs);
	}


	/* show all elements in the firstTable */
	map<LHS, set<string> >:: iterator itFirst;
	set<string>:: iterator itSet;

	for (itFirst = firstTable.begin(); itFirst != firstTable.end(); itFirst++) {
		LHS lhs = itFirst -> first;
		if (isNonterminal(lhs)) {
			cout << lhs + ": ";
		
			for (itSet = firstTable[lhs].begin(); itSet != firstTable[lhs].end(); itSet++) {
				cout << *itSet + " ";	
			}
			cout << endl;
		}
	}


	return 0;	
}
