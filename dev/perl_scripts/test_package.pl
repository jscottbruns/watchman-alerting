#!perl

BEGIN
{
	push(@INC, "./lib");
}

use Watchman::SOAP;

my $a = Watchman::SOAP->init( {
	'a'	=>	1,
	'b'	=>	2
} );

print "Done";