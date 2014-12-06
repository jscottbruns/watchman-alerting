#include "errexit.h"

/*************************************************************************
 * errexit
 * print an error message and exit
 *************************************************************************/
int errexit(format, va_alist)
char *format;
va_dcl
	// void errexit( const char *format, ... )
{
	va_list args;

	va_start(args);
	// _doprnt( format, args, stderr );
	vfprintf(stderr, format, args);
	va_end(args);

	exit(1);
}
