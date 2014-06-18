#ifndef QUADRUPLE_GTR
#define QUADRUPLE_GTR

#include <vector>

using namespace std;

typedef struct Quadruple_element {
	char op[8], arg1[8], arg2[8], result[8];
} Quadruple;

/* Declared in MachineCodeGTR.cpp */
extern vector< Quadruple > quadruples;

void QuadrupleGenerator();

#endif /* QUADRUPLE_GTR */
