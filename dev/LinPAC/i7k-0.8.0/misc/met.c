/*
 *  memory eatting test
 */
#include "msw.h"

main()
{
	unsigned int r, i;

	r = 0;
	i = 0;
	while (1) {
		r += Open_Com(COM2, 9600, Data8Bit, NonParity, OneStopBit);
		r += Close_Com(COM2);
		printf("loop %u %d\n", ++i, r);
		// break;
	}
}
