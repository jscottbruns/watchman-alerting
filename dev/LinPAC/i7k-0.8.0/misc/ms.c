/*
 * ms.c
 *
 * another microsoft wrapping test
 *
 */
#include "msw.h"

main()
{
	char *cmd, *str;
	char rbuf[256];
	int r, i;
	WORD texp;

	Open_Com(COM2, 9600, Data8Bit, NonParity, OneStopBit);

	// cmd = "#02\r";
	cmd = "$022\r";
	// cmd = "%0202080600\r"; /* non-check-sum */
	// cmd = "%0202080640\r"; /* check-sum */

	Send_Cmd(COM2, cmd, 0);
	Receive_Cmd(COM2, rbuf, 40, 0);
	printf("response: %s\n", rbuf);

	Close_Com(COM2);
}
