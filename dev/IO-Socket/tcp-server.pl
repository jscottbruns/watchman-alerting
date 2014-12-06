#!/usr/bin/perl
use strict;
use IO::Socket;
use Date::Format;

$SIG{CHLD} = 'IGNORE'; #ignore child processes to prevent zombies

my $socket = IO::Socket::INET->new(
	LocalPort	=>	'8080',
	Proto		=>	'tcp',	 
	Reuse		=>	1
);

die "Cant't create a listening socket: $@" unless $socket;

$socket->listen();
$socket->autoflush(1);

write_log("Server ready. Waiting for connections ... \n");

while ( my $connect = $socket->accept() ) {

	write_log("Spawning child process to handle connection\n");
	my $child;
	
	if ( ! ( $child = fork() ) ) {

		write_log("Child process spawned, reading input buffer\n");
		
		write_log("Tired, sleeping for a bit...");
		sleep 5;
		
		my ($buf, $input);

		while ( defined ( $buf = <$connect> ) ) {
			$input .= $buf;
		}
		
		write_log("Done reading input buffer, closing socket\n");
		
		$socket->close;
		read_input($input);

		write_log("Finished with data parsing tasks\n");
		exit 0;

	} else {
		
		write_log("Incoming connection from " . $connect->peerhost() . ":" . $connect->peerport() . "\n");
		$connect->close();
	}					
}

write_log("Finishing up, closing main socket\n");

$socket->close();

write_log("Done.\n");

sub read_input 
{
	my $input = shift;

	write_log("Received Input: " . $input . "\n");
}

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
