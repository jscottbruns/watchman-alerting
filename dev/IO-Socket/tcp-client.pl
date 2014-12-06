#!/usr/bin/perl
use strict;
use IO::Socket;
use Date::Format;

my $port = 8080;
my $file = $ARGV[0];

if ( ! $file || ! -f $file ) {

	&write_log("Invalid filename specified");
	exit;	
} 

write_log("Preparing to send $file to remote WatchmanAlerting site.\n");

$SIG{CHLD} = 'IGNORE'; #ignore child processes to prevent zombies

my $socket = IO::Socket::INET->new(
	Proto		=>	'tcp',
	PeerAddr	=>	'localhost',
	PeerPort	=>	$port,
	Reuse		=>	1,
);

die "Cant't create a listening socket: $@" unless $socket;

write_log("Connected to " . $socket->peerhost() . ":" . $socket->peerport() . " \n");

$socket->autoflush(1);

open OUTPUT, $file;

write_log("Writing text file to socket stream\n");

while ( <OUTPUT> ) {

	print $socket $_;	
}

write_log("Done writing data, closing socket\n");

$socket->close();

write_log("Done.");

sub write_log
{
    my $msg = shift;
    my $err = shift;
    my ($package, $file, $line) = caller;

    $msg =~ s/\n$//;
    $msg .= " (" . $$ . ")" if substr($$, 0, 1) eq '-';
    $msg = "[" . Date::Format::time2str('%C', time) . "] [" . $package . ":" . $line . "] " . ($err ? "[error] " : undef) . " $msg\n";

    print STDOUT $msg;
}

sub trim
{
    my $string = shift;
    if ( $string ) {
        $string =~ s/^\s+//;
        $string =~ s/\s+$//;
        return $string;
    }
}