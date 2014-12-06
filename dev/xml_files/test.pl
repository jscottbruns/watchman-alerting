#!perl
use Win32::SerialPort;

my $type = $ARGV[0];
my $msg;

if ( $type =~ /^set(.*)$/ ) {

	$msg = "<ID01><PT><CM><FT>" if ( $1 eq 'd');
	$msg = "<ID01><PA><CP><FQ>[48] APT FIRE" if ( $1 eq 'f' );
	$msg = "<ID01><PB><SA><CP><FS>[48] ENGINE/TRUCK - 9959 GOODLUCK ROAD (GREENBELT RD & DEAD END) - APARTMENT FIRE " if ( $1 eq 's' );
} elsif ( $type =~ /^run(.*)$/ ) {

	if ( ! $1 ) {
	
		print "No page specified\n";
		exit;
	}

	my $p = uc $1;
	$msg = "<ID01><RP$p>";
} elsif ( $type =~ /^d(.*)$/ ) {

	my $p = uc $1;
	$msg = "<ID01><DP$p>";
} elsif ( $type eq 'rst' ) {

	$msg = "<ID01><RST>";
} else {

	print "Error, no argument provided\n";
	exit;
}
	
if ( &write_serial($msg) ) {

	print "Successful\n";
	exit;
}

print "Failed\n";

sub write_serial
{
    my $msg = shift;	

	unless ( system("echo \"$msg\" > COM1") ) {
	
		print "error writing to serial: $!\n";
		return undef;
	}
	
	return 1;
}