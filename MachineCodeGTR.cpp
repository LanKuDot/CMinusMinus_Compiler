#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>

#include "QuadrupleGenerator.h"
#include "MachineCodeGTR.h"
#include "config.h"

using namespace std;

/* Would get quadruples from QuadrupleGenerator */
vector< Quadruple > quadruples;

typedef struct variable_address {
	char id[8];		// The identifier
	char type[8];	// The type of identifier
	int index;		// The address of this variable
} varAddr;

typedef struct register_info {
	bool isUsing;	// Someone is using this register
	char id[8];		// Who is using this register
} reg_state;

#define REGISTER_LEN	8

vector< varAddr > pageTable;
reg_state reg[REGISTER_LEN];	// TM has only 8 registers.

int searchAddress( char *id );
int findFreeRegister();
int findRegister( char *id );

void machineCodeGenerator()
{
	int line_number = 0, memory_used = 0, i;
	FILE *fp = fopen( TM_CODE_FILE, "w" );
	Quadruple target;

	// Initialize the registers
	for ( i = 0; i < REGISTER_LEN; ++i )
		reg[i].isUsing = false;
	// reg[7] is program counter
	reg[7].isUsing = true;
	strcpy( reg[7].id, "PC" );

	for ( i = 0; i < quadruples.size(); ++i )
	{
		target = quadruples.at(i);

		/* Variable declaration
		 * VARDECL type  none  id */
		if ( strcmp( target.op, "VARDECL" ) == 0 )
		{
			varAddr tmp;
			strcpy( tmp.id, target.result );
			strcpy( tmp.type, target.arg1 );
			tmp.index = memory_used++;

			pageTable.push_back( tmp );
		}
		/* Array declaration
		 * ARRAYDE type num id */
		else if ( strcmp( target.op, "ARRAYDE" ) == 0 )
		{
			varAddr tmp;
			strcpy( tmp.id, target.result );
			strcpy( tmp.type, target.arg1 );
			// Only store the address of begin of array
			tmp.index = memory_used;
			
			memory_used += atoi( target.arg2 );

			pageTable.push_back( tmp );
		}
		/* Assign
		 * = Arg1 none result */
		else if ( strcmp( target.op, "=" ) == 0 )
		{
			int address = searchAddress( target.arg1 );
			int regIndex, addressRegIndex;

			/* Load Arg1 to register */
			// Arg1 is a variable
			if ( address != -1 )
			{
				regIndex = findFreeRegister();
				addressRegIndex = findFreeRegister();
				reg[regIndex].isUsing = true;

				// Load address
				fprintf( fp, "%d: LDC  %d, %d(0)\n", line_number++, addressRegIndex, address );
				// Load the value from memory
				fprintf( fp, "%d: LD   %d, 0(%d)\n", line_number++, regIndex, addressRegIndex );
			}
			// Arg1 is a temporarily variable
			else if ( target.arg1[0] == 't' )
			{
				// Find which register is used by this variable
				regIndex = findRegister( target.arg1 );
			}
			// Constant
			else
			{
				regIndex = findFreeRegister();
				reg[regIndex].isUsing = true;

				// Load the const to register
				fprintf( fp, "%d: LDC  %d, %d(0)\n", line_number++, regIndex, atoi( target.arg1 ) );
			}

			address = searchAddress( target.result );

			/* Write to Result */
			// Result is a variable
			if ( address != -1 )
			{
				addressRegIndex = findFreeRegister();

				// Load address
				fprintf( fp, "%d: LDC  %d, %d(0)\n", line_number++, addressRegIndex, address );
				// Write to memory
				fprintf( fp, "%d: ST   %d, 0(%d)\n", line_number++, regIndex, addressRegIndex );
			}
			// Result is a temproary value
			else 
			{
				int tmpVarReg = findFreeRegister();
				reg[tmpVarReg].isUsing = true;

				// Generate 0 register
				addressRegIndex = findFreeRegister();
				fprintf( fp, "%d: LDC  %d, 0(0)\n", line_number++, addressRegIndex );
				// Write to register
				fprintf( fp, "%d: ADD  %d, %d, %d\n", line_number++, tmpVarReg, regIndex, addressRegIndex );
			}

			reg[regIndex].isUsing = false;
		}
	}

	// Halt the TM
	fprintf( fp, "%d: HALT 1, 0, 0\n", line_number );

	fclose( fp );

}	// end of machineCodeGenerator()

/* Get the index of the variable in the memory.
 *
 * - *id: [in] The identifier of variable
 *
 * Return the index of the variable in the memory, -1,
 * if not found.
 */
int searchAddress( char *id )
{
	int i;

	for ( i = 0; i < pageTable.size(); ++i )
	{
		if ( strcmp( pageTable.at(i).id, id ) == 0 )
			return i;
	}

	return -1;

}	// end of searchAddress();

/* Find a free register.
 *
 * Return the id of register.
 * Return -1 if there is no free register.
 */
int findFreeRegister()
{
	int i;

	// reg[7] is always used.
	for ( i = 0; i < REGISTER_LEN - 1; ++i )
	{
		if ( !reg[i].isUsing )
			return i;
	}

	return -1;
}

/* Find the register this variable is using
 *
 * - *id: [in] The id
 *
 * Return the index of register.
 * Return -1 if not found. */
int findRegister( char *id )
{
	int i;

	// reg[7] is always used.
	for ( i = 0; i < REGISTER_LEN - 1; ++i )
	{
		if ( reg[i].isUsing &&
				strcmp( reg[i].id, id ) == 0 )
			return i;
	}

	return -1;
}
