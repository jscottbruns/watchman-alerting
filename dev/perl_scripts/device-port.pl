#!/usr/bin/perl
use IPC::Run qw( run timeout );
use File::Spec;

my ($xsltstr, $ipcout, $ipcerr);

my @cmd = qw( stty -F /dev/ttyUSB0 9600 -opost -ocrnl -onlcr cs7 parenb -parodd clocal );

#print "Running STTY\n";
#run \@cmd, \undef, \$ipcout, \$ipcerr, timeout( 10 ) or die "IPC fatal: $?";
#print "TTY Err: $ipcerr\n" if $ipcerr;
#print "TTY Out: $ipcout\n" if $ipcout;

my $ipcin = File::Spec->catfile( '/home/jsbruns/workspace/WatchmanAlerting/audio', 'preIncAlert.mp3' );
run [ 'mp32rtp', '-i', '192.168.1.210', '-p', '4445' ], $ipcin, \undef, \$ipcerr, timeout( 10 ) or die "IPC fatal: $?";
exit;
print "Running xslt preprocessor\n";
my @cmd = qw( xsltproc bbxml/xml/alphasign.xsl dev/testserial.xml );
run [ 'xsltproc', 'bbxml/xml/alphasign.xsl', 'dev/testserial.xml' ], '>/dev/ttyUSB0', '2>', \$ipcerr, timeout( 10 ) or die "IPC fatal: $?";
print "TTY Err: $ipcerr\n" if $ipcerr;
print "TTY Out: $ipcout\n" if $ipcout;

exit;
print "Writing to signboards [ $ipcout ]\n";
my @cmd = qw( echo $ipcout );
run [ 'echo', $ipcout ], '>/dev/ttyUSB0', \$ipcerr, timeout( 10 ) or die "IPC fatal: $?";
print "TTY Err: $ipcerr\n" if $ipcerr;

