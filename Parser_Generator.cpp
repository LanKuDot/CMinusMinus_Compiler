#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
using namespace std;

typedef string LHS;
// RHS is a 2-d vector storing grammar
typedef vector< vector<string> > RHS;
typedef map<LHS, RHS> GRAMMAR;

void readFile(const char *, GRAMMAR &);
bool isUppercase(string);
bool nullable();
void first();
void follow();


void readFile(const char * fileName, GRAMMAR & grammar) {
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

bool isUppercase(string s) {
	if(s.at(0) >= 65 && s.at(0) <= 90) {
		return true;
	} else {
		return false;
	}
}

bool nullable(GRAMMAR grammar, LHS lhs) {
	RHS rhs = grammar.find(lhs) -> second;
	//cout << "Check nullable " + lhs << endl;
	// check all rules
	for (int i = 0; i != rhs.size(); i++) {
		// check all elements in the rule
		for (int j = 0; j != rhs[i].size(); j++) {
			// check if the element is uppercase(nonterminal)
			if(isUppercase(rhs[i][j])) {
				if(nullable(grammar, rhs[i][j])) {
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
	// now we have checked all the rules, but none of them are nullable,
	// then return false.
	return false;
}


int main() {
	GRAMMAR grammar;
	GRAMMAR::iterator it;
	const char * fileName = "grammar.txt";

	readFile(fileName, grammar);
	
	for (it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it -> first;
		//if(lhs == "VarDeclList")
			if(nullable(grammar, lhs)) {
				cout << lhs << " is nullable." << endl;
			}
	}

	return 0;	
}
