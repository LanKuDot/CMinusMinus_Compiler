#include "simple_itoa.h"

/* simple_itoa could only convert 7 digits positive integer
 * to string.
 * - value: [in] the integer to convert
 *
 * Return the converted string. */
char *simple_itoa( int value )
{
	static char tmp[8];
	char *p = &tmp[7];

	*p = '\0';

	if ( value == 0 )
		*--p = '0';
	else
	{
		for ( ; value; value /= 10 )
			*--p = "0123456789" [ value % 10 ];
	}

	return p;
}
