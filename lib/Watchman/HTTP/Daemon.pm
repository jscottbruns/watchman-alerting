package Watchman::HTTP::Daemon;

use strict;
use HTTP::Daemon;
use vars qw(@ISA $VERSION);
@ISA = qw(HTTP::Daemon);

$VERSION = '0.3';

sub product_tokens
{
	my $ver = shift;
    return "Powered by: Firehouse Watchman Control Server/$ver";
}

1;