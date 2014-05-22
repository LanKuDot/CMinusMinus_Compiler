#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
using namespace std;

typedef string LHS;
typedef vector< vector<string> > RHS;

void readFile(map<LHS, RHS>);
void nullable();
void first();
void follow();

void readFile(map<LHS, RHS> grammar) {
	ifstream fin ("grammar.txt");
	string line;
	int index = 0;	

	if (fin.is_open()) {
		while (getline(fin, line)) {
			LHS lhs;
			RHS rhs;
			if(line.at(0) != ' ') {
				cout << "lhs : " << line << endl;
				lhs = line;
			} else {
				cout << "rhs : " << endl;
				if(getline(fin, line) && line.at(0) == ' '){
					cout << "rule" << index << " : "<< line << endl;	
					istringstream iss(line);
					vector<string> tmp;
					do {
						string token;
						iss >> token;
						cout << "SubString : " << token << endl;
						tmp.insert(token);
					} while (iss)
					rhs.push_back(vector<string>());
					index++;
				}
			}
			grammar.insert(pair<LHS, RHS>(lhs, rhs));
		}
		fin.close();	
	} else {
		cout << "Unable to open file." << endl;
	}


	for (map<LHS, RHS>::iterator it = grammar.begin(); it != grammar.end(); it++) {
		LHS lhs = it.first;
		RHS rhs = it.second;
		cout << lhs << endl;
		for (RHS::iterator j = rhs.begin(); j != rhs.end(); j++) {
			for (vector<string>::iterator k = rhs[j].begin(); k != rhs.begin(j); k++) {
				cout << rhs[j][k];
			}
			cout << endl;
		}
	}

	return;	
}

int main() {
	map<LHS, RHS> grammar;
	readFile(grammar);
	// read in grammar.txt
	
	return 0;	
}
